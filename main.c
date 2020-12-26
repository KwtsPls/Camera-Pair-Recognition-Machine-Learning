#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "JsonParser.h"
#include "ErrorHandler.h"
#include "HashTable.h"
#include "CsvReader.h"
#include "BagOfWords.h"
#include "DataPreprocess.h"

int errorCode;

int main(int argc, char *argv[]){

    /* Check for valid arguments and datasets */

    //Check if the correct number of arguments was given
    if(argsCheck(argc)==0)
        return 1;

    //Initialize the flags for the training to be begin
    char *f=NULL;
    int n;
    char *v=NULL;
    char *b=NULL;
    if(initArgs(argc,argv,&f,&n,&v,&b)==0){
        cleanArgs(f,v,b);
        return 1;
    }

    char *X_name = get_datasetX_name();
    //Dataset X not found - program will terminate
    if(X_name==NULL) {
        errorCode=DATA_X_NOT_FOUND;
        print_error();
        return 1;
    }

    /*############################################*/


    /* Initialize structs to be used for the training */

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

    /*###################################################*/



    /* Create cliques and negative relations from the given file */

    printf("\nCreating cliques...\n\n");

    int linesRead = 0;
    ht = csvParser(f,&ht, &linesRead);

    printf("\nFinished creating cliques!\n\n");

    /*#####################################################*/


    /* Start the training of the model */

    printf("\nBegin Training...\n\n");

    csvLearning(f,ht,vocabulary,linesRead);

    printf("\nCreating file cliques.csv...\n");

    /*#####################################*/


    /* Write the statistics of the program and free all allocated memory */

    //Save the create cliques into a file
    csvWriteCliques(&ht);
    //If the flag for negative cliques is on write the negative cliques as well in a different file
    if(n == 1) csvWriteNegativeCliques(&ht);

    printf("\nFile created successfully!\n\n");

    cliqueDeleteHashTable(&ht,BUCKET_HARD_DELETE_MODE);
    destroy_secTable(&vocabulary,ST_HARD_DELETE_MODE);
    free(X_name);
    cleanArgs(f,v,b);
    return 0;
}