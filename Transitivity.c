#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <math.h>
#include "Transitivity.h"
#include "JsonParser.h"
#include "Dictionary.h"
#include "ErrorHandler.h"
#include "Bucket.h"
#include "HashTable.h"
#include "SecTable.h"
#include "DataPreprocess.h"
#include "LogisticRegression.h"
#include "RBtree.h"
#include "BagOfWords.h"
#include "sparseVector.h"

//Function to initialize a predictionPair
predictionPair *initPredictionPair(char *left_sp,char *right_sp,double pred){
    predictionPair *pair = malloc(sizeof(predictionPair));
    pair->left_sp = strdup(left_sp);
    pair->right_sp = strdup(right_sp);
    pair->pred = pred;
    pair->corrected = -1;

    return pair;
}

//Function to delete a predtictionPair
void deletePredictionPair(predictionPair *pair){
    free(pair->left_sp);
    free(pair->right_sp);
    free(pair);
}

//Function to initialize the data structures with a dummy dataset
int Initialize_dummy_dataset(char *name,HashTable **ht){

    DIR *dir;
    struct dirent *dptr = NULL;
    DIR *subdir;
    struct dirent *subdptr=NULL;

    char *web_name=NULL;
    char *file_name=NULL;
    char *spec_id=NULL;
    //token for strtok function
    char *token=NULL;

    dir = opendir(name);
    if(dir==NULL){
        errorCode=DIR_UNABLE_TO_OPEN;
        print_error();
        return -1;
    }

    //Initialize stopwords
    secTable *stopwords = init_stopwords();

    //Begin iterating through the subdirectories
    while((dptr = readdir(dir))!=NULL){
        if(strcmp(dptr->d_name, ".") && strcmp(dptr->d_name, "..")) {
            //Save the name of the current sub directory
            web_name = malloc(strlen(name)+strlen(dptr->d_name) + 2);
            if(web_name == NULL)
            {
                //Malloc Failed
                closedir(dir);
                errorCode = MALLOC_FAILURE;
                print_error();
                return -1;

            }
            strcpy(web_name, name);
            strcat(web_name, "/");
            strcat(web_name, dptr->d_name);

            subdir = opendir(web_name);
            if(subdir == NULL)
            {
                //Something went wrong while opening the subdirectory
                errorCode = DIR_UNABLE_TO_OPEN;
                print_error();
                closedir(dir);
                free(web_name);
                return -1;
            }

            //Begin iterating through the files
            while ((subdptr = readdir(subdir)) != NULL) {
                if(strcmp(subdptr->d_name, ".") && strcmp(subdptr->d_name, "..")) {

                    //Get the full path to the json file
                    file_name = malloc(strlen(web_name)+strlen(subdptr->d_name)+2);
                    if(file_name == NULL)
                    {
                        //MALLOC FAILED
                        free(web_name);
                        closedir(dir);
                        closedir(subdir);
                        errorCode = MALLOC_FAILURE;
                        print_error();
                        return -1;
                    }
                    strcpy(file_name,web_name);
                    strcat(file_name,"/");
                    strcat(file_name,subdptr->d_name);

                    //Get the number from the json files
                    token = strtok(subdptr->d_name, ".");
                    spec_id = malloc(strlen(dptr->d_name) + strlen(token) + 3);
                    if(spec_id == NULL)
                    {
                        //MALLOC FAILED
                        free(web_name);
                        free(file_name);
                        closedir(dir);
                        closedir(subdir);
                        errorCode = MALLOC_FAILURE;
                        print_error();
                        return -1;
                    }

                    //Create the spec_id
                    strcpy(spec_id, dptr->d_name);
                    strcat(spec_id,"//");
                    strcat(spec_id,token);

                    //Create an empty dict as a dummy input with the name of the current spec
                    Dictionary *dict = initDictionary(spec_id);
                    *ht = insertHashTable(ht,dict);


                    free(spec_id);
                    free(file_name);
                }
            }

            free(web_name);
            closedir(subdir);
        }
    }

    //delete the table used for the stopwords
    destroy_secTable(&stopwords,ST_SOFT_DELETE_MODE);

    closedir(dir);
    return 1;
}


//Function to create cliques and negative relations in the new dummy hash tables
void init_train_cliques(HashTable **data_ht,HashTable **pred_ht,char **pairs_train,int *y_train,int size){

    //For every pair in the training set
    for(int i=0;i<size;i++){
        char *line = strdup(pairs_train[i]);
        char *left_sp,*right_sp;
        //Take left_spec_id
        left_sp = strtok(line,",");
        //Take right_spec_id
        right_sp = strtok(NULL,",");

        if(y_train[i]==1){
            *data_ht = createCliqueHashTable(data_ht,left_sp,right_sp);
            *pred_ht = createCliqueHashTable(pred_ht,left_sp,right_sp);
        }
        else{
            *data_ht = negativeRelationHashTable(*data_ht,left_sp,right_sp);
            *pred_ht = negativeRelationHashTable(*pred_ht,left_sp,right_sp);
        }

        free(line);
    }

}

//Function to perform a recursive search in the tree struct that contains the predicted pairs
void resolveRB(HashTable **data_ht, HashTable **pred_ht,RBtree *preds,char ***pairs_corrected,sparseVector ***X_corrected, int **y_corrected,HashTable *ht,secTable *vocabulary,char *bow_type,int vector_type,logisticreg *regressor){
    int i=0;
    resolve(data_ht,pred_ht,preds->root,pairs_corrected,X_corrected,y_corrected,ht,vocabulary,bow_type,vector_type,regressor,&i);
}

//Function for resolving transitivity issues with the predicted pairs
void resolve(HashTable **data_ht, HashTable **pred_ht,RBnode *node,char ***pairs_corrected,sparseVector ***X_corrected, int **y_corrected,HashTable *ht,secTable *vocabulary,char *bow_type,int vector_type,logisticreg *regressor,int *index){

    if(node==NULL)
        return;

    resolve(data_ht,pred_ht,node->right,pairs_corrected,X_corrected,y_corrected,ht,vocabulary,bow_type,vector_type,regressor,index);

    LinkedList *l = node->l;
    ListNode *listnode = l->head;

    while(listnode!=NULL) {

        predictionPair *cur = listnode->entry;

        //Save the current line
        char *line = malloc(strlen(cur->left_sp) + 1 + strlen(cur->right_sp) + 1);
        strcpy(line, cur->left_sp);
        strcat(line, ",");
        strcat(line, cur->right_sp);
        (*pairs_corrected)[*index] = line;


        double *x_l = getBagOfWords(ht, vocabulary, cur->left_sp, bow_type);
        double *x_r = getBagOfWords(ht, vocabulary, cur->right_sp, bow_type);
        int sparse_size=0;
        double *x = vectorize(x_l, x_r, regressor->numofN, vector_type,&sparse_size);
        (*X_corrected)[*index] = init_sparseVector(x,regressor->numofN,sparse_size);


        //Save the vector of the current pair
        int relation = checkRelation(*data_ht, cur->left_sp, cur->right_sp);
        //Resolve any issues that occur
        if (relation != -1) {
            (*y_corrected)[*index] = relation;
        } else {
            int pred_relation = checkRelation(*pred_ht, cur->left_sp, cur->right_sp);
            if (pred_relation == -1) {
                (*y_corrected)[*index] = round(cur->pred);
                if (round(cur->pred) == 1)
                    *pred_ht = createCliqueHashTable(pred_ht, cur->left_sp, cur->right_sp);
                else
                    *pred_ht = negativeRelationHashTable(*pred_ht, cur->left_sp, cur->right_sp);
            } else {
                //Because predictions are sorted, we have taken the best predictions already in the hash table
                //So we assume that the relations causing the conflicts are weaker predictions and shouldn't be taken into consideration
                (*y_corrected)[*index] = pred_relation;
            }

        }

        (*index)++;
        free(x_l);
        free(x_r);
        listnode = listnode->next;
    }

    resolve(data_ht,pred_ht,node->left,pairs_corrected,X_corrected,y_corrected,ht,vocabulary,bow_type,vector_type,regressor,index);

}

//Function to resolve transitivity issues on the predictions from our model
int resolve_transitivity_issues(char ***pairs_train,sparseVector ***X_train,int **y_train,int train,RBtree *preds,HashTable *ht,secTable *vocabulary,char *bow_type,int vector_type, logisticreg *reg){
    char *dirname = get_datasetX_name();
    //Create a hash table corresponding to the training set data
    HashTable *data_ht=initHashTable(TABLE_INIT_SIZE);
    Initialize_dummy_dataset(dirname,&data_ht);
    //Create a hash table to resolve possible conflicts from the prediction data set
    HashTable *pred_ht=initHashTable(TABLE_INIT_SIZE);
    Initialize_dummy_dataset(dirname,&pred_ht);
    //Create the cliques from the training set
    init_train_cliques(&data_ht,&pred_ht,*pairs_train,*y_train,train);
    
    //Resolve any transitivity issues that occurred
    int size = preds->num_elements;
    char **pairs_corrected=malloc(sizeof(char*)*size);
    sparseVector **X_corrected=malloc(sizeof(sparseVector*)*size);
    int *y_corrected = malloc(sizeof(int)*size);
    resolveRB(&data_ht,&pred_ht,preds,&pairs_corrected,&X_corrected,&y_corrected,ht,vocabulary,bow_type,vector_type,reg);
    
    //Concatenate the old training set with the new pairs
    int new_size = train + size;
    sparseVector **new_X_train = malloc(sizeof(sparseVector*)*new_size);
    int *new_y_train = malloc(sizeof(int)*new_size);
    char **new_pairs_train = malloc(sizeof(char*)*new_size);

    for(int i=0;i<train;i++){
        new_X_train[i] = (*X_train)[i];
        new_y_train[i] = (*y_train)[i];
        new_pairs_train[i] = (*pairs_train)[i]; 
    }

    for(int i=0;i<size;i++){
        new_X_train[i+train] = X_corrected[i];
        new_y_train[i+train] = y_corrected[i];
        new_pairs_train[i+train] = pairs_corrected[i];
    }

    free(*X_train);
    *X_train = new_X_train;
    free(*y_train);
    *y_train = new_y_train;
    free(*pairs_train);
    *pairs_train = new_pairs_train;
    free(pairs_corrected);
    free(X_corrected);
    free(y_corrected);
    cliqueDeleteHashTable(&data_ht,BUCKET_HARD_DELETE_MODE);
    cliqueDeleteHashTable(&pred_ht,BUCKET_HARD_DELETE_MODE);
    destroyRB(preds);
    free(dirname);

    return new_size;
}