#include <stdio.h>
#include <stdlib.h>
#include "JsonParser.h"
#include "ErrorHandler.h"
#include "HashTable.h"
#include "CsvReader.h"

int errorCode;

int main(){

    char *X_name = get_datasetX_name();
    //Dataset X not found - program will terminate
    if(X_name==NULL) {
        errorCode=DATA_X_NOT_FOUND;
        print_error();
        return 1;
    }
    HashTable *ht = initHashTable(TABLE_INIT_SIZE);

    printf("Loading data...\n\n");

    int check = Initialize_dataset_X(X_name,&ht);

    printf("\nData loading was successful!\n\n");

    printf("\nCreating cliques...\n\n");

    ht = csvParser("sigmod_large_labelled_dataset.csv",&ht);

    printf("\nFinished creating cliques!\n\n");

    printf("\nCreating file cliques.csv...\n");

    //Creating File to write to
    csvWriteCliques(&ht);

    printf("\nFile created successfuly!\n\n");

    cliqueDeleteHashTable(&ht,BUCKET_HARD_DELETE_MODE);
    free(X_name);
    return 0;
}