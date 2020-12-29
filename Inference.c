#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "JsonParser.h"
#include "ErrorHandler.h"
#include "HashTable.h"
#include "CsvReader.h"
#include "BagOfWords.h"
#include "DataPreprocess.h"
#include "LogisticRegression.h"

int errorCode;

int main(int argc, char *argv[]){

    /* Check for valid arguments and datasets */

    //Check if the correct number of arguments was given
    if(argc < 3){
        printf("Error: Correct call is ./inference -c filename\n");
        return 1;
    }

    //Initialize the flags for the training to be begin

    if(strcmp(argv[1],"-c")!=0){
        printf("Error: Correct call is ./inference -c filename\n");
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

    /* Load the trained model from the stats.txt file */

    printf("\nLoading Regressor...\n\n");

    char *filename;
    char *bow_type;
    int vector_type;

    logisticreg *model = create_logisticReg_fromFile("stats.txt",&filename,&bow_type,&vector_type);
    if(model == NULL){
        printf("Stats.txt doesn't exist. Pls rerun firstly the ./main, and then run this main for the inference\n");
        return 1;
    }

    /* Create cliques and negative relations from the given file */

    printf("\nCreating cliques...\n\n");



    int linesRead = 0;
    ht = csvParser(filename,&ht, &linesRead);

    printf("\nFinished creating cliques!\n\n");

    /*#####################################################*/


    /* Begin inference */

    printf("\nBegin Inference...\n\n");

    printf("%s\n",bow_type);

    csvInference(argv[2], ht, vocabulary, model, bow_type, vector_type);

    cliqueDeleteHashTable(&ht,BUCKET_HARD_DELETE_MODE);
    destroy_secTable(&vocabulary,ST_HARD_DELETE_MODE);
    free(X_name);
    delete_logisticReg(&model);

    free(bow_type);
    free(filename);


    return 0;



}