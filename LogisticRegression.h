#ifndef LOGISTICREGRESSION_H
#define LOGISTICREGRESSION_H
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define ABSOLUTE_DISTANCE 0
#define CONCAT_VECTORS 1

typedef struct logisticreg{
    int numofN;
    double *vector_weights;
    double error;
}logisticreg;

//Function to create Logistic Regressions
logisticreg *create_logisticReg(int numofN,int mode);

//Function to calculate logistic regressions
logisticreg *fit_pair_logisticRegression(logisticreg *model,double *xi,int yi);

//Function to calculate euclidean distance
double *euclidean_distance(double *x, double *y, int numofN);

//Function to concatenate two given arrays into one
double *concatenate_vectors(double *x,double *y, int numofN);

//Function to check if our model is trained
double predict_pair_logisticRegression(logisticreg *model,double *xi);

//Function to calculate loss
double loss_LogisticRegression(logisticreg *model, double *xi,int yi);

//Function σ(t) = 1/1 + e^(-t)
double sigmoid(double t);

//Function to free memory for the logistic regression struct
void delete_logisticReg(logisticreg **del);

#endif // !LOGISTICREGRESSION_H