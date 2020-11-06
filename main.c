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
    HashTable *ht = initHashTable(2);
    int check = Initialize_dataset_X(X_name,&ht);
    ht = csvParser("sigmod_medium_labelled_dataset.csv",&ht);
    printHashTable(ht);
    deleteHashTable(&ht,BUCKET_DELETE_MODE);
    free(X_name);
    return 0;
}