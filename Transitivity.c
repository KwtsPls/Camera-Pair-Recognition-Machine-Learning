#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <math.h>
#include "Transitivity.h"
#include "Dictionary.h"
#include "ErrorHandler.h"
#include "Bucket.h"
#include "HashTable.h"
#include "SecTable.h"
#include "DataPreprocess.h"
#include "LogisticRegression.h"

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

//Parser for finding pairs of spec_ids in the csv file
HashTable *predictionsParser(char *filename,HashTable **predit_ht,HashTable *ht)
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    size_t read;

    //Open file
    fp = fopen(filename,"r");
    //Check if file Opened
    if(fp==NULL)
    {
        errorCode = OPENING_FILE;
        fclose(fp);
        print_error();
        return NULL;
    }

    int i=0;    //Number Of Lines Counter
    //Read each line
    while((read = getline(&line, &len,fp))!=-1)
    {
        if(i==0) //Skip First Line cause its Left_spec, Right_Spec, label
        {
            i++;
            continue;
        }
        
        char *left_sp,*right_sp,*lbl_str;
        //Take left_spec_id
        left_sp = strtok(line,",");
        //Take right_spec_id
        right_sp = strtok(NULL,",");
        //Take label
        lbl_str = strtok(NULL,",");
        //Label to integer
        double pred = atof(lbl_str);
        int label = round(pred);

        //check the relation the current pair has in the struct
        int relation = checkRelation(*predit_ht,left_sp,right_sp);

        if(relation==-1){
            //They're the same
            if(label == 1) {
                *predit_ht = createCliqueHashTable(predit_ht, left_sp, right_sp);
            }
            //Negative relation
            else if (label == 0) {
                *predit_ht = negativeRelationHashTable((*predit_ht), left_sp, right_sp);
            }
        }
        else if(label!=relation){
            
            //A transitivity issue appeared - check the data to resolve it
            int correct_relation = checkRelation(ht,left_sp,right_sp);
            if(label==1 && correct_relation==1){
                *predit_ht = correctPositiveRelation(predit_ht,left_sp,right_sp);
            }
            else if(label==0 && correct_relation==0){               
                *predit_ht = correctNegativeRelation(predit_ht,ht,left_sp,right_sp);
            }

        }


        i++;    //New line Read
    }

    //Free space and close file
    free(line);
    //Close the file
    fclose(fp);

    //Return num of files read
    return *predit_ht;
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

//Function for resolving transitivity issues with the predicted pairs
void resolve(HashTable **data_ht, HashTable **pred_ht,secTable *preds,double **X_corrected, int *y_corrected,HashTable *ht,secTable *vocabulary,char *bow_type,int vector_type,logisticreg *regressor){

    int index=0;
    for(int i=0;i<preds->numOfBuckets;i++){
        secondaryNode *node = preds->table[i];
        while(node!=NULL){
            for(int j=0;j<node->num_elements;j++){
                predictionPair *pair = node->values[j];
                char *left_sp = pair->left_sp;
                char *right_sp = pair->right_sp;
                double prediction = pair->pred;

                double *l_x = getBagOfWords(ht,vocabulary,left_sp,bow_type);
                double *r_x = getBagOfWords(ht,vocabulary,right_sp,bow_type);
                X_corrected[index] = vectorize(l_x,r_x,regressor->numofN,vector_type);
                index++;

                int relation = checkRelation(*data_ht,left_sp,right_sp);
                //If the current pair exists in the training set resolve the issue based on the known data
                if(relation != -1) {
                    ((predictionPair *)node->values[j])->corrected = relation;
                }
                //The current pair does not exist in the training set
                //if a transitivity issue occurs resolve it based on the predictions 
                else{
                    int pred_relation = checkRelation(*pred_ht,left_sp,right_sp);
                    if(pred_relation==-1){
                        //They're the same
                        if(round(prediction) == 1){
                            *pred_ht = createCliqueHashTable(pred_ht, left_sp, right_sp);
                        }
                        //Negative relation
                        else{
                            *pred_ht = negativeRelationHashTable((*pred_ht), left_sp, right_sp);
                        }
                    }
                    else if(round(prediction)!=pred_relation){
                        
                        if(round(prediction)==0){
                            *pred_ht = resolveNegativeRelation(data_ht,&pred_ht,pair,preds);
                        }

                    }

                }

                free(l_x);
                free(r_x);
            }

            node = node->next;
        }
    }

}

//Function to resolve transitivity issues on the predictions from our model
void resolve_transitivity_issues(char **pairs_train,double **X_train,int *y_train,int train,secTable *preds,HashTable *ht,secTable *vocabulary,char *bow_type,int vector_type, logisticreg *reg){
    char *dirname = get_datasetX_name();
    //Create a hash table corresponding to the training set data
    HashTable *data_ht=initHashTable(TABLE_INIT_SIZE);
    Initialize_dummy_dataset(dirname,&data_ht);
    //Create a hash table to resolve possible conflicts from the prediction data set
    HashTable *pred_ht=initHashTable(TABLE_INIT_SIZE);
    Initialize_dummy_dataset(dirname,&pred_ht);
    //Create the cliques from the training set
    init_train_cliques(&data_ht,&pred_ht,pairs_train,y_train,train);
    
    //Resolve any transitivity issues that occured
    double **X_corrected=malloc(sizeof(double*)*preds->num_elements);
    int *y_corrected = malloc(sizeof(int)*preds->num_elements);
    resolve(&data_ht,&pred_ht,preds,X_corrected,y_corrected,ht,vocabulary,bow_type,vector_type,reg);
    
}