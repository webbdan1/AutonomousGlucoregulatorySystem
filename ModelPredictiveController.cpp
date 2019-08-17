/******************************************************************************
** FILE: ModelPredictiveController.cpp
**
** ABSTRACT:
** Takes current data input from the DataQueue, hooks
** in the desired model, and runs the model through
** an optimization routine to determine the best insulin
** treatment to match the predicted values with
** the target blood glucose value.
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

#include "ModelPredictiveController.h"
#include <QFile>
#include <QTextStream>
#include <string>
using std::string;
#include <QDir>
#include <iostream>
#include <math.h>

/*-----------------------------------------------------------------------------
Name:     getSensitivity
Purpose:  Returns sensitivity which is a constant integer representing
          the impact of 1 unit of insulin on blood glucose after fully
          metabolized.
Receive:  N/A
Return:   int
-----------------------------------------------------------------------------*/
int ModelPredictiveController::getSensitivity() const
{
    return m_sensitivity;
}

/*-----------------------------------------------------------------------------
Name:     setSensitivity
Purpose:  Sets the sensitivity which is a constant integer representing
          the impact of 1 unit of insulin on blood glucose after fully
          metabolized.
Receive:  int
Return:   N/A
-----------------------------------------------------------------------------*/
void ModelPredictiveController::setSensitivity(int sensitivity)
{
    m_sensitivity = sensitivity;
}

/*-----------------------------------------------------------------------------
Name:     getPeakInsulinTime
Purpose:  Returns the time at which insulin plasma concentration peaks,
          in minutes.
Receive:  N/A
Return:   double
-----------------------------------------------------------------------------*/
double ModelPredictiveController::getPeakInsulinTime() const
{
    return m_peakInsulinTime;
}

/*-----------------------------------------------------------------------------
Name:     setPeakInsulinTime
Purpose:  Sets the time at which insulin plasma concentration peaks,
          in minutes.
Receive:  double
Return:   N/A
-----------------------------------------------------------------------------*/
void ModelPredictiveController::setPeakInsulinTime(double peakInsulinTime)
{
    m_peakInsulinTime = peakInsulinTime;
}

/*-----------------------------------------------------------------------------
Name:     getActivityDurationMinutes
Purpose:  Returns the time in minutes after which insulin is no longer present
          in the subject either subcutaneously or in plasma.
Receive:  N/A
Return:   double
-----------------------------------------------------------------------------*/
double ModelPredictiveController::getActivityDurationMinutes() const
{
    return m_activityDurationMinutes;
}

/*-----------------------------------------------------------------------------
Name:     setActivityDurationMinutes
Purpose:  Sets the time in minutes after which insulin is no longer present
          in the subject either subcutaneously or in plasma.
Receive:  N/A
Return:   double
-----------------------------------------------------------------------------*/
void ModelPredictiveController::setActivityDurationMinutes(
                                                double activityDurationMinutes)
{
    m_activityDurationMinutes = activityDurationMinutes;
}

/*-----------------------------------------------------------------------------
Name:     getTarget
Purpose:  Returns the blood glucose that the MPC wishes to target in mg/dl
Receive:  N/A
Return:   int
-----------------------------------------------------------------------------*/
int ModelPredictiveController::getTarget() const
{
    return m_target;
}

/*-----------------------------------------------------------------------------
Name:     setTarget
Purpose:  Sets the blood glucose that the MPC wishes to target in mg/dl
Receive:  N/A
Return:   int
-----------------------------------------------------------------------------*/
void ModelPredictiveController::setTarget(int target)
{
    m_target = target;
}

/*-----------------------------------------------------------------------------
Name:     addBGInput
Purpose:  Adds a previously recorded bg value to the MPC for prediction
          purposes.
Receive:  int
Return:   N/A
-----------------------------------------------------------------------------*/
void ModelPredictiveController::addBGInput(int bg)
{
    m_bgInputs.push_back(bg);
}

/*-----------------------------------------------------------------------------
Name:     addInsulinInput
Purpose:  Adds a previously recorded insulin value to the MPC for prediction
          purposes.
Receive:  float
Return:   N/A
-----------------------------------------------------------------------------*/
void ModelPredictiveController::addInsulinInput(float iob)
{
    m_insulinInputs.push_back(iob);
}

/*-----------------------------------------------------------------------------
Name:     getPredictions
Purpose:  Returns BG values predicted by the model for the future.
          They are predicted in 5 minute intervals for the 90 minute horizon,
          so 18 values in mg/dl are returned.
Receive:  N/A
Return:   vector<double>
-----------------------------------------------------------------------------*/
vector<double> ModelPredictiveController::getPredictions()
{
    return m_bgPredictions;
}

/*-----------------------------------------------------------------------------
Name:     runPredictionModel
Purpose:  Every 5 minutes AGS generates a new set of predictions for BG for
          the 90 minute time horizon. This function sends the provided
          inputs to the prediction model and runs it.
Receive:  N/A
Return:   N/A
-----------------------------------------------------------------------------*/
void ModelPredictiveController::runPredictionModel()
{
    vector<double> predictions = m_model->predict(m_bgInputs, m_insulinInputs, true);
    for(int i=0; i<predictions.size();i++){
        m_bgPredictions.push_back(predictions[i]);
    }
}

/*-----------------------------------------------------------------------------
Name:     getNInsulinValues
Purpose:  This is the model for how insulin appears in plasma with respect
          to time. This function takes the bolus and returns n future insulin
          values to the caller. This is based off literature from novo-nordisk
          on the pharmacokinetic properties of Novolog and Fiasp. It also is
          sure to add in the insulin that will be in plasma due to
          preexisting boluses by adding the m_insulinInput values provided to
          the MPC.
Receive:  n the number of future plasma insulin concentration values requested
          float bolus the insulin bolus administered at t=0
Return:   vector<float>
-----------------------------------------------------------------------------*/
vector<float> ModelPredictiveController::getNInsulinValues(int n, float bolus){
    float end = n*5;
    int minsAgo = 5;
    vector<float> results;

    while(minsAgo<=end){
        float tau = m_peakInsulinTime*(1-m_peakInsulinTime/end)/
                (1-2*m_peakInsulinTime/end);
        float a = 2*tau/end;
        float s = 1/(1-a+(1+a)*exp(-end/tau));
        float activityContrib = float(bolus) * float((s / (pow(tau,2)))
                           * minsAgo * (1 - minsAgo / end) * exp(-minsAgo/tau));
        float iobContrib = bolus * (1 - s * (1 - a) * ((pow(minsAgo, 2)
                           / (tau * end * (1 - a)) - minsAgo / tau - 1) *
                           exp(-minsAgo / tau) + 1));
        //adds the current insulin already on board
        results.push_back(iobContrib+m_insulinInputs[minsAgo/5]);
        minsAgo +=5;
    }
    return results;
}

/*-----------------------------------------------------------------------------
Name:     calculateControlInput
Purpose:  Runs the models using all possible insulin control inputs to be
          administered at the next time step starting with the maxBolus and
          decrementing by 0.5 units to 0. It stores all the model projections
          in containers and sends them to the optimizer to find the one with
          the least error between the projection and the target BG value.
Receive:  N/A
Return:   N/A
-----------------------------------------------------------------------------*/
void ModelPredictiveController::calculateControlInput()
{
    double correction = m_maxBolus;
    vector<vector<double>> results;
    vector<double> correctionResults;
    while(correction>=0){
      vector<float> futureInsulin = getNInsulinValues(18,correction);
      //predict
      vector<double> nextBGSet =
      m_model->projectCorrection(m_bgPredictions,futureInsulin, m_sensitivity);

      results.push_back(nextBGSet);

      //record results      
      correctionResults.push_back(correction);

      //decrement bolus
      correction -= 0.5;
      //clear vector
    }
    m_controlInput = optimizeControl(results,correctionResults);
}

/*-----------------------------------------------------------------------------
Name:     getMaxBolus
Purpose:  Returns the maximum bolus the MPC is allowed to issue set by the user,
          in units.
Receive:  N/A
Return:   double
-----------------------------------------------------------------------------*/
double ModelPredictiveController::getMaxBolus() const
{
    return m_maxBolus;
}

/*-----------------------------------------------------------------------------
Name:     getNInsulinValues
Purpose:  This is the model for how insulin appears in plasma with respect
          to time. This function takes the bolus and returns n future insulin
          values to the caller. This is based off literature from novo-nordisk
          on the pharmacokinetic properties of Novolog and Fiasp. It also is
          sure to add in the insulin that will be in plasma due to
          preexisting boluses by adding the m_insulinInput values provided to
          the MPC.
Receive:  n the number of future plasma insulin concentration values requested
          float bolus the insulin bolus administered at t=0
Return:   vector<float>
-----------------------------------------------------------------------------*/
void ModelPredictiveController::setMaxBolus(double maxBolus)
{
    m_maxBolus = maxBolus;
}

/*-----------------------------------------------------------------------------
Name:     setModel
Purpose:  Gives the MPC a model for the relationship between BG and plasma
          insulin concentration.
Receive:  Model*
Return:   N/A
-----------------------------------------------------------------------------*/
void ModelPredictiveController::setModel(Model *model)
{
    m_model = model;
}

/*-----------------------------------------------------------------------------
Name:     optimizeControl
Purpose:  Looks at all the possible future BG curves based on control input
          and picks the one with the least error between the projected values
          and the target BG value.
Receive:  bg is a container holding the model output curves for all possible
          control inputs
          correction is the vector containing all the insulin control inputs
          aligned with each curve in bg by index
Return:   double the insulin treatment recomended by the MPC
-----------------------------------------------------------------------------*/
double ModelPredictiveController::optimizeControl(vector<vector<double>> bg,
                                                  vector<double> correction)
{
    int distance = 0;
    double treatment = 0.0;
    int trajectoryIndex=0;
    for(int i=0; i<correction.size(); i++){
        std::cout<<correction[i]<<": ";
        for(int y=0;y<bg[i].size();y++){
            std::cout<<bg[i][y]<<", ";
        }
        std::cout<<'\n';
        vector<int> errors;
        for(int j=0;j<bg[i].size();j++){
            int error = abs(bg[i][j]-m_target);
            errors.push_back(error);
        }
        //average out the error
        double total=0;
        for(int m=0;m<errors.size();m++){
            total+=errors[m];
        }
        double avgError = total/errors.size();
        if(!distance){
            distance=avgError;
        }
        else if(avgError<distance){
            distance = avgError;
            treatment = correction[i];
            trajectoryIndex = i;
            m_controlOutput = bg[i];
        }
    }
std::cout<< "Chosen Bolus: " << treatment<<std::endl;
    return treatment;
}

/*-----------------------------------------------------------------------------
Name:     getControlOutput
Purpose:  Returns the recomended control for the user to take at the next
          time step such that BG will converge to the target according to
          the model projections.
Receive:  N/A
Return:   vector<double>
-----------------------------------------------------------------------------*/
vector<double> ModelPredictiveController::getControlOutput() const
{
    return m_controlOutput;
}

/*-----------------------------------------------------------------------------
Name:     getNInsulinValues
Purpose:  Returns the number of BG value inputs provided to the model.
Receive:  N/A
Return:   int
-----------------------------------------------------------------------------*/
int ModelPredictiveController::bgListSize()
{
    return m_bgInputs.size();
}
