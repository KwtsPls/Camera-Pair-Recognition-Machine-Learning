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
    double learning_rate;
    int steps;
    int batches;
}logisticreg;

//Function to calculate logistic regressions
logisticreg *fit_logisticRegression(logisticreg *model,double **X,int *y,int low,int high);

//Function to create Logistic Regressionsrning rate
logisticreg *create_logisticReg(int numofN,int mode,int steps,int batches,double learning_rate);

//Parsing a file that is the stats of the previous state of the model
logisticreg *create_logisticReg_fromFile(char *filename,char **sigmod_filename, char **bow_type, int *vector_type);

//Function to create the vector for training
double *vectorize(double *x, double *y, int numofN,int type);

//Function to calculate euclidean distance
double *absolute_distance(double *x, double *y, int numofN);

//Function to concatenate two given arrays into one
double *concatenate_vectors(double *x,double *y, int numofN);

//Function to return the norm || x - y ||
double norm_distance(double *x, double *y, int numofN);

//Function to check if our model is trained
double *predict_logisticRegression(logisticreg *model,double **X,int train,int n);

//Function to calculate loss
double loss_LogisticRegression(logisticreg *model, double *xi,int yi);

//Function σ(t) = 1/1 + e^(-t)
double sigmoid(double t);

//Perfom the training based on the model
logisticreg *train_logisticRegression(logisticreg *model,double **X,int *y,int size);

//Function to print statistics to file
void printStatistics(logisticreg *model,char *filename,char *bow_type, int vector_type);

//Function to free memory for the logistic regression struct
void delete_logisticReg(logisticreg **del);

#endif // !LOGISTICREGRESSION_H