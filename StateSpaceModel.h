/******************************************************
** FILE: StateSpaceModel.h
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

#ifndef STATESPACEMODEL_H
#define STATESPACEMODEL_H

#include "Model.h"

class StateSpaceModel : public Model
{
public:
    StateSpaceModel() = default;
    virtual ~StateSpaceModel() = default;

    vector<double> predict(vector<int> bgInputs, vector<float> insulinInputs,
                           bool saveFlag);
    vector<double> projectCorrection(vector<double> bgInputs,
                                     vector<float> insulinInputs,
                                     int sensitivity);
};

#endif // STATESPACEMODEL_H
