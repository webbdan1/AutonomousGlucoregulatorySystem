/******************************************************************************
** FILE: InsulinDataEntry.h
**
** ABSTRACT:
** Inherits from DataEntry. Represents the current
** estimated plasma insulin concentration at the time
** specified (in seconds since epoch).
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

#ifndef INSULINDATAENTRY_H
#define INSULINDATAENTRY_H

#include "DataEntry.h"

class InsulinDataEntry : public DataEntry
{
protected:
    double m_insulinOnBoard;

public:
    InsulinDataEntry() = default;
    virtual ~InsulinDataEntry() = default;
    InsulinDataEntry(InsulinDataEntry& bg) = default;

    double insulinOnBoard() const;
    void setInsulinOnBoard(double insulinOnBoard);
    void print();
};

#endif // INSULINDATAENTRY_H
