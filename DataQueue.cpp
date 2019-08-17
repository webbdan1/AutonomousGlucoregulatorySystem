/******************************************************************************
** FILE: DataQueue.cpp
**
** ABSTRACT:
** The application maintains one DataQueue object which
** is responsible for the query of raw data every 5
** minutes using the DataScraper script, turning that
** raw data into DataEntry objects, and providing
** an interface to the stored data for the rest of
** the application. The data is stored in CircularArray
** containers.
**
** DOCUMENTS:
**
**
** AUTHOR:
** Daniel Webb
**
** CREATION DATE:
** 06/24/2019
**
** NOTES:
**
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <QDir>
#include <QString>
#include <iostream>
using std::string;
#include "DataQueue.h"

/*-----------------------------------------------------------------------------
Name:     getFutureInsulinValues
Purpose:  The DataScraper script calculates the estimated plasma insulin
          concentration in 5 minute intervals for 90 minutes after the data
          in question is queried. This container holds those values. The MPC
          uses it to predict future BG in the case of the state space model.
Receive:  N/A
Return:   vector<float> DataQueue
-----------------------------------------------------------------------------*/
vector<float> DataQueue::getFutureInsulinValues()
{
    return m_futureInsulinValues;
}

/*-----------------------------------------------------------------------------
Name:     enqueueBGPrediction
Purpose:  Add a BG prediction value to the CircularArray.
Receive:  double* prediction
Return:   N/A
-----------------------------------------------------------------------------*/
void DataQueue::enqueueBGPrediction(double* prediction)
{
    m_predictions.enqueue(prediction);
}

/*-----------------------------------------------------------------------------
Name:     dequeueBGPrediction
Purpose:  Remove a BG prediction value from the CircularArray.
Receive:  N/A
Return:   double*
-----------------------------------------------------------------------------*/
double* DataQueue::dequeueBGPrediction()
{
    return m_predictions.dequeue();
}

/*-----------------------------------------------------------------------------
Name:     dequeueBGEntry
Purpose:  Remove a BGDataEntry from the CircularArray.
Receive:  N/A
Return:   BGDataEntry *
-----------------------------------------------------------------------------*/
BGDataEntry *DataQueue::dequeueBGEntry()
{
    return m_bgDataEntries.dequeue();
}

/*-----------------------------------------------------------------------------
Name:     enqueueBGEntry
Purpose:  Add a BGDataEntry to the CircularArray.
Receive:  BGDataEntry* entry
Return:   N/A
-----------------------------------------------------------------------------*/
void DataQueue::enqueueBGEntry(BGDataEntry* entry)
{
    m_bgDataEntries.enqueue(entry);
}

/*-----------------------------------------------------------------------------
Name:     dequeueInsulinEntry
Purpose:  Remove an InsulinDataEntry from the CircularArray.
Receive:  N/A
Return:   InsulinDataEntry *
-----------------------------------------------------------------------------*/
InsulinDataEntry *DataQueue::dequeueInsulinEntry()
{
    return m_insulinDataEntries.dequeue();
}

/*-----------------------------------------------------------------------------
Name:     enqueueInsulinEntry
Purpose:  Add an InsulinDataEntry to the CircularArray.
Receive:  InsulinDataEntry* entry
Return:   N/A
-----------------------------------------------------------------------------*/
void DataQueue::enqueueInsulinEntry(InsulinDataEntry* entry)
{
    m_insulinDataEntries.enqueue(entry);
}

/*-----------------------------------------------------------------------------
Name:     getQueueCapacity
Purpose:  Wrapper to return capacity of CircularArray container.
Receive:  N/A
Return:   int
-----------------------------------------------------------------------------*/
int DataQueue::getQueueCapacity() const
{
    return m_bgDataEntries.getCapacity();
}

/*-----------------------------------------------------------------------------
Name:     setQueueCapacity
Purpose:  Wrapper to set capacity of CircularArray container for all containers
          used by the DataQueue instance.
Receive:  const int &capacity
Return:   N/A
-----------------------------------------------------------------------------*/
void DataQueue::setQueueCapacity(const int &capacity)
{
    m_bgDataEntries.setCapacity(capacity);
    m_insulinDataEntries.setCapacity(capacity);
    m_predictions.setCapacity(capacity);
}

/*-----------------------------------------------------------------------------
Name:     getQueueSize
Purpose:  Wrapper to get number of objects in the CircularArray container.
Receive:  N/A
Return:   int
-----------------------------------------------------------------------------*/
int DataQueue::getQueueSize() const
{
    return m_bgDataEntries.getSize();
}

/*-----------------------------------------------------------------------------
Name:     getFirstBGEntry
Purpose:  Wrapper to get earliest BGDataEntry in the CircularArray container.
Receive:  N/A
Return:   BGDataEntry*
-----------------------------------------------------------------------------*/
BGDataEntry* DataQueue::getFirstBGEntry() const
{
    return m_bgDataEntries.getFirstValue();
}

/*-----------------------------------------------------------------------------
Name:     getFirstBGEntry
Purpose:  Wrapper to get most recent BGDataEntry in the CircularArray
          container.
Receive:  N/A
Return:   BGDataEntry*
-----------------------------------------------------------------------------*/
BGDataEntry* DataQueue::getLastBGEntry() const
{
    return m_bgDataEntries.getLastValue();
}

/*-----------------------------------------------------------------------------
Name:     getNInsulinEntries
Purpose:  Returns most recent n entries stored in the Insulin queue. Checks
          first to make sure the queue contains at least n entries.
          *NOTE: I plan to have this routine behave similarly to getNBGEntries
                 in the future.
Receive:  int number of requested entires
Return:   vector<InsulinDataEntry *>
-----------------------------------------------------------------------------*/
vector<InsulinDataEntry *> DataQueue::getNInsulinEntries(int n)
{
    vector<InsulinDataEntry*> values;
    if(m_insulinDataEntries.getSize()>=n){
        values = m_insulinDataEntries.getNValues(n);
    }
    else{
        std::cout<<"Not enough entries in queue!"<<std::endl;
    }
    return values;
}

/*-----------------------------------------------------------------------------
Name:     getNBGEntries
Purpose:  Returns most recent n values (not entries) stored in the BG queue.
          Uses the QueryNEntries script to query the AGS mysql database for the
          entries since they may not be available to the application if
          this routine is called before the queue has n entries.
          *NOTE: I plan to have this routine construct BGDataEntry objects
                 in the future.
Receive:  int number of requested entires
Return:   vector<int>
-----------------------------------------------------------------------------*/
vector<int> DataQueue::getNBGEntries(int n)
{
    vector<int> values;
    string bgData;

    string RFRP = "/QueryNEntries/QueryNEntries";
    string RFFQP = QDir::currentPath().toStdString()+RFRP;

    std::cout << "Opening query reading pipe" << std::endl;
    FILE* pipe = popen(RFFQP.c_str(), "r");
    if (!pipe)
    {
      std::cerr << "Couldn't start command." << std::endl;

    }
    char line[1024];

    while (fgets(line, 1024, pipe))
         bgData += line;
    auto returnCode = pclose(pipe);
    //get predictions and push back to prediction container
    QString stringData = QString::fromUtf8(bgData.c_str());
    QStringList formattedData = stringData.split('\n');
    //Push back all 6 values
    values.push_back(formattedData[0].toInt());
    values.push_back(formattedData[1].toInt());
    values.push_back(formattedData[2].toInt());
    values.push_back(formattedData[3].toInt());
    values.push_back(formattedData[4].toInt());
    values.push_back(formattedData[5].toInt());
    return values;
}

/*-----------------------------------------------------------------------------
Name:     getNPredictionEntries
Purpose:  Returns most recent n entries stored in the Prediction queue. Checks
          first to make sure the queue contains at least n entries.
          *NOTE: I plan to have this routine behave similarly to getNBGEntries
                 in the future.
Receive:  int number of requested entires
Return:   vector<double *>
-----------------------------------------------------------------------------*/
vector<double *> DataQueue::getNPredictionEntries(int n)
{
    vector<double*> values;
    if(m_insulinDataEntries.getSize()>=n){
        values = m_predictions.getNValues(n);
    }
    else{
        std::cout<<"Not enough entries in queue!"<<std::endl;
    }
    return values;
}

/*-----------------------------------------------------------------------------
Name:     getFirstInsulinEntry
Purpose:  Wrapper to get earliest InsulinDataEntry in the CircularArray
          container.
Receive:  N/A
Return:   InsulinDataEntry*
-----------------------------------------------------------------------------*/
InsulinDataEntry *DataQueue::getFirstInsulinEntry() const
{
    return m_insulinDataEntries.getFirstValue();
}

/*-----------------------------------------------------------------------------
Name:     getLastInsulinEntry
Purpose:  Wrapper to get most recent InsulinDataEntry in the CircularArray
          container.
Receive:  N/A
Return:   InsulinDataEntry*
-----------------------------------------------------------------------------*/
InsulinDataEntry *DataQueue::getLastInsulinEntry() const
{
    return m_insulinDataEntries.getLastValue();
}

/*-----------------------------------------------------------------------------
Name:     scrapeData
Purpose:  Calls the DataScraper script and pipes the data into a list. From
          there, it uses the factories to construct (x1) insulin and bg data
          entry objects and stores them in the queue. It also stores future
          insulin values in the respective queue.
Receive:  N/A
Return:   bool true if data has been scraped, false if there was no new data
          available.
-----------------------------------------------------------------------------*/
bool DataQueue::scrapeData()
{
    string bgData;
    //construct the command string to execute
    string dexcomShareServerRP = "/DataScraper/DataScraper";
    string dexcomShareServerFQP =
            QDir::currentPath().toStdString()+dexcomShareServerRP;
    //call the script and pipe output to bdData
    std::cout << "Opening Dexcom reading pipe" << std::endl;
    FILE* pipe = popen(dexcomShareServerFQP.c_str(), "r");
    if (!pipe)
    {
        std::cerr << "Couldn't start command." << std::endl;
    }
    char line[512];
    //data goes into line array, bgData records it
    while (fgets(line, 512, pipe))
         bgData += line;
    //close pipe
    auto returnCode = pclose(pipe);
    //create DataEntry objects to add to the queue
    InsulinDataEntry* aInsulinDataEntry;
    BGDataEntry* aBGDataEntry;
    //if indeed data has been scraped
    if(bgData.size()){
        BGDataEntryFactory aBGDataEntryFactory;
        InsulinDataEntryFactory aInsulinDataEntryFactory;
        QString stringData = QString::fromUtf8(bgData.c_str());
        //put data in list, comma-separated
        QStringList formattedData = stringData.split(",");
        //use the factory interface to create the DataEntry objects
        aBGDataEntry = aBGDataEntryFactory.createDataEntry(formattedData);
        aInsulinDataEntry =
        aInsulinDataEntryFactory.createDataEntry(formattedData);
        //figure out the sample time of the last recorded bg data entry
        if(aBGDataEntry->getSampleTime()){
            if(m_bgDataEntries.getSize()){
                double lastTime = 0.00;
                if(m_bgDataEntries.getSize()==1){                    
                    lastTime =
                    m_bgDataEntries.getFirstValue()->getSampleTime();
                }
                else{                   
                    lastTime = m_bgDataEntries.getLastValue()->getSampleTime();
                }
                //if the time is less than the sample time for this entry,
                //it is a new one
                if( (lastTime) < (aBGDataEntry->getSampleTime()) ){
                    //therefore, we can add it to the queue
                    m_insulinDataEntries.enqueue(aInsulinDataEntry);
                    m_bgDataEntries.enqueue(aBGDataEntry);
                    //remember to store future insulin values produced by the
                    //script
                    for(int j=4;j<formattedData.size();j++){
                        m_futureInsulinValues.push_back
                        (formattedData[j].toFloat());
                    }
                    //tell the caller we have new data
                    return true;
                }
                //otherwise this data is a repeat of old data, so
                //don't add it and return false
                else{                   
                    std::cout << "Not a new reading" << std::endl;
                    return false;
                }
            }
            //clearly new data, we can add it to the queues
            else{
                m_insulinDataEntries.enqueue(aInsulinDataEntry);
                m_bgDataEntries.enqueue(aBGDataEntry);
                for(int j=4;j<formattedData.size();j++){
                    m_futureInsulinValues.push_back
                    (formattedData[j].toFloat());
                }
                return true;
            }
        }
        return false;
    }
    return false;
}

/*-----------------------------------------------------------------------------
Name:     printData
Purpose:  Wrapper to print BG and Insulin data in the queues.
Receive:  N/A
Return:   N/A
-----------------------------------------------------------------------------*/
void DataQueue::printData()
{
    m_bgDataEntries.print();
    m_insulinDataEntries.print();
}
