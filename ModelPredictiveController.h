/******************************************************************************
** FILE: ModelPredictiveController.h
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
** 08/01/2019
**
** NOTES:
**
******************************************************************************/

#ifndef MODELPREDICTIVECONTROLLER_H
#define MODELPREDICTIVECONTROLLER_H

#include <vector>
using std::vector;
#include "BGDataEntry.h"
#include "InsulinDataEntry.h"
#include "Model.h"

class ModelPredictiveController
{
protected:
    vector<int> m_bgInputs;
    vector<float> m_insulinInputs;
    vector<double> m_bgPredictions;
    vector<double> m_controlOutput;
    int m_sensitivity;
    double m_peakInsulinTime;
    double m_activityDurationMinutes;
    int m_target;
    double m_maxBolus;
    Model* m_model;


public:
    ModelPredictiveController() = default;
    ~ModelPredictiveController() = default;

    void setModel(Model* model);
    double optimizeControl(vector<vector<double>> bg,
                           vector<double> correction);
    void addBGInput(int bg);
    vector<float> getNInsulinValues(int n, float bolus);
    void addInsulinInput(float iob);
    void runPredictionModel();
    void calculateControlInput();
    void calculateControlOutput();
    vector<double> getControlOutput() const;
    vector<double> getPredictions();
    int bgListSize();
    int getSensitivity() const;
    void setSensitivity(int sensitivity);
    double getPeakInsulinTime() const;
    void setPeakInsulinTime(double peakInsulinTime);
    double getActivityDurationMinutes() const;
    void setActivityDurationMinutes(double activityDurationMinutes);
    int getTarget() const;
    void setTarget(int target);
    double getMaxBolus() const;
    void setMaxBolus(double maxBolus);
};

#endif // MODELPREDICTIVECONTROLLER_H
