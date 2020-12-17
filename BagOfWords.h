#ifndef PROJECT_BAGOFWORDS_H
#define PROJECT_BAGOFWORDS_H
#include "HashTable.h"
#include "SecTable.h"

typedef struct indexed_word{
    char *word;
    int index;
    double idf;
    double tf;
    double tf_idf_mean;
}indexedWord;

//Function to create an indexed word
indexedWord *createIndexedWord(char *word,int index);

//Function to delete an indexed word
void deleteIndexedWord(indexedWord *iw);

//Function to return an array representing a bow with tf-idf values for a given spec id
double *getBagOfWords(HashTable *ht,secTable *vocabulary,char *spec_id,char *mode);

//Helper function to test bow
void test_bow_implementation(HashTable *ht,secTable *vocabulary,char *filename);

#endif //PROJECT_BAGOFWORDS_H
