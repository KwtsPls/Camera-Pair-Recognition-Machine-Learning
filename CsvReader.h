#ifndef CSVREADER_H
#define CSVREADER_H
#include <stdio.h>
#include <stdlib.h>
#include "ErrorHandler.h"
#include "LogisticRegression.h"
#include <errno.h>

//Parser for finding pairs of spec_ids in the csv file
HashTable *csvParser(char *filename, HashTable **ht, int *linesRead);

//Function for learning
void csvLearning(char *filename, HashTable *ht, secTable *vocabulary, int linesRead,char *bow_type,int vector_type);

//Function for writing negative cliques
void csvWriteNegativeCliques(HashTable **ht);

//Function to create a new csv to write the cliques into
void csvWriteCliques(HashTable **ht);

#endif