/******************************************************************************
** FILE: InsulinDataEntryFactory.cpp
**
** ABSTRACT:
** Uses the base abstract factory interface to construct
** Insulin data entry objects.
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

#include "InsulinDataEntryFactory.h"
#include "InsulinDataEntry.h"

/*-----------------------------------------------------------------------------
Name:     createDataEntry
Purpose:  Use raw data to fill the various fields of an Insulin DataEntry and
          return the fully constructed result. The raw data is in the following
          order: time in seconds since epoch, value in units.
Receive:  const QStringList& rawData
Return:   InsulinDataEntry*
-----------------------------------------------------------------------------*/
InsulinDataEntry* InsulinDataEntryFactory::createDataEntry
                                           (const QStringList& rawData)
{
    InsulinDataEntry* insulinDataEntry = new InsulinDataEntry();
    QString str = rawData.at(4);
    double dstr = str.toDouble();
    insulinDataEntry->setInsulinOnBoard(dstr);

    double sampleTime = rawData.at(3).toDouble();
    insulinDataEntry->setSampleTime(sampleTime);

    return insulinDataEntry;
}



