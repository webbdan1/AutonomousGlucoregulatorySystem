/******************************************************************************
** FILE: BGDataEntryFactory.cpp
**
** ABSTRACT:
** Uses abstract factory base class interface to create
** a BG data entry. Takes raw data from data scraper
** and fills the various fields of a bg data entry.
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

#include "BGDataEntryFactory.h"
#include "BGDataEntry.h"

/*-----------------------------------------------------------------------------
Name:     createDataEntry
Purpose:  Use raw data to fill the various fields of a BGDataEntry and return
          the fully constructed result. The raw data is in the following order:
          value, trend, query time, sample time. Calculates delay time
          manually.
Receive:  const QStringList& rawData
Return:   BGDataEntry*
-----------------------------------------------------------------------------*/
BGDataEntry* BGDataEntryFactory::createDataEntry(const QStringList& rawData)
{
    BGDataEntry* bgDataEntry = new BGDataEntry();

    bgDataEntry->setValue(rawData.at(0).toInt());
    bgDataEntry->setTrend(rawData.at(1).toInt());

    double sampleTime = rawData.at(3).toDouble();
    bgDataEntry->setSampleTime(sampleTime);

    double scrapeTime = sampleTime + rawData.at(2).toDouble();
    bgDataEntry->setScrapeTime(scrapeTime);

    double delayTime = (sampleTime+300.0)-scrapeTime;
    bgDataEntry->setDelayTime(delayTime);

    return bgDataEntry;
}
