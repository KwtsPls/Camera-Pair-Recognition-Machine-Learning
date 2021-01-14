#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "DataLoading.h"

//Function to swap integers
void swap_int(int *a,int *b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

//Function to swap doubles
void swap_vectors(double **a,double **b){
    double *temp = *a;
    *a = *b;
    *b = temp;
}

//Function to swap strings
void swap_string(char **a,char **b){
    char *temp = *a;
    *a = *b;
    *b = temp;
}

//Function to return an integer in range
int random_int(int n){
    int e =  n/RAND_MAX;
    int mod = n % RAND_MAX;
    int r = (rand()%RAND_MAX)*e + (rand()%mod);
    return r;
}

//Function to shuffle the given data
void shuffle_data(double **X,int *y,char **pairs,int n,int random_state){

    //For the given number of random_state shuffle the array
    for(int k=0;k<random_state;k++){
        for(int i=0;i<n;i++){
            int random_index = random_int(n);
            swap_int(&y[i],&y[random_index]);
            swap_vectors(&X[i],&X[random_index]);
            swap_string(&pairs[i],&pairs[random_index]);
        }
    }

}

//Function to initialize an empty datasets struct
datasets *init_dataset(){
    datasets *data = malloc(sizeof(datasets));
    data->X_train=NULL;
    data->X_test=NULL;

    data->y_train=NULL;
    data->y_test=NULL;

    data->pairs_train=NULL;
    data->pairs_test=NULL;

    return data;
}

//Function to delete a dataset
void destroy_dataset(datasets **data){
    free((*data)->X_train);
    free((*data)->X_test);
    free((*data)->y_train);
    free((*data)->y_test);
    free((*data)->pairs_train);
    free((*data)->pairs_test);
    free(*data);
}

//Function to split the data set into a train and test sets
datasets *split_train_test(double **X,int *y,char **pairs,int n,int random_state,float split,int *n_train,int *n_test){

    datasets *data = init_dataset();

    //Shuffle the data before splitting it
    shuffle_data(X,y,pairs,n,random_state);

    //Set the size of the train set
    *n_train = (int)(((float)n)*(1.0-split));
    //Set the size of the test set
    *n_test = n - *n_train;

    //Split the input set into train and test
    data->X_train = malloc((*n_train)*sizeof(double*));
    data->X_test = malloc((*n_test)*sizeof(double*));
    memcpy(data->X_train,X,(*n_train)*sizeof(double*));
    memcpy(data->X_test,X+(*n_train),(*n_test)*sizeof(double*));

    //Split the target set into train and test
    data->y_train = malloc((*n_train)*sizeof(int));
    data->y_test = malloc((*n_test)* sizeof(int));
    memcpy(data->y_train,y,(*n_train)*sizeof(int));
    memcpy(data->y_test,y+(*n_train),(*n_test)*sizeof(int));

    //Split the array with the pairs into train and test
    data->pairs_train = malloc((*n_train)* sizeof(char*));
    data->pairs_test = malloc((*n_test)* sizeof(char*));
    for(int i=0;i<*n_train;i++)
        data->pairs_train[i] = strdup(pairs[i]);
    for(int i=0;i<*n_test;i++)
        data->pairs_test[i] = strdup(pairs[(*n_train)+i]);

    for(int i=0;i<n;i++)
        free(pairs[i]);
    free(pairs);

    return data;
}

