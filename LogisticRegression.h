#ifndef LOGISTICREGRESSION_H
#define LOGISTICREGRESSION_H
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "JobScheduler.h"

#define ABSOLUTE_DISTANCE 0
#define CONCAT_VECTORS 1

typedef struct sparse_vector sparseVector;
typedef struct job_scheduler JobScheduler;

typedef struct logisticreg{
    int numofN;
    double *vector_weights;
    double learning_rate;
    int steps;
    int batches;
    int ratio;
}logisticreg;

//Function to calculate logistic regressions
void fit_logisticRegression(void *arguments);

//Function to create a Logistic Regression model
logisticreg *create_logisticReg(int numofN,int mode,int steps,int batches,double learning_rate,int ratio);

//Parsing a file that is the stats of the previous state of the model
logisticreg *create_logisticReg_fromFile(char *filename,char **sigmod_filename, char **bow_type, int *vector_type);

//Function to create the vector for training
double *vectorize(double *x, double *y, int numofN,int type,int *sparse_size);

//Function to calculate euclidean distance
double *absolute_distance(double *x, double *y, int numofN,int *sparse_size);

//Function to concatenate two given arrays into one
double *concatenate_vectors(double *x,double *y, int numofN,int *sparse_size);

//Function to return the norm || x - y ||
double norm_distance(double *x, double *y, int numofN);

//Function to check if our model is trained
double *predict_logisticRegression(logisticreg *model,sparseVector **X,int n);

//Function to calculate loss
double loss_LogisticRegression(logisticreg *model,sparseVector **X,int *y,int low,int high);

//Function σ(t) = 1/1 + e^(-t)
double sigmoid(double t);

//Perform the training based on the model
logisticreg *train_logisticRegression(logisticreg *model,sparseVector **X,int *y,int size,JobScheduler *scheduler);

//Function to print statistics to file
void printStatistics(logisticreg *model,char *filename,char *bow_type, int vector_type);

//Function to free memory for the logistic regression struct
void delete_logisticReg(logisticreg **del);

//Function to get the result of a prediction for a given input
double hypothesis(logisticreg *model,sparseVector *x);

#endif // !LOGISTICREGRESSION_H