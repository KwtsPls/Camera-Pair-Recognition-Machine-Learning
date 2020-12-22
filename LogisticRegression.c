#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "LogisticRegression.h"

//Function to create Logistic Regressions
logisticreg *create_logisticReg(int numofN,int mode){
    //Allocating size 
    logisticreg *lr = malloc(sizeof(logisticreg));
    lr->error = 0.000001;
    //Vocabulary length + 1 for the bias
    if(mode==ABSOLUTE_DISTANCE)
        lr->numofN = numofN+1;
    //Vocabulary length * 2 + 1 for concatenated vectors + 1 for bias
    else
        lr->numofN = (numofN*2)+1;

    lr->vector_weights = malloc(lr->numofN * sizeof(double));
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
        z[i] = fabs(x[i-1]-y[i-1]);
    }

    //Add an additional column for bias w0
    z[0]=1.0;

    return z;
}

//Function to concatenate two given arrays into one
double *concatenate_vectors(double *x,double *y, int numofN){
    double *z;
    z = malloc(sizeof(double) * numofN);
    //Concatenate the vectors x and y
    for(int i=1; i<=(numofN-1)/2;i++) {
        z[i] = x[i-1];
        z[i+(numofN-1)/2] = y[i-1];
    }

    //Add an additional column for bias w0
    z[0]=1.0;

    return z;
}

//Function to calculate logistic regressions
logisticreg *fit_pair_logisticRegression(logisticreg *model, double *xi, int yi){

    // η - learning rate
    double learning_rate = 0.0004;

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
            // w(t+1) = w(t) - η*gradient 
            new_weight[j] = model->vector_weights[j] - learning_rate*gradient;
        }

        int train_flag=0;
        for(int j=0;j<model->numofN;j++){
            if(fabs(model->vector_weights[j] - new_weight[j])<model->error){
                train_flag = 1;
                break;
            }
        }

        //Update the old weights
        for(int j=0;j<model->numofN; j++)
            model->vector_weights[j] = new_weight[j];
        
        if(train_flag==1)
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
double loss_LogisticRegression(logisticreg *model, double *xi,int yi){
    double error=0.0;
    double wtx=0.0;
    for(int z=0;z<model->numofN;z++)
        wtx += model->vector_weights[z]*xi[z];
    wtx = sigmoid(wtx);

    if(yi==1)
        error = -(((double)yi)*log(wtx));
    else
        error = -(((double)(1.0-yi))*log(1.0-wtx));

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
