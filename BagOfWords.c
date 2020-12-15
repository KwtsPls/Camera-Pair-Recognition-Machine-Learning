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