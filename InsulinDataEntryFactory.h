/******************************************************************************
** FILE: InsulinDataEntryFactory.h
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

#ifndef INSULINDATAENTRYFACTORY_H
#define INSULINDATAENTRYFACTORY_H

#include "AbstractDataEntryFactory.h"
#include "InsulinDataEntry.h"

class InsulinDataEntryFactory : public AbstractDataEntryFactory
{
public:
    virtual ~InsulinDataEntryFactory() = default;
    virtual InsulinDataEntry* createDataEntry(const QStringList& rawData);
};

#endif // INSULINDATAENTRYFACTORY_H
