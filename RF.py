################################################################################
# FILE: RF.py
#
# ABSTRACT:
# Utility script used by AGS to store the results from a RandomForest model
# prediction in the AGS database.
#
# DOCUMENTS:
# 
#
# AUTHOR:
# Daniel Webb
#
# CREATION DATE:
# 06/10/2019
#
# NOTES:
#
################################################################################

import mysql.connector
from mysql.connector import errorcode
import configparser
import os

Config = configparser.SafeConfigParser()
with open(os.path.join(os.getcwd(), 'RF/config.ini'), 'r') as configfile:
    Config.readfp(configfile)
    MAX_AUTHFAILS = Config.get("dexcomshare", "max_auth_fails")
    DEXCOM_ACCOUNT_NAME = Config.get("dexcomshare", "dexcom_share_login")
    DEXCOM_PASSWORD = Config.get("dexcomshare", "dexcom_share_password")
    Config.readfp(configfile)
    DB_USER = Config.get("database", "user")
    DB_PASSWORD = Config.get("database", "password")
    DB_HOST = Config.get("database", "host")
    DB_NAME = Config.get("database", "name")

def getID():
    '''
    Get the ID from the tblScrapedData that this prediction entry is associated
    with so they may have matching unique keys for later use.
    :return: ID 
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
        getID = ("SELECT * FROM tblScrapedData ORDER BY ID DESC LIMIT 1")
        try:
            cursor.execute(getID)

            id = cursor.fetchone()
            id = id[0]
            #while id is not None:
                #print(id)
                #id = cursor.fetchone()

            cursor.close()
            cnx.close()
        except mysql.connector.Error as err:
            #print(err)
            p=0
        return id

def insertPredictions(ID,BG1, BG2, BG3, BG4, BG5, BG6, BG7, BG8, BG9, BG10, BG11, BG12, BG13, BG14, BG15, BG16, BG17, BG18):
    '''
    insert the RandomForest output into the AGS database
    :ID: unique key
    :BG*: The bg value in 5 minute intervals in the future following the current
    bg reading.
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
                # print(err)
                p = 0
        else:
            cursor = cnx.cursor()
            add_scrape = ("INSERT INTO tblRFPredictions "
                          "(ID,BG5,BG10,BG15,BG20,BG25,BG30,BG35,BG40,BG45,BG50,BG55,BG60,BG65,BG70,BG75,BG80,BG85,BG90) "
                          "VALUES (%s,%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)")
            data_scrape = (ID,BG1, BG2, BG3, BG4, BG5, BG6, BG7, BG8, BG9, BG10, BG11, BG12, BG13, BG14, BG15, BG16, BG17, BG18)
            try:
                cursor.execute(add_scrape, data_scrape)
                cnx.commit()
                cursor.close()
                cnx.close()
            except mysql.connector.Error as err:
                print(err)
                p = 0

def getPredictions():
    '''
    Read the prediction data from the file RFResults.txt
    :return: array of data
    '''
    RFArray=[]
    # Load dataset
    file = open("RandomForest/RFResults.txt",'r')
    lines = file.readlines()
    for line in lines:
        line = line.split(',')
        for item in line:
            RFArray.append(item)

    return RFArray

def main():
    '''
    Read the data and insert it to the AGS database
    '''
    rfResult = getPredictions()
    ID =getID()
    insertPredictions(ID,rfResult[0], rfResult[1], rfResult[2], rfResult[3], rfResult[4],
                          rfResult[5], rfResult[6], rfResult[7], rfResult[8], rfResult[9],
                          rfResult[10], rfResult[11], rfResult[12], rfResult[13], rfResult[14],
                          rfResult[15], rfResult[16], rfResult[17])


if __name__ == '__main__':
    main()
