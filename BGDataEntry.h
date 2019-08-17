/******************************************************************************
** FILE: BGDataEntry.h
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

#ifndef BGDATAENTRY_H
#define BGDATAENTRY_H

#include <QDateTime>
#include "DataEntry.h"

class BGDataEntry : public DataEntry
{
protected:
    int m_value;
    int m_trend;

public:
    BGDataEntry() = default;
    virtual ~BGDataEntry() = default;
    BGDataEntry(BGDataEntry& bg) = default;

    int getValue() const;
    void setValue(int value);
    int getTrend() const;
    void setTrend(int trend);
    void print();
};

#endif // BGDATAENTRY_H
