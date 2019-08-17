/******************************************************************************
** FILE: InsulinDataEntry.cpp
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

#include "InsulinDataEntry.h"
#include <iostream>

/*-----------------------------------------------------------------------------
Name:     insulinOnBoard
Purpose:  Return insulin on board, AKA estimated plasma insulin concentration
          in units.
Receive:  N/A
Return:   double
-----------------------------------------------------------------------------*/
double InsulinDataEntry::insulinOnBoard() const
{
    return m_insulinOnBoard;
}

/*-----------------------------------------------------------------------------
Name:     setInsulinOnBoard
Purpose:  Set insulin on board, AKA estimated plasma insulin concentration
          in units.
Receive:  double insulinOnBoard
Return:   N/A
-----------------------------------------------------------------------------*/
void InsulinDataEntry::setInsulinOnBoard(double insulinOnBoard)
{
    m_insulinOnBoard = insulinOnBoard;
}

/*-----------------------------------------------------------------------------
Name:     print
Purpose:  Print out insulin data entry attributes to the console.
Receive:  N/A
Return:   N/A
-----------------------------------------------------------------------------*/
void InsulinDataEntry::print()
{
    std::cout << "IOB: " << m_insulinOnBoard << " Sample Time: " <<
    m_sampleTime << std::endl;
}
