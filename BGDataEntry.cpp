/******************************************************************************
** FILE: BGDataEntry.cpp
**
** ABSTRACT:
** Represents one blood glucose reading scraped from
** Dexcom using the DataScraper script. The script
** queries the Dexcom Share Server and gathers data
** on the BG value, in mg/dl, the time represented in
** seconds since epoch, the lag time (time since previous
** reading), and the trend (difference in bg value since
** last reading).Inherits from data entry interface.
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

#include "BGDataEntry.h"
#include <iostream>

/*-----------------------------------------------------------------------------
Name:     getValue
Purpose:  Return Blood Glucose value in mg/dl.
Receive:  N/A
Return:   int BG value
-----------------------------------------------------------------------------*/
int BGDataEntry::getValue() const
{
    return m_value;
}

/*-----------------------------------------------------------------------------
Name:     setValue
Purpose:  Set Blood Glucose value in mg/dl.
Receive:  int BG value
Return:   N/A
-----------------------------------------------------------------------------*/
void BGDataEntry::setValue(int value)
{
    m_value = value;
}

/*-----------------------------------------------------------------------------
Name:     getTrend
Purpose:  Return difference between last two BG readings in mg/dl.
Receive:  N/A
Return:   int BG trend
-----------------------------------------------------------------------------*/
int BGDataEntry::getTrend() const
{
    return m_trend;
}

/*-----------------------------------------------------------------------------
Name:     setTrend
Purpose:  Set difference between last two BG readings in mg/dl.
Receive:  int BG value
Return:   N/A
-----------------------------------------------------------------------------*/
void BGDataEntry::setTrend(int trend)
{
    m_trend = trend;
}

/*-----------------------------------------------------------------------------
Name:     print
Purpose:  Print Blood Glucose value in mg/dl, as well as sample and delay
          time in seconds since epoch.
Receive:  N/A
Return:   N/A
-----------------------------------------------------------------------------*/
void BGDataEntry::print()
{
    std::cout << "Value: " << m_value << " Sample Time: " << m_sampleTime
              << " Delay: " << m_delayTime << std::endl;
}
