/******************************************************************************
** FILE: MainWindow.cpp
**
** ABSTRACT:
** The MainWindow for the GUI.
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

#include "MainWindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    m_dataQueue = new DataQueue();
    ui->setupUi(this);

    connect(ui->queryBG, SIGNAL(clicked()), ui->queryBG,
            SLOT(beenClicked()));
    connect(ui->queryBG, SIGNAL(iChanged(QObject*)), this,
            SLOT(actOnChange(QObject*)));

}

/*-----------------------------------------------------------------------------
Name:     ~MainWindow
Purpose:  Destructor.
Receive:  N/A
Return:   N/A
-----------------------------------------------------------------------------*/
MainWindow::~MainWindow()
{
    delete ui;
    delete m_dataQueue;
}

/*-----------------------------------------------------------------------------
Name:     actOnChange
Purpose:  Mediator function for the mediator pattern in Gamma et. al.
          Centralized controller for collaboration between UI elements.
Receive:  QObject * obj UI object that generated signal
Return:   N/A
-----------------------------------------------------------------------------*/
void MainWindow::actOnChange(QObject * obj)
{
    if(obj==ui->queryBG){
          m_dataQueue->scrapeData();

          BGDataEntry* currentBG =
                  m_dataQueue->getFirstBGEntry();
          QString bgString =
                  QString::number(currentBG->getValue());
          QString trendString =
                  QString::number(currentBG->getTrend());
          QString sampleTimeString =
                  QString::number(currentBG->getSampleTime());
          QString delayTimeString =
                  QString::number(currentBG->getDelayTime());
          //create new table item and fill fields
          QTableWidgetItem *value = new QTableWidgetItem;
          value->setText(bgString);
          QTableWidgetItem *trend = new QTableWidgetItem;
          trend->setText(trendString);
          QTableWidgetItem *sample = new QTableWidgetItem;
          sample->setText(sampleTimeString);
          QTableWidgetItem *delay = new QTableWidgetItem;
          delay->setText(delayTimeString);
          //add to table
          int insertIndex = (ui->bgTable->rowCount());
          ui->bgTable->insertRow(insertIndex);
          ui->bgTable->setItem(insertIndex,0, value);
          ui->bgTable->setItem(insertIndex,1, trend);
          ui->bgTable->setItem(insertIndex,2, sample);
          ui->bgTable->setItem(insertIndex,3, delay);

    }
}
