#ifndef PROJECT_JSONPARSER_H
#define PROJECT_JSONPARSER_H

#include "ErrorHandler.h"
#include "HashTable.h"
#include "SecTable.h"

//Function to find the name of the directory holding the dataset X
char *get_datasetX_name();

//Function to initialize the data structures with dataset X
int Initialize_dataset_X(char *,HashTable **,secTable **);

//Function to read data from json files
Dictionary *parse_json_file(char *,char *,secTable *stopwords,secTable **vocabulary);

#endif //PROJECT_JSONPARSER_H
