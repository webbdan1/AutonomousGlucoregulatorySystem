/******************************************************************************
** FILE: Main.cpp
**
** ABSTRACT:
** Drives the main functions of the application in a
** thread that executes once every 4 minutes
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
#include <QApplication>
#include <iostream>
#include <thread>
#include <chrono>
#include "ModelPredictiveController.h"
#include "RandomForestModel.h"
#include "StateSpaceModel.h"

/*-----------------------------------------------------------------------------
Name:     printCurrentTime
Purpose:  Sends current system time to output console for debugging purposes.
Receive:  std::chrono::system_clock::time_point timePoint
Return:   N/A
-----------------------------------------------------------------------------*/
void printCurrentTime(std::chrono::system_clock::time_point timePoint)
{
    std::time_t timeStamp = std::chrono::system_clock::to_time_t(timePoint);
    std::cout << std::ctime(&timeStamp) << std::endl;

}

/*-----------------------------------------------------------------------------
Name:     threadFunc
Purpose:  Commands the application thread to sleep until the specified
          time point. 240 seconds since Dexcom creates a new reading every
          5 minutes. This ensures we usually don't query new data when there is
          none.
Receive:  N/A
Return:   N/A
-----------------------------------------------------------------------------*/
void threadFunc()
{

    std::cout<<"Current Time :: ";
    // Print Current Time
    printCurrentTime(std::chrono::system_clock::now());

    // create a time point pointing to 60 seconds in future
    std::chrono::system_clock::time_point timePoint =
            std::chrono::system_clock::now() + std::chrono::seconds(240);

    std::cout << "Going to Sleep Until :: "; printCurrentTime(timePoint);


    // Sleep Till specified time point
    // Accepts std::chrono::system_clock::time_point as argument
    std::this_thread::sleep_until(timePoint);

    std::cout<<"Current Time :: ";
    // Print Current Time
    printCurrentTime(std::chrono::system_clock::now());
}

/*-----------------------------------------------------------------------------
Name:     main
Purpose:  Drives the AGS application on a timed thread. Maintains a DataQueue
          which holds up to 24 hours of data, sends it to various MPC's using
          various different Model objects and gets the optimization output.
Receive:  command line arguments
Return:   int
-----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //instantiate data queue and mpc
    DataQueue* dataQueue = new DataQueue();
    vector<double> temp;
    //main loop
    while(true){     
      //query new data from dexcom
      bool successFlag = dataQueue->scrapeData();
      if(successFlag){
          //STATE SPACE MODEL
           ModelPredictiveController* modelPredictiveControllerSS =
                   new ModelPredictiveController();
           StateSpaceModel* stateSpaceModel = new StateSpaceModel();
           modelPredictiveControllerSS->setModel(stateSpaceModel);

           //RF MODEL
           ModelPredictiveController* modelPredictiveControllerRF =
                   new ModelPredictiveController();
           RandomForestModel* randomForestModel = new RandomForestModel();
           modelPredictiveControllerRF->setModel(randomForestModel);

          //configure SS MPC
          modelPredictiveControllerSS->setSensitivity(30);
          modelPredictiveControllerSS->setPeakInsulinTime(57.0);
          modelPredictiveControllerSS->setActivityDurationMinutes(90.0);
          modelPredictiveControllerSS->setTarget(110);
          modelPredictiveControllerSS->setMaxBolus(16.0);

          //configure RF MPC
          modelPredictiveControllerRF->setSensitivity(30);
          modelPredictiveControllerRF->setPeakInsulinTime(57.0);
          modelPredictiveControllerRF->setActivityDurationMinutes(90.0);
          modelPredictiveControllerRF->setTarget(110);
          modelPredictiveControllerRF->setMaxBolus(16.0);


          //get future insulin values
          vector<float> futureInsulin = dataQueue->getFutureInsulinValues();
          for(int k=0;k<futureInsulin.size();k++){
              modelPredictiveControllerSS->addInsulinInput(futureInsulin[k]);
              modelPredictiveControllerRF->addInsulinInput(futureInsulin[k]);
          }

          //get last 6 BG readings
          vector<int> bgEntries = dataQueue->getNBGEntries(6);
          for(int i =5; i>-1;i--){
              modelPredictiveControllerSS->addBGInput(bgEntries[i]);
              modelPredictiveControllerRF->addBGInput(bgEntries[i]);
          }

          vector<double> predictionsSS;
          vector<double> predictionsRF;

          //get base SS prediction
          modelPredictiveControllerSS->runPredictionModel();
          predictionsSS = modelPredictiveControllerSS->getPredictions();

          //get base RF prediction
          modelPredictiveControllerRF->runPredictionModel();
          predictionsRF = modelPredictiveControllerRF->getPredictions();

          //MPC SS Optimization
          modelPredictiveControllerSS->calculateControlInput();


          //MPC RF Optimization
          modelPredictiveControllerRF->calculateControlInput();


          //delete objects
          delete modelPredictiveControllerRF;
          delete randomForestModel;
          delete modelPredictiveControllerSS;
          delete stateSpaceModel;
      }

      //wait 4 minutes until next bg reading is ready to rerun
      std::thread th(&threadFunc);
      th.join();
    }
    return 0;
}
