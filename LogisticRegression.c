#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "LogisticRegression.h"

//Function to create Logistic Regressions
logisticreg *create_logisticReg(int numofN){
    //Allocating size 
    logisticreg *lr = malloc(sizeof(logisticreg));
    printf("3 midenika\n");
    lr->error = 0.000420;   
    lr->numofN = numofN;
    lr->vector_weights = malloc(numofN * sizeof(double));
    for(int i=0; i<lr->numofN; i++)
        lr->vector_weights[i] = 0.0; 
    return lr;
}


//Function to calculate euclidean distance
double *euclidean_distance(double *x, double *y, int numofN){
    double *z;
    z = malloc(sizeof(double) * numofN);
    //Calculate the absolute distance between every value in the pair (x,y)
    // zi = | xi - yi |
    for(int i=1; i<numofN;i++){
        z[i] = fabs(x[i]-y[i]);
    }
    z[0]=1.0;
    return z;
}

//Function to calculate logistic regressions
logisticreg *fit_pair_logisticRegression(logisticreg *model, double *xi, int yi){

    // η - learning rate
    double learning_rate = 0.001;

    //Initialize previous weights
    double *new_weight = malloc(sizeof(double)*model->numofN);
    int cnt = 0;
    //Changing weights 1 to 1, until the error is reached...
    while(1){  
        cnt++;
        for(int j=0;j<model->numofN;j++){
            double gradient=0.0;
            double xij = xi[j];
            //Calculate w_T * x
            for(int z=0;z<model->numofN;z++)
                gradient += model->vector_weights[z]*xi[z];
            gradient = sigmoid(gradient);
            //(σ(w_T*xi) - yi)
            gradient = (gradient - ((double) yi))*xij;
            new_weight[j] = model->vector_weights[j] - learning_rate*gradient;
        }

        int train_flag=0;
        for(int j=0;j<model->numofN;j++){
            if(fabs(model->vector_weights[j] - new_weight[j])>=model->error){
                train_flag = 1;
                break;
            }
        }

        for(int j=0;j<model->numofN; j++)
            model->vector_weights[j] = new_weight[j];
        
        if(train_flag==0)
            break;

    }
       

    free(new_weight);
    return model;
}

double predict_pair_logisticRegression(logisticreg *model,double *xi){
    double y_pred=0.0;
    for(int j=0; j<model->numofN; j++){
        y_pred += model->vector_weights[j] * xi[j]; 
    }
    y_pred = sigmoid(y_pred);
    
    return y_pred;
}

//Function to calculate loss
double loss_LogisticRegression(logisticreg *model, double **X_train,int *y_train,int size){
    double error=0.0; 
    for(int i=0;i<size;i++){
        double *xi = X_train[i];
        double wtx=0.0;
        for(int z=0;z<model->numofN;z++)
            wtx += model->vector_weights[z]*xi[z];
        wtx = sigmoid(wtx);
        if(y_train[i]==1)
            error += -(((double)y_train[i])*log(wtx));
        else
            error += -(((double)(1-y_train[i]))*log(1-wtx));
    }
    error = error/(double)size;
    return error;
}

//Function σ(t) = 1/1 + e^(-t)
double sigmoid(double t){
    double sigma;
    sigma = (double) 1.0 / ((double) (1.0 + exp((double) (-t))));
    return sigma;
}

//Function to free memory for the logistic regression struct
void delete_logisticReg(logisticreg **del){
    free((*del)->vector_weights);
    free((*del));
    *del = NULL;
}
