#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "JsonParser.h"
#include "ErrorHandler.h"
#include "HashTable.h"
#include "CsvReader.h"
#include "BagOfWords.h"

int errorCode;

int main(int argc, char *argv[]){

    char *X_name = get_datasetX_name();
    //Dataset X not found - program will terminate
    if(X_name==NULL) {
        errorCode=DATA_X_NOT_FOUND;
        print_error();
        return 1;
    }

    //Hashtable containing the cliques
    HashTable *ht = initHashTable(TABLE_INIT_SIZE);

    //Hashtable for the vocabulary
    secTable *vocabulary = create_secTable(ST_INIT_SIZE,SB_SIZE,HashIndexedWord,CompareIndexedWord,DeleteIndexedWord,indxWrd);

    printf("Loading data...\n\n");

    int check = Initialize_dataset_X(X_name,&ht,&vocabulary);
    if(check==-1) {
        printf("Dataset initialization was not successful...");
        deleteHashTable(&ht,BUCKET_HARD_DELETE_MODE);
        destroy_secTable(&vocabulary,ST_HARD_DELETE_MODE);
        return 1;
    }

    printf("\nData loading was successful!\n\n");

    printf("\nFound %d unique words...\n\n",vocabulary->num_elements);

    //Get the updated vocabulary
    vocabulary = evaluate_tfidf_secTable(vocabulary,sizeHashTable(ht));

    printf("\nWords we will keep %d...\n\n",vocabulary->num_elements);

    printf("\nCreating cliques...\n\n");

    int linesRead = 0;
    ht = csvParser("sigmod_large_labelled_dataset.csv",&ht, &linesRead);

    printf("\nFinished creating cliques!\n\n");

    printf("\nBegin Training...\n\n");

    //csvLearning("sigmod_medium_labelled_dataset.csv",ht,vocabulary,linesRead);

    printf("\nCreating file cliques.csv...\n");

    //Creating File to write to
    csvWriteCliques(&ht);
    int print_neg_flag=1;
    if(print_neg_flag == 1)
        csvWriteNegativeCliques(&ht);

    printf("\nFile created successfully!\n\n");

    cliqueDeleteHashTable(&ht,BUCKET_HARD_DELETE_MODE);
    destroy_secTable(&vocabulary,ST_HARD_DELETE_MODE);
    free(X_name);
    return 0;
}