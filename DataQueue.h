/******************************************************************************
** FILE: DataQueue.h
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

#ifndef DATAQUEUE_H
#define DATAQUEUE_H

#include <QVector>
#include "CircularArarray.h"
#include "BGDataEntry.h"
#include "InsulinDataEntry.h"
#include "BGDataEntryFactory.h"
#include "InsulinDataEntryFactory.h"

class DataQueue
{
protected:
    CircularArray<BGDataEntry*> m_bgDataEntries;
    CircularArray<InsulinDataEntry*> m_insulinDataEntries;
    int m_capacity = 288;
    CircularArray<double*> m_predictions;
    vector<float> m_futureInsulinValues;

public:
    DataQueue() = default;
    virtual ~DataQueue() = default;
    DataQueue(DataQueue& buffer) = default;

    vector<float> getFutureInsulinValues();
    void enqueueBGPrediction(double* prediction);
    double* dequeueBGPrediction();
    BGDataEntry* dequeueBGEntry();
    void enqueueBGEntry(BGDataEntry* entry);
    InsulinDataEntry* dequeueInsulinEntry();
    void enqueueInsulinEntry(InsulinDataEntry* entry);
    int getQueueCapacity() const;
    void setQueueCapacity(const int& capacity);
    int getQueueSize() const;
    BGDataEntry* getFirstBGEntry() const;
    InsulinDataEntry *getFirstInsulinEntry() const;
    InsulinDataEntry *getLastInsulinEntry() const;
    BGDataEntry* getLastBGEntry() const;
    vector<InsulinDataEntry*> getNInsulinEntries(int n);
    vector<int> getNBGEntries(int n);
    vector<double *> getNPredictionEntries(int n);
    bool scrapeData();
    void printData();
};

#endif // DATAQUEUE_H
