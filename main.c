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
    int v;
    int max_features;
    char *b=NULL;
    if(initArgs(argc,argv,&f,&n,&v,&max_features,&b)==0){
        cleanArgs(f,b);
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

    printf("#############################\n\n");
    printf("Loading data...\n\n");

    int check = Initialize_dataset_X(X_name,&ht,&vocabulary);
    if(check==-1) {
        printf("Dataset initialization was not successful...");
        deleteHashTable(&ht,BUCKET_HARD_DELETE_MODE);
        destroy_secTable(&vocabulary,ST_HARD_DELETE_MODE);
        return 1;
    }
    printf("\nData loading was successful!\n\n");
    printf("#############################\n\n");

    printf("#############################\n");
    printf("Vector statistics:\n\n");
    printf("-Found %d unique words\n",vocabulary->num_elements);

    //Get the updated vocabulary
    vocabulary = evaluate_tfidf_secTable(vocabulary,sizeHashTable(ht),max_features);

    printf("-Final vector size %d\n",vocabulary->num_elements);
    if(v==CONCAT_VECTORS) printf("-Vector implementation : Concatenated matrices\n");
    else printf("-Vector implementation : Absolute distance between matrices\n");

    if(strcmp(b,"tf-idf")==0) printf("-Vector components: tf-idf values\n");
    else printf("-Vector components: bag of words\n\n");
    printf("#############################\n\n");
    /*###################################################*/

    /* Create cliques and negative relations from the given file */
    printf("#############################\n");

    printf("Creating cliques...\n\n");

    int linesRead = 0;
    int pos_num=0;
    int neg_num=0;
    ht = csvParser(f,&ht, &linesRead,&pos_num,&neg_num);

    int ratio = neg_num/pos_num;
    printf("0 to 1 ratio is %d\n\n",ratio);


    printf("Finished creating cliques!\n\n");
    printf("#############################\n\n");
    /*#####################################################*/


    /* Start the training of the model */

    csvLearning(f,ht,vocabulary,linesRead,b,v,ratio);

    /*#####################################*/
    printf("#############################\n");
    printf("Writing data to disk...\n\n");

    /* Write the statistics of the program and free all allocated memory */
    //If the flag for negative cliques is on write the negative cliques as well in a different file
    if(n == 0) remove("neg_cliques.csv");

    //Save the vocabulary
    writeVocab_secTable(vocabulary);

    printf("\nFiles created successfully!\n\n");
    printf("predictions.csv: file containing the predictions of the model on the test set\n");
    printf("cliques.csv : file containing all pairs\n");
    if(n==1) printf("neg_cliques.csv : file containing all negative relations between cliques\n");
    printf("stats.txt : file containing the statistics of the training procedure\n");
    printf("vocabulary.txt : file containing the vocabulary of the training\n\n");
    printf("#############################\n\n");
    cliqueDeleteHashTable(&ht,BUCKET_HARD_DELETE_MODE);
    destroy_secTable(&vocabulary,ST_HARD_DELETE_MODE);
    free(X_name);
    cleanArgs(f,b);
    return 0;
}