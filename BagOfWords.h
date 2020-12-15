#ifndef PROJECT_BAGOFWORDS_H
#define PROJECT_BAGOFWORDS_H

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

#endif //PROJECT_BAGOFWORDS_H
