################################################################################
# FILE: RandomForestTrain.py
#
# ABSTRACT:
# Loads the data from the file provided and uses it to train the RandomForest
# model for the relationship between blood glucose and plasma insulin 
# concentration to project blood sugar over a 90 minute predcition horizon.
# It reports statistics of the model after testing it and saves the model 
# to disk as RandomForest.sav
#
# DOCUMENTS:
# 
#
# AUTHOR:
# Daniel Webb
#
# CREATION DATE:
# 08/10/2019
#
# NOTES:
#
################################################################################

import pandas as pd
from sklearn.model_selection import train_test_split
import numpy as np
from sklearn.ensemble import RandomForestRegressor
from sklearn.externals import joblib


def run(fn, output):
   
    names = ['BG1', 'BG2', 'BG3', 'BG4', 'BG5', 'BG6', 'IOB', 'I5', 'I10','I15',
    'I20', 'I25', 'I30','I35', 'I40', 'I45', 'I50', 'I55', 'I60', 'I65' ,'I70' ,
    'I75', 'I80', 'I85', 'I90', 'FIVE', 'TEN', 'FIFTEEN', 'TWENTY','TWENTYFIVE',
     'THIRTY', '35', '40', '45', '50', '55', '60', '65' ,'70' ,'75', '80', '85',
      '90']
    dataset = pd.read_csv(fn, names=names)
    dataset.dropna(inplace=True)
    y = dataset
    y = y.drop('BG1',1)
    y = y.drop('BG2', 1)
    y = y.drop('BG3', 1)
    y = y.drop('BG4', 1)
    y = y.drop('BG5', 1)
    y = y.drop('BG6', 1)
    y = y.drop('IOB', 1)
    y = y.drop('I5',1)
    y = y.drop('I10', 1)
    y = y.drop('I15', 1)
    y = y.drop('I20', 1)
    y = y.drop('I25', 1)
    y = y.drop('I30', 1)
    y = y.drop('I35', 1)
    y = y.drop('I40', 1)
    y = y.drop('I45', 1)
    y = y.drop('I50', 1)
    y = y.drop('I55', 1)
    y = y.drop('I60', 1)
    y = y.drop('I65', 1)
    y = y.drop('I70', 1)
    y = y.drop('I75', 1)
    y = y.drop('I80', 1)
    y = y.drop('I85', 1)
    y = y.drop('I90', 1)
    # Remove the labels from the features
    dataset = dataset.drop('THIRTY', 1)
    dataset = dataset.drop('TWENTYFIVE', 1)
    dataset = dataset.drop('TWENTY', 1)
    dataset = dataset.drop('FIFTEEN', 1)
    dataset = dataset.drop('TEN', 1)
    dataset = dataset.drop('FIVE', 1)
    dataset = dataset.drop('35', 1)
    dataset = dataset.drop('40', 1)
    dataset = dataset.drop('45', 1)
    dataset = dataset.drop('50', 1)
    dataset = dataset.drop('55', 1)
    dataset = dataset.drop('60', 1)
    dataset = dataset.drop('65', 1)
    dataset = dataset.drop('70', 1)
    dataset = dataset.drop('75', 1)
    dataset = dataset.drop('80', 1)
    dataset = dataset.drop('85', 1)
    dataset = dataset.drop('90', 1)
    X = np.array(dataset)
    y = np.array(y)
    # X = sm.add_constant(X, prepend=False)
    # Training and test data sets 0.20
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.10)
    # print (X_train.shape, y_train.shape)
    # print (X_test.shape, y_test.shape)

    # Instantiate model with 100 decision trees
    rf = RandomForestRegressor(n_estimators=100)

    # Train the model on training data
    rf.fit(X_train.astype(float), y_train.astype(float));

    predictions = rf.predict(X_test)

    #Calculate the absolute errors
    errors = abs(predictions - y_test)
    
    #Print out the mean absolute error (mae)
    print("Mean Absolute Error:", round(np.mean(errors), 2), "mg/dL.")
    
    #Calculate mean absolute percentage error (MAPE)
    mape = 100 * (errors / y_test)

    #Calculate and display accuracy
    accuracy = 100 - np.mean(mape)
    print("Accuracy:", round(accuracy, 2), "%.")
  
    # save the model to disk
    filename = output
    joblib.dump(rf, filename)

def main():
    run("add.txt","RandomForest.sav")

if __name__ == '__main__':

    main()
