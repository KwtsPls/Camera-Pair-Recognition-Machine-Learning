#ifndef TRANSITIVITY_H
#define TRANSITIVITY_H

#include "HashTable.h"

//Function to resolve transitivity issues on the predictions from our model
void resolve_transitivity_issues(char *filename,char *dirname,HashTable *ht);

//Function to initialize the data structures with a dummy dataset
int Initialize_dummy_dataset(char *name,HashTable **ht);

//Parser for finding pairs of spec_ids in the csv file
HashTable *predictionsParser(char *filename,HashTable **ht);

#endif // !TRANSITIVITY_H
