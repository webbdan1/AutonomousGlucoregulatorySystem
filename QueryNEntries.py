################################################################################
# FILE: QueryNEntries.py
#
# ABSTRACT:
# Utility script used by AGS to query the most recent 6 blood glucose entries
# from the AGS database.
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
with open(os.path.join(os.getcwd(), 'QueryNEntries/config.ini'), 'r') as 
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

def query():
    '''
    query the AGS database for 6 bg entries and output to console for AGS
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
        query = ("SELECT BG FROM tblScrapedData ORDER BY id desc limit 6")

        try:
            cursor.execute(query)
            row = cursor.fetchone()
            while row is not None:
                print(row[0])
                row = cursor.fetchone()

            cnx.commit()
            cursor.close()
            cnx.close()
        except mysql.connector.Error as err:
            #print(err)
            p=0


def main():
    query()

if __name__ == '__main__':

    main()
