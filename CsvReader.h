#ifndef CSVREADER_H
#define CSVREADER_H
#include <stdio.h>
#include <stdlib.h>
#include "DataLoading.h"
#include "ErrorHandler.h"
#include "BinaryHeap.h"
#include "LogisticRegression.h"
#include <errno.h>

//Parser for finding pairs of spec_ids in the csv file
HashTable *csvParser(char *filename, HashTable **ht, int *linesRead,int *pos_num,int *neg_num);

//Function for learning
void csvLearning(char *filename, HashTable *ht, secTable *vocabulary, int linesRead,char *bow_type,int vector_type,int ratio);

//Function to get the predictions from all pairs from the data
BHTree *predict_all_pairs(logisticreg *regressor,float threshold,HashTable *ht,secTable *vocabulary,char *bow_type,int vector_type);

//Function for writing negative cliques
void csvWriteNegativeCliques(HashTable **ht);

//Function to create a new csv to write the cliques into
void csvWriteCliques(HashTable **ht);

//Function for inference
void csvInference(char *filename, HashTable *ht, secTable *vocabulary, logisticreg *model, char *bow_type, int vector_type);

//Function for writing predictions to file
void csvWritePredictions(datasets *data, double *pred, int test_size);

#endif