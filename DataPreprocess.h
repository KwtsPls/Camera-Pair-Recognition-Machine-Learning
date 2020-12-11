#ifndef PROJECT_DATAPREPROCESS_H
#define PROJECT_DATAPREPROCESS_H

#include "SecTable.h"

//Function to create a hash table containing stopwords
secTable *init_stopwords();

//Function to perform some basic cleaning on a string
char *text_cleaning(char *text);

//Check if word is in utf-16 encoding
int check_utf16(char *word);

//Function to remove stopwords from a given string
char *remove_stopwords(char *text,secTable *stopwords,secTable **vocabulary);

//Function to preprocess a given string
char *preprocess(char *text,secTable *stopwords,secTable **vocabulary);

#endif //PROJECT_DATAPREPROCESS_H
