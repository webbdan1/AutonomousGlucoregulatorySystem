/******************************************************************************
** FILE: RandomForestModel.cpp
**
** ABSTRACT:
** Provides input to the RandomForest script which runs
** the trained random forest model created using
** statsmodels in Python 3.6. Provides output to the
** MPC for optimization.
**
** DOCUMENTS:
**
**
** AUTHOR:
** Daniel Webb
**
** CREATION DATE:
** 08/15/2019
**
** NOTES:
**
******************************************************************************/

#include "RandomForestModel.h"
#include <QFile>
#include <QTextStream>
#include <string>
#include <QDir>
#include <QTextStream>
#include <iostream>
#include "BGDataEntry.h"
using std::string;

/*-----------------------------------------------------------------------------
Name:     predict
Purpose:  Runs the prediction model once using input bg and insulin
          data at t=0.
Receive:  bgInputs and insulin inputs for the model, sensitivity is constant
          representing the impact of 1 unit of insulin on blood glucose.
Return:   vector<double> predictions for future BG (90 mins / 5 = 18 values)
-----------------------------------------------------------------------------*/
vector<double> RandomForestModel::predict(vector<int> bgInputs,
                                          vector<float> insulinInputs,
                                          bool saveFlag)
{
    vector<double> bgPredictions;

    //first write out new BG /insulin values
   QFile data(QDir::currentPath()+"/RandomForest/test.txt");
   if (data.open(QIODevice::WriteOnly | QIODevice::Text)) {
       QTextStream out(&data);
       if(bgInputs.size()>=1){
           out
           << bgInputs[0] << "," << bgInputs[1] << ","
           << bgInputs[2] << "," << bgInputs[3] << ","
           << bgInputs[4] << "," << bgInputs[5] << ","
           << insulinInputs[0] << "," << insulinInputs[1] << ","
           << insulinInputs[2] << "," << insulinInputs[3] << ","
           << insulinInputs[4] << "," << insulinInputs[5] << ","
           << insulinInputs[6] << "," << insulinInputs[7] << ","
           << insulinInputs[8] << "," << insulinInputs[9] << ","
           << insulinInputs[10] << "," << insulinInputs[11] << ","
           << insulinInputs[12] << "," << insulinInputs[13] << ","
           << insulinInputs[14] << "," << insulinInputs[15] << ","
           << insulinInputs[16] << "," << insulinInputs[17] << ","
           << insulinInputs[18];
       }
   }
   //next, run the models to get predictions
   if(bgInputs.size()>=1){
     string bgData;
     //random forest python script is called
     string RFRP = "/RandomForest/RandomForest.py";
     //string RFRP = "/RandomForestTest/RandomForestTest";
     string RFFQP = "python3 " + QDir::currentPath().toStdString()+RFRP;

     std::cout << "Opening RF Prediction reading pipe" << std::endl;
     FILE* pipe = popen(RFFQP.c_str(), "r");
     if (!pipe)
     {
       std::cerr << "Couldn't start command." << std::endl;

     }
     char line[1024];
     //script output is collected into list
     while (fgets(line, 1024, pipe))
          bgData += line;
     auto returnCode = pclose(pipe);
     //get predictions and push back to prediction container
     QString stringData = QString::fromUtf8(bgData.c_str());
     QStringList formattedData = stringData.split('\n');
     //Push back all predictions
     bgPredictions.push_back(formattedData[0].toDouble());
     bgPredictions.push_back(formattedData[1].toDouble());
     bgPredictions.push_back(formattedData[2].toDouble());
     bgPredictions.push_back(formattedData[3].toDouble());
     bgPredictions.push_back(formattedData[4].toDouble());
     bgPredictions.push_back(formattedData[5].toDouble());
     bgPredictions.push_back(formattedData[6].toDouble());
     bgPredictions.push_back(formattedData[7].toDouble());
     bgPredictions.push_back(formattedData[8].toDouble());
     bgPredictions.push_back(formattedData[9].toDouble());
     bgPredictions.push_back(formattedData[10].toDouble());
     bgPredictions.push_back(formattedData[11].toDouble());
     bgPredictions.push_back(formattedData[12].toDouble());
     bgPredictions.push_back(formattedData[13].toDouble());
     bgPredictions.push_back(formattedData[14].toDouble());
     bgPredictions.push_back(formattedData[15].toDouble());
     bgPredictions.push_back(formattedData[16].toDouble());
     bgPredictions.push_back(formattedData[17].toDouble());
     //if we want to save this prediction result to database
     if(saveFlag){
         string RFRP = "/RF/RF";
         string RFFQP = QDir::currentPath().toStdString()+RFRP;

         std::cout << "Opening RF Database pipe" << std::endl;
         FILE* pipe = popen(RFFQP.c_str(), "r");
         if (!pipe)
         {
           std::cerr << "Couldn't start command." << std::endl;

         }
         auto returnCode = pclose(pipe);
     }
     return bgPredictions;
   }
}

/*-----------------------------------------------------------------------------
Name:     projectCorrection
Purpose:  Runs the hypothetical insulin control input supplied to the model
          and gives the projected BG output based on this input.
Receive:  bgInputs and insulin inputs for the model, sensitivity is constant
          representing the impact of 1 unit of insulin on blood glucose.
Return:   vector<double> predictions for future BG (90 mins / 5 = 18 values)
-----------------------------------------------------------------------------*/
vector<double> RandomForestModel::projectCorrection(vector<double> bgInputs,
                                                    vector<float> insulinInputs,
                                                    int sensitivity)
{
    vector<double> bgPredictions;

    //first write out new BG/insulin values
   QFile data(QDir::currentPath()+"/RandomForest/test.txt");
   if (data.open(QIODevice::WriteOnly | QIODevice::Text)) {
       QTextStream out(&data);
       if(bgInputs.size()>=1){
           out
           << bgInputs[0] << "," << bgInputs[1] << ","
           << bgInputs[2] << "," << bgInputs[3] << ","
           << bgInputs[4] << "," << bgInputs[5] << ","
           << insulinInputs[0] << "," << insulinInputs[1] << ","
           << insulinInputs[2] << "," << insulinInputs[3] << ","
           << insulinInputs[4] << "," << insulinInputs[5] << ","
           << insulinInputs[6] << "," << insulinInputs[7] << ","
           << insulinInputs[8] << "," << insulinInputs[9] << ","
           << insulinInputs[10] << "," << insulinInputs[11] << ","
           << insulinInputs[12] << "," << insulinInputs[13] << ","
           << insulinInputs[14] << "," << insulinInputs[15] << ","
           << insulinInputs[16] << "," << insulinInputs[17] << ","
           << insulinInputs[18];
       }
   }
   //next, run the models to get predictions
   if(bgInputs.size()>=1){
     string bgData;

     string RFRP = "/RandomForest/RandomForest.py";
     //string RFRP = "/RandomForestTest/RandomForestTest";
     string RFFQP = "python3 " + QDir::currentPath().toStdString()+RFRP;

     std::cout << "Opening RF Projection reading pipe" << std::endl;
     FILE* pipe = popen(RFFQP.c_str(), "r");
     if (!pipe)
     {
       std::cerr << "Couldn't start command." << std::endl;

     }
     char line[1024];
     //add raw output to list
     while (fgets(line, 1024, pipe))
          bgData += line;
     auto returnCode = pclose(pipe);
     //get predictions and push back to prediction container
     QString stringData = QString::fromUtf8(bgData.c_str());
     QStringList formattedData = stringData.split('\n');
     //Push back all predictions
     bgPredictions.push_back(formattedData[0].toDouble());
     bgPredictions.push_back(formattedData[1].toDouble());
     bgPredictions.push_back(formattedData[2].toDouble());
     bgPredictions.push_back(formattedData[3].toDouble());
     bgPredictions.push_back(formattedData[4].toDouble());
     bgPredictions.push_back(formattedData[5].toDouble());
     bgPredictions.push_back(formattedData[6].toDouble());
     bgPredictions.push_back(formattedData[7].toDouble());
     bgPredictions.push_back(formattedData[8].toDouble());
     bgPredictions.push_back(formattedData[9].toDouble());
     bgPredictions.push_back(formattedData[10].toDouble());
     bgPredictions.push_back(formattedData[11].toDouble());
     bgPredictions.push_back(formattedData[12].toDouble());
     bgPredictions.push_back(formattedData[13].toDouble());
     bgPredictions.push_back(formattedData[14].toDouble());
     bgPredictions.push_back(formattedData[15].toDouble());
     bgPredictions.push_back(formattedData[16].toDouble());
     bgPredictions.push_back(formattedData[17].toDouble());
     return bgPredictions;
   }
}
