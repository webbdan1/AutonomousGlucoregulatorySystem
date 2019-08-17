/******************************************************************************
** FILE: Model.h
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

#ifndef MODEL_H
#define MODEL_H

#include <vector>
using std::vector;
#include "BGDataEntry.h"

class Model
{
public:
    Model() = default;
    virtual ~Model() = default;

    virtual vector<double> predict(vector<int> bgInputs,
                                   vector<float> insulinInputs, bool saveFlag);
    virtual vector<double> projectCorrection(vector<double> bgInputs,
                                             vector<float> insulinInputs,
                                             int sensitivity);
};

#endif // MODEL_H
