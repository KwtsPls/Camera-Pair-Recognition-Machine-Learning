#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "DataLoading.h"
#include "BagOfWords.h"

//Function to swap integers
void swap_int(int *a,int *b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

//Function to swap doubles
void swap_vectors(sparseVector **a,sparseVector **b){
    sparseVector *temp = *a;
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
void shuffle_data(sparseVector **X,int *y,char **pairs,int n,int random_state){

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
    if((*data)->X_train!=NULL)
        free((*data)->X_train);
    if((*data)->X_test!=NULL)
        free((*data)->X_test);
    if((*data)->y_train!=NULL)
        free((*data)->y_train);
    if((*data)->y_test!=NULL)
        free((*data)->y_test);
    if((*data)->pairs_train!=NULL)
        free((*data)->pairs_train);
    if((*data)->pairs_test!=NULL)
        free((*data)->pairs_test);
    free(*data);
}

//Function to split the data set into a train and test sets
datasets *split_train_test(sparseVector **X,int *y,char **pairs,int n,int random_state,float split,int *n_train,int *n_test){

    datasets *data = init_dataset();

    //Shuffle the data before splitting it
    shuffle_data(X,y,pairs,n,random_state);

    //Set the size of the train set
    *n_train = (int)(((float)n)*(1.0-split));
    //Set the size of the test set
    *n_test = n - *n_train;

    //Split the input set into train and test
    data->X_train = malloc((*n_train)*sizeof(sparseVector*));
    data->X_test = malloc((*n_test)*sizeof(sparseVector*));

    //Split the target set into train and test
    data->y_train = malloc((*n_train)*sizeof(int));
    data->y_test = malloc((*n_test)* sizeof(int));
    memcpy(data->y_train,y,(*n_train)*sizeof(int));
    memcpy(data->y_test,y+(*n_train),(*n_test)*sizeof(int));

    //Split the array with the pairs into train and test
    data->pairs_train = malloc((*n_train)* sizeof(char*));
    data->pairs_test = malloc((*n_test)* sizeof(char*));
    for(int i=0;i<*n_train;i++) {
        data->pairs_train[i] = strdup(pairs[i]);
        data->X_train[i] = X[i];
    }
    for(int i=0;i<*n_test;i++) {
        data->pairs_test[i] = strdup(pairs[(*n_train) + i]);
        data->X_test[i] = X[(*n_train)+i];
    }

    for(int i=0;i<n;i++)
        free(pairs[i]);
    free(pairs);

    return data;
}


//Function to load the dataset from a given csv file
void load_data(char *filename,int linesRead,HashTable *ht,secTable *vocabulary,logisticreg *regressor,char *bow_type,int vector_type,sparseVector ***X,int **y,char ***pairs){

    FILE *fp;
    fp = fopen(filename,"r");
    char *line = NULL;
    size_t len = 0;
    size_t read;
    int lines=0;

    *X = malloc(sizeof(sparseVector)*linesRead);
    *y = malloc(sizeof(int)*linesRead);
    *pairs = malloc(sizeof(char*)*linesRead);

    while((read = getline(&line, &len,fp))!=-1){

        if(lines==0){ //Skip First Line cause its Left_spec, Right_Spec, label
            lines++;
            continue;
        }

        char *left_sp,*right_sp,*lbl_str;
        //Take left_spec_id
        left_sp = strtok(line,",");
        //Take right_spec_id
        right_sp = strtok(NULL,",");
        //Take label
        lbl_str = strtok(NULL,",");
        //Label to integer
        int label = atoi(lbl_str);

        double *l_x = getBagOfWords(ht,vocabulary,left_sp,bow_type);
        double *r_x = getBagOfWords(ht,vocabulary,right_sp,bow_type);
        int sparse_size=0;
        double *xi=vectorize(l_x,r_x,regressor->numofN,vector_type,&sparse_size);
        sparseVector *v_xi = init_sparseVector(xi,regressor->numofN,sparse_size);

        (*X)[lines-1]=v_xi;
        (*y)[lines-1]=label;
        char *new_pair = malloc(strlen(left_sp)+1+strlen(right_sp)+1);
        strcpy(new_pair,left_sp);
        strcat(new_pair,",");
        strcat(new_pair,right_sp);
        (*pairs)[lines-1]=new_pair;

        lines++;
        free(l_x);
        free(r_x);
    }

    fclose(fp);
    free(line);
}