/******************************************************************************
** FILE: BGDataEntryFactory.h
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

#ifndef BGDATAENTRYFACTORY_H
#define BGDATAENTRYFACTORY_H

#include "AbstractDataEntryFactory.h"
#include "BGDataEntry.h"

class BGDataEntryFactory : public AbstractDataEntryFactory
{
public:
    BGDataEntryFactory() = default;
    BGDataEntryFactory(BGDataEntryFactory& factory);
    virtual ~BGDataEntryFactory() = default;
    virtual BGDataEntry* createDataEntry(const QStringList& rawData);
};

#endif // BGDATAENTRYFACTORY_H
