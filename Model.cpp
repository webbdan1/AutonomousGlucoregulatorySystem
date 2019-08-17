/******************************************************************************
** FILE: Model.cpp
**
** ABSTRACT:
** The base class for models used to model the
** relationship between insulin and blood sugar.
** Each model has its own unique way of predicting and
** optimization.
**
** DOCUMENTS:
**
**
** AUTHOR:
** Daniel Webb
**
** CREATION DATE:
** 07/15/2019
**
** NOTES:
**
******************************************************************************/

#include "Model.h"
#include <iostream>

/*-----------------------------------------------------------------------------
Name:     predict
Purpose:  Runs the prediction model once using input bg and insulin
          data at t=0.
Receive:  bgInputs and insulin inputs for the model, sensitivity is constant
          representing the impact of 1 unit of insulin on blood glucose.
Return:   vector<double> predictions for future BG (90 mins / 5 = 18 values)
-----------------------------------------------------------------------------*/
vector<double> Model::predict(vector<int> bgInputs,
                              vector<float> insulinInputs,
                              bool saveFlag)
{
    std::cout<<"predicting base"<<std::endl;
}

/*-----------------------------------------------------------------------------
Name:     projectCorrection
Purpose:  Runs the hypothetical insulin control input supplied to the model
          and gives the projected BG output based on this input.
Receive:  bgInputs and insulin inputs for the model, sensitivity is constant
          representing the impact of 1 unit of insulin on blood glucose.
Return:   vector<double> predictions for future BG (90 mins / 5 = 18 values)
-----------------------------------------------------------------------------*/
vector<double> Model::projectCorrection(vector<double> bgInputs,
                                        vector<float> insulinInputs,
                                        int sensitivity)
{
     std::cout<<"predicting base"<<std::endl;
}
