#ifndef TRANSITIVITY_H
#define TRANSITIVITY_H

#include "HashTable.h"
#include "BinaryHeap.h"
#include "RBtree.h"
#include "LogisticRegression.h"

typedef struct bh_tree BHTree;
typedef struct rb_tree RBtree;
typedef struct rb_node RBnode;

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
int resolve_transitivity_issues(char ***pairs_train,sparseVector ***X_train,int **y_train,int train,RBtree *preds,HashTable *ht,secTable *vocabulary,char *bow_type,int vector_type, logisticreg *reg);

//Function to initialize the data structures with a dummy dataset
int Initialize_dummy_dataset(char *name,HashTable **ht);

//Function to create cliques and negative relations in the new dummy hash tables
void init_train_cliques(HashTable **data_ht,HashTable **pred_ht,char **pairs_train,int *y_train,int size);

//Function to perform a recursive search in the tree struct that contains the predicted pairs
void resolveRB(HashTable **data_ht, HashTable **pred_ht,RBtree *preds,char ***pairs_corrected,sparseVector ***X_corrected, int **y_corrected,HashTable *ht,secTable *vocabulary,char *bow_type,int vector_type,logisticreg *regressor);

//Function for resolving transitivity issues with the predicted pairs
void resolve(HashTable **data_ht, HashTable **pred_ht,RBnode *node,char ***pairs_corrected,sparseVector ***X_corrected, int **y_corrected,HashTable *ht,secTable *vocabulary,char *bow_type,int vector_type,logisticreg *reg,int *index);

#endif // TRANSITIVITY_H
