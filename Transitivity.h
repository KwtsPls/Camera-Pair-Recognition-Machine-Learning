#ifndef TRANSITIVITY_H
#define TRANSITIVITY_H

#include "HashTable.h"
#include "SecTable.h"

typedef struct pred_pair{
    char *left_sp;
    char *right_sp;
    double pred;
    int corrected;
}predictionPair;

//Function to initialize a predictionPair
predictionPair *initPredictionPair(char *left_sp,char *right_sp,double pred);

//Function to delete a predtictionPair
void deletePredictionPair(predictionPair *pair);

//Function to resolve transitivity issues on the predictions from our model
void resolve_transitivity_issues(char **pairs_train,double **X_train,int *y_train,int train,secTable *preds,HashTable *ht,secTable *vocabulary,char *bow_type,int vector_type, logisticreg *reg);

//Function to initialize the data structures with a dummy dataset
int Initialize_dummy_dataset(char *name,HashTable **ht);

//Parser for finding pairs of spec_ids in the csv file
HashTable *predictionsParser(char *filename,HashTable **ht);

//Function to create cliques and negative relations in the new dummy hash tables
void init_train_cliques(HashTable **data_ht,HashTable **pred_ht,char **pairs_train,int *y_train,int size);

//Function for resolving transitivity issues with the predicted pairs
void resolve(HashTable **data_ht, HashTable **pred_ht,secTable *preds,double **X_corrected, int *y_corrected,HashTable *ht,secTable *vocabulary,char *bow_type,int vector_type,logisticreg *reg);

#endif // TRANSITIVITY_H
