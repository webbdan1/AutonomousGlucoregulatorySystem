/******************************************************************************
** FILE: StateSpaceModel.cpp
**
** ABSTRACT:
** Non-linear state space model for the relationship
** between plasma insulin concentration and blood
** glucose. Provides prediction results to the MPC.
**
** DOCUMENTS:
**
**
** AUTHOR:
** Daniel Webb
**
** CREATION DATE:
** 08/10/2019
**
** NOTES:
**
******************************************************************************/

#include "StateSpaceModel.h"
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
Return:   vector<double> predictions for future (1 in this case)
-----------------------------------------------------------------------------*/
vector<double> StateSpaceModel::predict(vector<int> bgInputs, vector<float> insulinInputs, bool saveFlag){
    vector<double> bgPredictions;
     bgPredictions.push_back(bgInputs[0]);
     return bgPredictions;
}

/*-----------------------------------------------------------------------------
Name:     projectCorrection
Purpose:  Runs the hypothetical insulin control input supplied to the model
          and gives the projected BG output based on this input. In this case
          we take the change in plasma insulin concentration at each time
          step, multiply by the sensitivity, and subtract it from the
          current time step to get projected BG. This is how the state space
          model works, it is recursive. This is not a recursive function but
          will be in the future.
Receive:  bgInputs and insulin inputs for the model, sensitivity is constant
          representing the impact of 1 unit of insulin on blood glucose.
Return:   vector<double> predictions for future BG (90 mins / 5 = 18 values)
-----------------------------------------------------------------------------*/
vector<double> StateSpaceModel::projectCorrection(vector<double> bgInputs, vector<float> insulinInputs, int sensitivity)
{
    //predict
    vector<double> results;
    for(int i = 0;i<insulinInputs.size();i++){
        //change in plasma insulin concentration
        double delta = insulinInputs[i]-insulinInputs[i+1];
        //multiply by sensititivity
        double multiplicand = sensitivity*-1.0*delta;
        //current bg t=0
        double bg;
        if(results.size()){
            bg= results[results.size()-1];
        }
        else{
            bg=bgInputs[i];
        }
        //subtract impact of insulin from bg at t=0 to get bg at t=1
        double nextBG = bg + multiplicand;
        results.push_back(nextBG);
    }
    return results;
}
