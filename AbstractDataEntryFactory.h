/******************************************************************************
** FILE: AbstractDataEntryFactory.h
**
** ABSTRACT:
** The abstract interface for creating DataEntry objects.
**
** DOCUMENTS:
** A reference to the applicable design documents.
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

#ifndef ABSTRACTDATAENTRYFACTORY_H
#define ABSTRACTDATAENTRYFACTORY_H

#include <QStringList>
#include "DataEntry.h"

class AbstractDataEntryFactory
{
public:
    virtual DataEntry* createDataEntry(const QStringList& rawData) = 0;
};

#endif // ABSTRACTDATAENTRYFACTORY_H
