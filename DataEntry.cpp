/******************************************************************************
** FILE: DataEntry.cpp
**
** ABSTRACT:
** Base class for application data (bg values, insulin
** values)
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
#include "DataEntry.h"

/*-----------------------------------------------------------------------------
Name:     getScrapeTime
Purpose:  Return the time at which AGS queried dexcom for the data in seconds
          since epoch.
Receive:  N/A
Return:   double
-----------------------------------------------------------------------------*/
double DataEntry::getScrapeTime() const
{
    return m_scrapeTime;
}

/*-----------------------------------------------------------------------------
Name:     setScrapeTime
Purpose:  Set the time at which AGS queried dexcom for the data in seconds
          since epoch.
Receive:  const double &scrapeTime
Return:   N/A
-----------------------------------------------------------------------------*/
void DataEntry::setScrapeTime(const double &scrapeTime)
{
    m_scrapeTime = scrapeTime;
}

/*-----------------------------------------------------------------------------
Name:     getSampleTime
Purpose:  Return the time at which the blood glucose reading was physically
          sampled by the Dexcom CGM in seconds since epoch.
Receive:  N/A
Return:   double
-----------------------------------------------------------------------------*/
double DataEntry::getSampleTime() const
{
    return m_sampleTime;
}

/*-----------------------------------------------------------------------------
Name:     setSampleTime
Purpose:  Set the time at which AGS queried dexcom for the data in seconds
          since epoch.
Receive:  const double &sampleTime
Return:   N/A
-----------------------------------------------------------------------------*/
void DataEntry::setSampleTime(const double &sampleTime)
{
    m_sampleTime = sampleTime;
}

/*-----------------------------------------------------------------------------
Name:     getScrapeTime
Purpose:  Return the time in seconds between when the Blood glucose reading
          was physically sampled by Dexcom and queried by AGS.
Receive:  N/A
Return:   double
-----------------------------------------------------------------------------*/
double DataEntry::getDelayTime() const
{
    return m_delayTime;
}

/*-----------------------------------------------------------------------------
Name:     setDelayTime
Purpose:  Set the time in seconds between when the Blood glucose reading
          was physically sampled by Dexcom and queried by AGS.
Receive:  const double &delayTime
Return:   N/A
-----------------------------------------------------------------------------*/
void DataEntry::setDelayTime(const double &delayTime)
{
    m_delayTime = delayTime;
}


