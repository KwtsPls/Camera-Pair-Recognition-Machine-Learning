#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "ErrorHandler.h"
#include "LogisticRegression.h"
#include "sparseVector.h"

//Function to create Logistic Regressions
logisticreg *create_logisticReg(int numofN,int mode,int steps,int batches,double learning_rate,int ratio){
    //Allocating size 
    logisticreg *lr = malloc(sizeof(logisticreg));
    
    //Number of steps that learning needs
    lr->steps = steps;

    //Number of batches
    lr->batches = batches;

    if(ratio==0)
        lr->ratio=1;
    else
        lr->ratio=ratio;

    // η - learning rate
    lr->learning_rate = learning_rate;
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

//Parsing a file that is the stats of the previous state of the model
logisticreg *create_logisticReg_fromFile(char *filename, char **sigmod_filename,char **bow_type, int *vector_type){
    //Allocating size
    logisticreg *model = malloc(sizeof(logisticreg));
    //Ratio is not important in inference
    model->ratio=1;

    //File parsing

    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    size_t read;
    int linesRead = 0;
    int i = 0;

    //Open file to read..
    fp = fopen("stats.txt","r");

    //Check if file Opened
    if(fp==NULL){
        errorCode = OPENING_FILE;
        fclose(fp);
        print_error();
        return NULL;
    }

    while((read = getline(&line, &len,fp))!=-1){
        linesRead++;
        char *str;
        switch (i){
        case 0:
            //Filename,bowtype,vector_type
            str = strtok(line," ,\n");
            //FIlename
            *sigmod_filename = strdup(str);
            
            str = strtok(NULL," ,\n");
            //bowtype
            *bow_type = strdup(str);

            //vector_type from string to Int
            str=strtok(NULL," ,\n");
            if(strcmp("abs",str) == 0)
                *vector_type = ABSOLUTE_DISTANCE;
            else
                *vector_type = CONCAT_VECTORS;    

            break;
        case 1:
            //Number
            str = strtok(line," :\n");
            //Of
            str = strtok(NULL," :\n");
            //N
            str = strtok(NULL," :\n");

            //Number of N:
            str = strtok(NULL," :\n");
            model->numofN = atoi(str);
            break;
        case 2:
            if(linesRead == 3)
                model->vector_weights = malloc(sizeof(double)*(model->numofN));
            else
                model->vector_weights[linesRead-4] = atof(line);
            break;
        case 3:
            //Steps
            str = strtok(line," :\n");
            
            //Steps :
            str = strtok(NULL," :\n");
            model->steps = atoi(str);            
            break;
        case 4:
            //Learning
            str = strtok(line," :\n");
            //rate
            str = strtok(NULL," :\n");
            
            //Learning rate :
            str = strtok(NULL," :\n");
            model->learning_rate = atof(str);
            break;
        case 5:
            //Batches
            str = strtok(line," :\n");
            
            //Batches :
            str = strtok(NULL," :\n");
            model->learning_rate = atoi(str);
        }
        //When parsing weights need to parse every one of them so i needs to be increased only when we parsed every weight
        if(i!=2)
            i++;
        else if(linesRead - 3 == model->numofN)
            i++;
    }
    free(line);    
    fclose(fp);

    return model;

}



//Function to create the vector for training
double *vectorize(double *x, double *y, int numofN,int type,int *sparse_size){
    if(type==ABSOLUTE_DISTANCE)
        return absolute_distance(x,y,numofN,sparse_size);
    else   
        return concatenate_vectors(x,y,numofN,sparse_size);
}

//Function to calculate euclidean distance
double *absolute_distance(double *x, double *y, int numofN,int *sparse_size){
    double *z;
    z = malloc(sizeof(double) * numofN);
    //Calculate the absolute distance between every value in the pair (x,y)
    // zi = | xi - yi |
    for(int i=1; i<numofN;i++){
        z[i] = fabs(x[i-1]-y[i-1]);
        if(z[i]!=0.0) (*sparse_size)++;
    }
    //Add an additional column for bias w0
    (*sparse_size)++;
    z[0]=1.0;

    return z;
}

//Function to concatenate two given arrays into one
double *concatenate_vectors(double *x,double *y, int numofN,int *sparse_size){
    double *z;
    z = malloc(sizeof(double) * numofN);
    //Concatenate the vectors x and y
    for(int i=0; i<(numofN-1)/2;i++) {
        z[i+1] = x[i];
        if(z[i+1]!=0.0) (*sparse_size)++;
        z[i+1+(numofN-1)/2] = y[i];
        if(z[i+1+(numofN-1)/2]!=0.0) (*sparse_size)++;
    }

    //Add an additional column for bias w0
    (*sparse_size)++;
    z[0]=1.0;

    return z;
}

//Function to return the norm || x - y ||
double norm_distance(double *x, double *y, int numofN){
    double norm=0.0;
    for(int i=0; i<numofN;i++){
        norm += (x[i]-y[i])*(x[i]-y[i]);
    }
    norm = sqrt(norm);

    return norm;
}

//Function to calculate logistic regressions
logisticreg *fit_logisticRegression(logisticreg *model,sparseVector **X,int *y,int size){


    //Initialize previous weights
    double *new_weight = malloc(sizeof(double)*model->numofN);
    int cnt = 0;
    //Changing weights 1 to 1, until the error is reached...
    for(int s=0;s<model->steps;s++){  
        cnt++;
        for(int j=0;j<model->numofN;j++){
            double gradient=0.0;
            double h=0.0;
            for(int i=0;i<size;i++){
                sparseVector *v_xi = X[i];
                //Find if xij is zero or not
                int index = find_index_sparseVector(v_xi,j);
                if(index==-1) continue;
                double *xi = v_xi->concentrated_matrix;
                double xij = v_xi->concentrated_matrix[index];
                if(y[i]==1){
                    for(int k=0;k<model->ratio;k++){
                        //Calculate w_T * x
                        for(int z=0;z<v_xi->num_elements;z++) {
                            int index = v_xi->index_array[z];
                            h += model->vector_weights[index] * xi[z];
                        }
                        h = sigmoid(h);
                        //(σ(w_T*xi) - yi)
                        h = (h - ((double) y[i]))*xij;
                        gradient += h;
                    }
                }
                else{
                    //Calculate w_T * x
                    for(int z=0;z<v_xi->num_elements;z++) {
                        int index = v_xi->index_array[z];
                        h += model->vector_weights[index] * xi[z];
                    }
                    h = sigmoid(h);
                    //(σ(w_T*xi) - yi)
                    h = (h - ((double) y[i]))*xij;
                    gradient += h;                   
                }
            }
            // w(t+1) = w(t) - η*gradient 
            new_weight[j] = model->vector_weights[j] - model->learning_rate*gradient;
        }

        //Update the current weights
        for(int j=0;j<model->numofN; j++)
            model->vector_weights[j] = new_weight[j];

    }
       

    free(new_weight);
    return model;
}

//Perform the training based on the model
logisticreg *train_logisticRegression(logisticreg *model,sparseVector **X,int *y,int size){
    int n = size/(model->batches);
    int r = size%(model->batches);
    //Perform a mini-batch training
    for(int i=0;i<(n-1);i++){
        sparseVector **X_batch = X+i*model->batches;
        int *y_batch = y+i*model->batches;
        model = fit_logisticRegression(model,X_batch,y_batch,model->batches);

        if((i*(model->batches))%1024==0 && i!=0)
            printf("Iterations: %d\n",i*(model->batches));
    }

    if(r!=0){
        sparseVector **X_batch = X+(n-1)*model->batches;
        int *y_batch = y+(n-1)*model->batches;        
        model = fit_logisticRegression(model,X_batch,y_batch,r);
    }

    return model;
}

//Function for predicting the
double *predict_logisticRegression(logisticreg *model,sparseVector **X,int n){
    double *y_pred=malloc(sizeof(double)*n);
    for(int i=0;i<n;i++){
        double yi_pred=0.0;
        sparseVector *v_xi = X[i];
        double *xi = v_xi->concentrated_matrix;
        for(int j=0;j<v_xi->num_elements;j++) {
            int index = v_xi->index_array[j];
            yi_pred += model->vector_weights[index] * xi[j];
        }
        yi_pred = sigmoid(yi_pred);
        y_pred[i] = yi_pred;
    }
    return y_pred;
}

//Function to calculate loss
double loss_LogisticRegression(logisticreg *model,sparseVector **X,int *y,int low,int high){

    double total_error=0.0;
    for(int i=low;i<high;i++) {
        double error = 0.0;
        double wtx = 0.0;
        sparseVector *v_xi = X[i];
        double *xi = v_xi->concentrated_matrix;
        int yi = y[i];
        for(int z=0;z<v_xi->num_elements;z++) {
            int index = v_xi->index_array[z];
            wtx += model->vector_weights[index] * xi[z];
        }
        wtx = sigmoid(wtx);

        if (yi == 1)
            error = -(((double) yi) * log10(wtx));
        else
            error = -(((double) (1.0 - yi)) * log10(1.0 - wtx));

        total_error += error;
    }

    return total_error;
}

//Function to get the result of a prediction for a given input
double hypothesis(logisticreg *model,sparseVector *x){
    double wtx=0.0;
    double *xi = x->concentrated_matrix;
    for(int z=0;z<x->num_elements;z++) {
        int index = x->index_array[z];
        wtx += model->vector_weights[index] * xi[z];
    }

    return sigmoid(wtx);  
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


//Function to print statistics to file
void printStatistics(logisticreg *model, char *filename,char *bow_type, int vector_type){
    FILE *fp;
    //Open file to write to...
    fp = fopen("stats.txt","w+");

    int err = 0;

    //Check if file Opened
    if(fp==NULL){
        errorCode = OPENING_FILE;
        fclose(fp);
        print_error();
        return;
    }

    char *vector_string;
    if(vector_type == CONCAT_VECTORS)
        vector_string = strdup("concat");
    else
        vector_string = strdup("abs");
    

    err = fprintf(fp,"%s,%s,%s\n",filename,bow_type,vector_string);
   
    free(vector_string);
   
    if(err<0){
        errorCode = WRITING_TO_FILE;
        print_error();
    }
            
    //Print everything from our model, so we can use it again in another program
    for(int i=0;i<5;i++){
        switch (i){
        case 0:
            err=fprintf(fp,"Number of N : %d\n",model->numofN);
            break;
        case 1:
            err = fprintf(fp,"Weights:\n");
            if(err<0){
                    errorCode = WRITING_TO_FILE;
                    print_error();
            }
            for(int j=0;j<model->numofN;j++){
                err = fprintf(fp,"%f\n",model->vector_weights[j]);
                    //Something went wrong while writing to the file....
                if(err<0){
                    errorCode = WRITING_TO_FILE;
                    print_error();
                }
            }
            break;
        case 2:
            err=fprintf(fp,"Steps : %d\n",model->steps);
            break;
        case 3:
            err=fprintf(fp,"Learning rate : %f\n",model->learning_rate);
            break;
        case 4:
            err=fprintf(fp,"Batches : %d\n",model->batches);
        }
        //Something went wrong while writing to the file....
        if(err<0){
            errorCode = WRITING_TO_FILE;
            print_error();
        }
    }
    fclose(fp);
}