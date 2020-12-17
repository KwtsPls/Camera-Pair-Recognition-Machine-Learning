#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "LogisticRegression.h"

//Function to create Logistic Regressions
logisticreg *create_logisticReg(int numofN){
    //Allocating size 
    logisticreg *lr = malloc(sizeof(logisticreg));

    lr->error = 0.00420;   
    lr->numofN = numofN;
    lr->vector_weights = malloc(numofN * sizeof(double));
    for(int i=0; i<lr->numofN; i++)
        lr->vector_weights[i] = 1.0; 
    return lr;
}


//Function to calculate euclidean distance
double *euclidean_distance(double *x, double *y, int numofN){
    double *z;
    z = malloc(sizeof(double) * numofN);
    //Euclidean distance d(x,y) = √|(x_(i))^2 - (y_(i))^2|
    for(int i=0; i<numofN;i++){
        z[i] = fabs(x[i]-y[i]);
    }
    return z;
}

//Function to calculate logistic regressions
logisticreg *fit_logisticRegression(logisticreg *model, double **X_train,int *y_train,int size){

    // η - learning rate
    double learning_rate = 0.001;

    //Initialize previous weights
    double *prev_weight = malloc(sizeof(double)*model->numofN);
    int cnt = 0;
    //Changing weights 1 to 1, until the error is reached...
    while(1){  
        cnt++;
        for(int j=0;j<model->numofN;j++){
            double gradient=0.0;
            for(int i=0;i<size;i++){
                double *xi = X_train[i];
                double xij = xi[j];

                //Calculate w_T * x
                double wtx=0.0;
                for(int z=0;z<model->numofN;z++)
                    wtx += model->vector_weights[z]*xi[z];
                wtx = sigmoid(wtx);
                //(σ(wT*xi) - yi)
                wtx = (wtx - (double) y_train[i])*xij;
                gradient += wtx;
            }
            prev_weight[j] = model->vector_weights[j];
            model->vector_weights[j] = model->vector_weights[j] - learning_rate*gradient;
        }

        /*int train_flag=0;
        for(int j=0;j<model->numofN;j++){
            if(fabs(model->vector_weights[j] - prev_weight[j])<model->error){
                train_flag = 1;
                break;
            }
        }
        printf("%d\n",cnt);

        if(train_flag==1)
            break;*/

        printf("%d\n",cnt);
        if(cnt==2)
            break;

    }
       

    free(prev_weight);
    return model;
}

double *inference_logisticRegression(logisticreg *model,double **X_test,int *y_test,int size){
    double *y_pred = malloc(sizeof(double)*size);
    for(int j=0;j<size;j++){
        y_pred[j]=0.0;
        for(int i=0; i<model->numofN; i++){
            y_pred[j] += model->vector_weights[i] * X_test[j][i]; 
        }
        y_pred[j] = sigmoid(y_pred[j]);
    }
    
    return y_pred;
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
