#include <stdlib.h>
#include <string.h>
#include "BagOfWords.h"

//Function to create an indexed word
indexedWord *createIndexedWord(char *word,int index){

    indexedWord *w = malloc(sizeof(indexedWord));
    w->word = malloc(strlen(word)+1);
    strcpy(w->word,word);
    w->index = index;
    w->idf=0.0;
    w->tf=1.0;
    w->tf_idf_mean=0.0;

    return w;
}

//Function to delete an indexed word
void deleteIndexedWord(indexedWord *iw){
    free(iw->word);
    free(iw);
}

//Function to sort an array of indexed words
void quicksortIndexedWord(indexedWord **arr,int low,int high,SortComparison fun){
    if (low < high)
    {
        /* pi is partitioning index, arr[pi] is now
           at right place */
        int pi = partitionIndexedWord(arr, low, high,fun);

        quicksortIndexedWord(arr, low, pi - 1,fun);  // Before pi
        quicksortIndexedWord(arr, pi + 1, high,fun); // After pi
    }
}

//Function to perform partition to a given part of the array
int partitionIndexedWord(indexedWord **arr,int low,int high,SortComparison fun){
    indexedWord *pivot = arr[high]; // pivot
    int i = (low - 1); // Index of smaller element

    for (int j = low; j <= high - 1; j++)
    {
        // If current element is smaller than the pivot
        if (fun(arr[j],pivot)>=0)
        {
            i++; // increment index of smaller element
            swapIndexedWords(&arr[i], &arr[j]);
        }
    }
    swapIndexedWords(&arr[i + 1], &arr[high]);
    return (i + 1);
}

//Function to swap to given indexed words
void swapIndexedWords(indexedWord **a,indexedWord **b){
    indexedWord *t = *a;
    *a = *b;
    *b = t;
}

//Function to compare words based on their tf_idf_mean
int compare_with_tf_idf(indexedWord *a,indexedWord *b){
    if(a->tf_idf_mean > b->tf_idf_mean) return 1;
    else if(a->tf_idf_mean < b->tf_idf_mean) return -1;
    else return 0;
}

//Function to compare words based on their tf_idf_mean
int compare_word(indexedWord *a,indexedWord *b){
    return strcmp(a->word,b->word);
}

//Function to return an array representing a bow with tf-idf values for a given spec id
double *getBagOfWords(HashTable *ht,secTable *vocabulary,char *spec_id,char *mode){


    //Return bag of words with tf-idf values
    if(strcmp(mode,"tf-idf")==0){
        //Get the length of the vocabulary
        int len = vocabulary->num_elements;

        //Initialize the array representing the bow
        double *bow = malloc(sizeof(double)*len);
        double *idf_array = malloc(sizeof(double)*len);

        //Initialize every value at zero
        for(int i=0;i<len;i++) {
            bow[i] = 0.0;
            idf_array[i]=0.0;
        }

        //Get the dictionary holding the information of the current spec
        Dictionary *dict = findSpecHashTable(ht,spec_id);

        int text_len=0;
        char *text = strdup(dict->list->value[0]);
        char *temp = text;
        char *cur=NULL;
        indexedWord *iw;

        //Iterate through the text to find it's length and calculate how many times each word from our vocabulary exists
        while((cur = strsep(&text, " "))!= NULL){
            if((iw=getIndexedWord_secTable(vocabulary,cur))!=NULL){
                text_len++;
                bow[iw->index]+=1.0;
                idf_array[iw->index] = iw->idf;
            }
        }

        for(int i=0;i<len;i++){
            //calculate tf-idf value for the word in this text
            if(bow[i]!=0.0)
                bow[i] = (bow[i]/(double)text_len)*idf_array[i];
        }

        free(idf_array);
        free(temp);
        return bow;
    }
    //Return simple bag of words
    else{
        //Get the length of the vocabulary
        int len = vocabulary->num_elements;

        //Initialize the array representing the bow
        double *bow = malloc(sizeof(double)*len);

        //Initialize every value at zero
        for(int i=0;i<len;i++) {
            bow[i] = 0.0;
        }

        //Get the dictionary holding the information of the current spec
        Dictionary *dict = findSpecHashTable(ht,spec_id);

        char *text = strdup(dict->list->value[0]);
        char *temp = text;
        char *cur=NULL;
        indexedWord *iw;

        //Iterate through the text to find it's length and calculate how many times each word from our vocabulary exists
        while((cur = strsep(&text, " "))!= NULL){
            if((iw=getIndexedWord_secTable(vocabulary,cur))!=NULL){
                bow[iw->index]+=1.0;
            }
        }

        free(temp);
        return bow;
    }
}