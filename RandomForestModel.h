/******************************************************************************
** FILE: RandomForestModel.h
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

#ifndef RANDOMFORESTMODEL_H
#define RANDOMFORESTMODEL_H

#include "Model.h"
#include <vector>
using std::vector;

class RandomForestModel : public Model
{
public:
    RandomForestModel() = default;
    virtual ~RandomForestModel() = default;

    vector<double> predict(vector<int> bgInputs, vector<float> insulinInputs,
                           bool saveFlag);
    vector<double> projectCorrection(vector<double> bgInputs,
                                     vector<float> insulinInputs,
                                     int sensitivity);
};


#endif // RANDOMFORESTMODEL_H
