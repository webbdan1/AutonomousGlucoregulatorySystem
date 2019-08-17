/******************************************************************************
** FILE: DataEntry.h
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

#ifndef DATAENTRY_H
#define DATAENTRY_H

#include <QDateTime>

class DataEntry
{
protected:
    double m_scrapeTime;
    double m_sampleTime;
    double m_delayTime;

public:
    DataEntry() = default;
    ~DataEntry() = default;
    DataEntry(DataEntry& entry) = default;

    double getScrapeTime() const;
    void setScrapeTime(const double &scrapeTime);
    double getSampleTime() const;
    void setSampleTime(const double &sampleTime);
    double getDelayTime() const;
    void setDelayTime(const double &delayTime);
};

#endif // DATAENTRY_H
