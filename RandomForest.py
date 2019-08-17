################################################################################
# FILE: RandomForest.py
#
# ABSTRACT:
# Loads the already trained Random Forest model from disk, loads the desired
# inputs from the file test.txt, and supplies the inputs to the model, then 
# outputs the predictions to the console and the file RFResults.txt
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

import joblib
import pandas as pd

def getPredictions():
    '''
    Predict what BG will be in future.
    :param coefficient: all coefficients from ML OLS Regression for model
    :return float: predicted BG in 30 minutes
    '''

    # Load dataset
    url = "RandomForest/test.txt"
    names = ['BG1', 'BG2', 'BG3', 'BG4', 'BG5', 'BG6', 'IOB', 'I5', 'I10', 
             'I15', 'I20', 'I25', 'I30', 'I35', 'I40', 'I45', 'I50', 'I55', 
             'I60', 'I65', 'I70', 'I75', 'I80', 'I85', 'I90']
             
    dataset = pd.read_csv(url, names=names)
    df = pd.DataFrame(data=dataset, columns=names)
    # df.dropna(inplace=True)

    RFArray = [[df['BG1'][0], df['BG2'][0], df['BG3'][0], df['BG4'][0], 
                df['BG5'][0], df['BG6'][0], df['IOB'][0],
                df['I5'][0], df['I10'][0], df['I15'][0], df['I20'][0], 
                df['I25'][0], df['I20'][0], df['I35'][0],
                df['I30'][0], df['I45'][0], df['I40'][0], df['I55'][0], 
                df['I60'][0], df['I65'][0], df['I70'][0],
                df['I75'][0], df['I80'][0], df['I85'][0], df['I90'][0]]]
    
    RFModel = joblib.load("RandomForest/RandomForest.sav")
    RFResult = RFModel.predict(RFArray)
 
    #print to console for AGS
    print(RFResult[0][0])
    print(RFResult[0][1])
    print(RFResult[0][2])
    print(RFResult[0][3])
    print(RFResult[0][4])
    print(RFResult[0][5])
    print(RFResult[0][6])
    print(RFResult[0][7])
    print(RFResult[0][8])
    print(RFResult[0][9])
    print(RFResult[0][10])
    print(RFResult[0][11])
    print(RFResult[0][12])
    print(RFResult[0][13])
    print(RFResult[0][14])
    print(RFResult[0][15])
    print(RFResult[0][16])
    print(RFResult[0][17])
    
    #write resutls out to file for RF saver
    outFile = open("RandomForest/RFResults.txt",'w')
    outFile.write(str(RFResult[0][0])+","+str(RFResult[0][1])+","
    +str(RFResult[0][2])+","+str(RFResult[0][3])+","+str(RFResult[0][4])+","+
    str(RFResult[0][5])+","+str(RFResult[0][6])+","+str(RFResult[0][7])+","+
    str(RFResult[0][8])+","+str(RFResult[0][9])+","+str(RFResult[0][10])+","+
    str(RFResult[0][11])+","+str(RFResult[0][12])+","+str(RFResult[0][13])+","+
    str(RFResult[0][14])+","+str(RFResult[0][15])+","+str(RFResult[0][16])+","+
    str(RFResult[0][17]))
    outFile.close()


def main():
    getPredictions()


if __name__ == '__main__':
    main()
