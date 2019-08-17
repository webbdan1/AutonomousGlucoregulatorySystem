################################################################################
# FILE: DataScraper.py
#
# ABSTRACT:
# Queries the Dexcom Share Server using the RESTful API for the most recent 
# Blood Glucose reading as well as insulin data entered by the user. Puts this 
# data in the AGS database and returns it to the console for the caller (AGS)
#
# DOCUMENTS:
# 
#
# AUTHOR:
# Daniel Webb
#
# CREATION DATE:
# 05/22/2019
#
# NOTES:
#
################################################################################


#!/usr/bin/env python
import mysql.connector
from mysql.connector import errorcode
import configparser
import datetime
from datetime import date
import os
import re
import requests
import time
import json
from math import exp
from requests.exceptions import ConnectionError

Config = configparser.SafeConfigParser()
with open(os.path.join(os.getcwd(), 'DataScraper/config.ini'), 'r') as 
configfile:
    Config.readfp(configfile)
    MAX_AUTHFAILS = Config.get("dexcomshare", "max_auth_fails")
    DEXCOM_ACCOUNT_NAME = Config.get("dexcomshare", "dexcom_share_login")
    DEXCOM_PASSWORD = Config.get("dexcomshare", "dexcom_share_password")
    Config.readfp(configfile)
    DB_USER = Config.get("database", "user")
    DB_PASSWORD = Config.get("database", "password")
    DB_HOST = Config.get("database", "host")
    DB_NAME = Config.get("database", "name")
    PEAK_INSULIN = Config.get("patient", "PEAK_INSULIN")
    DIA = Config.get("patient", "DIA")


CHECK_INTERVAL = 60 * 2.5
AUTH_RETRY_DELAY_BASE = 2
FAIL_RETRY_DELAY_BASE = 2

MAX_FETCHFAILS = 10
RETRY_DELAY = 60 # Seconds
LAST_READING_MAX_LAG = 60 * 15

last_date = 0
notify_timeout = 5
notify_bg_threshold = 170
notify_rate_threshold = 10
tempsilent = 0


class Defaults:
    applicationId = "d89443d2-327c-4a6f-89e5-496bbb0317db"
    agent = "Dexcom Share/3.0.2.11 CFNetwork/711.2.23 Darwin/14.0.0"
    login_url = "https://share1.dexcom.com/ShareWebServices/Services/" +\
        "General/LoginPublisherAccountByName"
    accept = 'application/json'
    content_type = 'application/json'
    LatestGlucose_url = "https://share1.dexcom.com/ShareWebServices/" +\
        "Services/Publisher/ReadPublisherLatestGlucoseValues"
    sessionID = None
    MIN_PASSPHRASE_LENGTH = 12
    last_seen = 0


# Mapping friendly names to trend IDs from dexcom
DIRECTIONS = {
    "nodir": 0,
    "DoubleUp": 1,
    "SingleUp": 2,
    "FortyFiveUp": 3,
    "Flat": 4,
    "FortyFiveDown": 5,
    "SingleDown": 6,
    "DoubleDown": 7,
    "NOT COMPUTABLE": 8,
    "RATE OUT OF RANGE": 9,
}
keys = DIRECTIONS.keys()


class DataBuffer:
    def __init__(self):
        self.data = {}

    def getData(self):
        return self.data

    def addReading(self,time,bg):
        self.data[time]=bg


glucoseBuffer=DataBuffer()


def calculateIOB(peakParam, activityValue, totalParam, minsAgo, futureFlag):
    '''
    iobCalc returns two variables:
    activityContrib = units of treatment.insulin used in previous minute
    iobContrib = units of treatment.insulin still remaining at a given point in 
    time "Contrib" is used because these are the amounts contributed from 
    pontentially multiple treatment dosages Uses exponential insulin action 
    curve (which takes both a dia and a peak parameter)
    '''
    if(futureFlag):
        minsAgo += futureFlag

    peak = peakParam
    end = activityValue * 60.0
    activityContrib = 0.0
    obContrib = 0.0
    iobContrib = 0.0
    if (minsAgo < end) :
        tau = peak*(1-peak/end)/(1-2*peak/end)
        a = 2*tau/end;
        S = 1/(1-a+(1+a)*exp(-end/tau))
        activityContrib = float(totalParam) * float((S / (tau**2)) * minsAgo 
        * (1 - minsAgo / end) * exp(-minsAgo/tau))
        iobContrib = totalParam * (1 - S * (1 - a) * ((pow(minsAgo, 2) / 
        (tau * end * (1 - a)) - minsAgo / tau - 1) * exp(-minsAgo / tau) + 1))
    return iobContrib

def insertToDB(IOB,ID,DB):
    '''
    Inserts insulin reading to AGS database
    :IOB: insulin on board
    :ID: unique key
    :DB: mysql-connector object representing the database
    '''
    if not IOB:
        IOB=0.0

    cursor = DB.cursor()
    add_scrape = ("INSERT INTO tblIOB "
                    "(ID,Value) "
                    "VALUES (%s, %s)")
    data = (ID,IOB)
    try:
        cursor.execute(add_scrape, data)
        DB.commit()
        cursor.close()
        DB.close()
    except mysql.connector.Error as err:
        # print(err)
        p=0


def getMinsAgo(time):
    '''
    Returns the time elapsed in minutes since provided time
    :time: the time in "%Y-%m-%dT%H:%M:%S.%fZ" format
    :return: minutes elapsed since time
    '''
    fmt = "%Y-%m-%dT%H:%M:%S.%fZ"
    now = datetime.datetime.now()
    nowStr = now.strftime(fmt)
    nowDt = datetime.datetime.strptime(nowStr, fmt)
    dtsDt = datetime.datetime.strptime(time, fmt)
    replaceHour = ((dtsDt.hour - 4) % 24)
    if(dtsDt.hour < 4):
        replaceDay = dtsDt.day - 1
        newDt = dtsDt.replace(hour=replaceHour,day=replaceDay)
    else:
        newDt = dtsDt.replace(hour=replaceHour)

    elapsed = nowDt - newDt
    return abs(elapsed/datetime.timedelta(minutes=1))


def login_payload(opts):
    '''
    Build payload for the auth api query
    :opts: authentication info
    :return: paylaod
    '''
    body = {
        "password": opts.password,
        "applicationId": opts.applicationId,
        "accountName": opts.accountName
        }
    return body


def authorize(opts):
    '''
    Login to dexcom share and get a session token 
    :opts: authentication info
    :return: session token
    '''

    url = Defaults.login_url
    body = login_payload(opts)
    headers = {
            'User-Agent': Defaults.agent,
            'Content-Type': Defaults.content_type,
            'Accept': Defaults.accept
            }

    return requests.post(url, json=body, headers=headers)


def fetch_query(opts):
    '''
    Build the api query for the data fetch
    :opts: authentication info
    :return: session token
    '''

    url = Defaults.LatestGlucose_url + '?' + 
    "sessionID="+opts.sessionID+"&minutes=1440&maxCount=1"
    return url


def fetch(opts):
    '''
    Fetch latest reading from dexcom share server
    :opts: authentication info
    :return: POST
    '''
    
    url = fetch_query(opts)
    body = {
            'applicationId': 'd89443d2-327c-4a6f-89e5-496bbb0317db'
            }

    headers = {
            'User-Agent': Defaults.agent,
            'Content-Type': Defaults.content_type,
            'Content-Length': "0",
            'Accept': Defaults.accept
            }

    return requests.post(url, json=body, headers=headers)


class Error(Exception):
    """Base class for exceptions in this module."""
    pass


class AuthError(Error):
    """Exception raised for errors when trying to Auth to Dexcome share
    """

    def __init__(self, status_code, message):
        self.expression = status_code
        self.message = message



class FetchError(Error):
    """Exception raised for errors in the date fetch.
    """

    def __init__(self, status_code, message):
        self.expression = status_code
        self.message = message




def parse_dexcom_response(ops, res):
    '''
    Break the dexcom response up into the various fields
    :ops: authentication info
    :res: the dexcom response
    :return: json format of data
    '''
    
    epochtime = int((
                datetime.datetime.utcnow() -
                datetime.datetime(1970, 1, 1)).total_seconds())

    last_reading_time = int(
        re.search('\d+', res.json()[0]['ST']).group())/1000
    reading_lag = epochtime - last_reading_time
    trend = res.json()[0]['Trend']
    mgdl = res.json()[0]['Value']
#        trend_english = DIRECTIONS.keys()[DIRECTIONS.values().index(trend)]
    if mgdl:

        return {
            "bg": mgdl,
            "trend": trend,
            "reading_lag": reading_lag,
            "last_reading_time": last_reading_time
            }



def report_glucose(reading,buffer):
    '''
    Basic output
    :reading: dexcom json response
    :buffer: buffer to add reading to
    '''

    buffer.addReading(reading["last_reading_time"],reading["bg"])


def get_sessionID(opts):
    '''
    Get session ID for dexcom share server login and request
    :opts: authentication info
    :return: session ID
    '''

    authfails = 0
    while not opts.sessionID:
        res = authorize(opts)
        if res.status_code == 200:
            opts.sessionID = res.text.strip('"')

        else:
            if authfails > MAX_AUTHFAILS:
                raise AuthError(res.status_code, res)
            else:

                time.sleep(AUTH_RETRY_DELAY_BASE**authfails)
                authfails += 1
    return opts.sessionID

def insertScrape(BG,TREND,LAG,TIME):
    '''
    Put BG reading in AGS database
    :BG: Blood glucose in mg/dl
    "TREND: Change in BG since last reading in mg/dl
    :LAG: Difference in time between when dexcom sampled the reading and we 
    requested it.
    :return: success code
    '''

    try:
      cnx = mysql.connector.connect(user=DB_USER,
                                    database=DB_NAME,
                                    password=DB_PASSWORD,
                                    host=DB_HOST)
    except mysql.connector.Error as err:
      if err.errno == errorcode.ER_ACCESS_DENIED_ERROR:
        print("Something is wrong with your user name or password")
      elif err.errno == errorcode.ER_BAD_DB_ERROR:
        print("Database does not exist")
      else:
        #print(err)
        p=0
    else:
        cursor = cnx.cursor()
        add_scrape = ("INSERT INTO tblScrapedData "
                        "(ID, BG, TREND, LAG) "
                        "VALUES (%s, %s, %s, %s)")
        data_scrape = (TIME,BG,TREND,LAG)
        try:
            cursor.execute(add_scrape, data_scrape)
            cnx.commit()
            cursor.close()
            cnx.close()
        except mysql.connector.Error as err:
            #print(err)
            p=0


def monitor_dexcom(run_once=False):
    '''
    Standalone engine for data scraper, data goes into buffer. Not used by
    AGS unless run_once=False.
    :run_once: Only query data once
    '''

    opts = Defaults
    opts.accountName = os.getenv("DEXCOM_ACCOUNT_NAME", DEXCOM_ACCOUNT_NAME)
    opts.password = os.getenv("DEXCOM_PASSWORD", DEXCOM_PASSWORD)
    opts.interval = float(os.getenv("CHECK_INTERVAL", CHECK_INTERVAL))

    runs = 0
    fetchfails = 0
    failures = 0
    while True:

        runs += 1
        if not opts.sessionID:
            authfails = 0
            opts.sessionID = get_sessionID(opts)
        try:
            res = fetch(opts)
            if res and res.status_code < 400:
                fetchfails = 0
                reading = parse_dexcom_response(opts, res)
                if len(reading):
                    glucoseBuffer.addReading(reading["last_reading_time"],
                    reading["bg"])
                    if len(glucoseBuffer.data) >= 288:
                        glucoseBuffer.data.clear()
                    if run_once:
                        return reading
                    else:
                        if reading['last_reading_time'] > opts.last_seen:
                            report_glucose(reading,glucoseBuffer)
                            opts.sessionID = "foo"
                            opts.last_seen = reading['last_reading_time']
            else:
                failures += 1
                if run_once or fetchfails > MAX_FETCHFAILS:
                    opts.sessionID = None
                    raise FetchError(res.status_code, res)
                else:
                    if fetchfails > (MAX_FETCHFAILS/2):
                        opts.sessionID = None
                    else:
                        time.sleep(
                            (FAIL_RETRY_DELAY_BASE**authfails))
                            #opts.interval)
                    fetchfails += 1
        except ConnectionError:
            opts.sessionID = None
            if run_once:
                raise
            time.sleep(RETRY_DELAY)
        time.sleep(opts.interval)


def query_dexcom(push_report=False):
    '''
    Run a query on dexcom.
    :push_report: if we wish to send the report somewhere. (not used by AGS)
    :return: data from the reading
    '''

    reading = monitor_dexcom(run_once=True)
    if push_report and reading:
        report_glucose(reading)
    return reading


def main():
    '''
    Query data, put in AGS database, send output to console.
    '''

    data = query_dexcom()
    #python2json = json.dumps(data)
    result = str(data["bg"])+","+str(data["trend"])+","+str(data["reading_lag"])
    +","+str(data["last_reading_time"])
    #print(result)
    BG=str(data["bg"])
    TREND=str(data["trend"])
    LAG=str(data["reading_lag"])
    TIME=str(data["last_reading_time"])
    insertScrape(BG,TREND,LAG,TIME);



    today = date.today()
    todayString = today.isoformat()
    yesterday = date.today() - datetime.timedelta(1)
    yesterdayString= yesterday.isoformat()
    INSULIN = INSULIN = 
    'http://bgcs.herokuapp.com/api/v1/treatments.json?find[insulin][$gte]=0.1'
    url = INSULIN

    # use requests JSON parser to read
    response = requests.get(url).text
    data = json.loads(response)

    i = 0
    resultList=[]
    timeList=[]
    while(True):
        try:
            result1 = data[i]['insulin']
            resultList.append(result1)
            time = data[i]['created_at']
            timeList.append(time)
            i+=1
        except IndexError:
            break

    minsAgoList=[]
    for i in range(len(timeList)):
        minsAgoList.append(getMinsAgo(timeList[i]))


    try:
      cnx = mysql.connector.connect(user=DB_USER,
                                    database=DB_NAME,
                                    password=DB_PASSWORD,
                                    host=DB_HOST)
    except mysql.connector.Error as err:
      if err.errno == errorcode.ER_ACCESS_DENIED_ERROR:
        print("Something is wrong with your user name or password")
      elif err.errno == errorcode.ER_BAD_DB_ERROR:
        print("Database does not exist")
      else:
        #print(err)
        p=0
    else:
        IOB=0
        fiveIOB=0
        tenIOB=0
        fifteenIOB=0
        twentyIOB=0
        twentyFiveIOB=0
        thirtyIOB=0
        I35=0
        I40=0
        I45=0
        I50=0
        I55=0
        I60=0
        I65=0
        I70=0
        I75=0
        I80=0
        I85=0
        I90=0
        #future estimated plasma insulin concentration values
        for i in range(len(resultList)):
            if(minsAgoList[i]<ACTIVE_INSULIN):
                IOB += calculateIOB(PEAK_INSULIN,DIA,resultList[i],minsAgoList[i],0)
                fiveIOB += calculateIOB(PEAK_INSULIN,DIA,resultList[i],minsAgoList[i],5)
                tenIOB += calculateIOB(PEAK_INSULIN, DIA, resultList[i], minsAgoList[i], 10)
                fifteenIOB += calculateIOB(PEAK_INSULIN, DIA, resultList[i], minsAgoList[i], 15)
                twentyIOB += calculateIOB(PEAK_INSULIN, DIA, resultList[i], minsAgoList[i], 20)
                twentyFiveIOB += calculateIOB(PEAK_INSULIN, DIA, resultList[i], minsAgoList[i], 25)
                thirtyIOB += calculateIOB(PEAK_INSULIN, DIA, resultList[i], minsAgoList[i], 30)
                I35 += calculateIOB(PEAK_INSULIN,DIA,resultList[i],minsAgoList[i],35)
                I40 += calculateIOB(PEAK_INSULIN,DIA,resultList[i],minsAgoList[i],40)
                I45 += calculateIOB(PEAK_INSULIN, DIA, resultList[i], minsAgoList[i], 40)
                I50 += calculateIOB(PEAK_INSULIN, DIA, resultList[i], minsAgoList[i], 50)
                I55 += calculateIOB(PEAK_INSULIN, DIA, resultList[i], minsAgoList[i], 55)
                I60 += calculateIOB(PEAK_INSULIN, DIA, resultList[i], minsAgoList[i], 60)
                I65 += calculateIOB(PEAK_INSULIN, DIA, resultList[i], minsAgoList[i], 65)
                I70 += calculateIOB(PEAK_INSULIN,DIA,resultList[i],minsAgoList[i],70)
                I75 += calculateIOB(PEAK_INSULIN,DIA,resultList[i],minsAgoList[i],75)
                I80 += calculateIOB(PEAK_INSULIN, DIA, resultList[i], minsAgoList[i], 80)
                I85 += calculateIOB(PEAK_INSULIN, DIA, resultList[i], minsAgoList[i], 85)
                I90 += calculateIOB(PEAK_INSULIN, DIA, resultList[i], minsAgoList[i], 90)

        insertToDB(IOB,TIME,cnx)

    #console output for AGS Pipe
    result = result+","+str(IOB)+","+str(fiveIOB)+","+str(tenIOB)+","+
    str(fifteenIOB)+","+str(twentyIOB)+","+str(twentyFiveIOB)+","+
    str(thirtyIOB)+","+str(I35)+","+str(I40)+","+str(I45)+","+str(I50)+
    ","+str(I55)+","+str(I60)+","+str(I65)+","+str(I70) + "," + str(I75)
     + "," + str(I80) + "," + str(I85) + "," + str(I90)
    print(result)
    return result

if __name__ == '__main__':

    main()
