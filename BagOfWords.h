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

// Compare functions for the different data types in the hash table
typedef int (*SortComparison)(indexedWord *, indexedWord *);
//Function to compare words based on their tf_idf_mean
int compare_with_tf_idf(indexedWord *a,indexedWord *b);
//Function to compare words based on their tf_idf_mean
int compare_word(indexedWord *a,indexedWord *b);


//Function to create an indexed word
indexedWord *createIndexedWord(char *word,int index);

//Function to delete an indexed word
void deleteIndexedWord(indexedWord *iw);

//Function to return an array representing a bow with tf-idf values for a given spec id
double *getBagOfWords(HashTable *ht,secTable *vocabulary,char *spec_id,char *mode);

//Function to sort an array of indexed words
void quicksortIndexedWord(indexedWord **arr,int low,int high,SortComparison fun);

//Function to perform partition to a given part of the array
int partitionIndexedWord(indexedWord **arr,int low,int high,SortComparison fun);

//Function to swap to given indexed words
void swapIndexedWords(indexedWord **a,indexedWord **b);

#endif //PROJECT_BAGOFWORDS_H
