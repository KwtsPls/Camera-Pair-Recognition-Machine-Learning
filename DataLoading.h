#ifndef PROJECT_DATALOADING_H
#define PROJECT_DATALOADING_H

//Data struct to save the sets more efficiently
typedef struct datasets{
    double **X_train;
    double **X_test;
    int *y_train;
    int *y_test;
    char **pairs_train;
    char **pairs_test;
}datasets;

//Function to return an integer in range
int random_int(int n);

//Function to swap integers
void swap_int(int *a,int *b);

//Function to swap doubles
void swap_vectors(double **a,double **b);

//Function to swap strings
void swap_string(char **a,char **b);

//Function to shuffle the given data
void shuffle_data(double **X,int *y,char **pairs,int n,int random_state);

//Function to split the data set into a train and test sets
datasets *split_train_test(double **X,int *y,char **pairs,int n,int random_state,float split,int *n_train,int *n_test);

//Function to initialize an empty datasets struct
datasets *init_dataset();

//Function to delete a dataset
void destroy_dataset(datasets **data);

#endif //PROJECT_DATALOADING_H
