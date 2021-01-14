#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <math.h>
#include "Dictionary.h"
#include "ErrorHandler.h"
#include "Bucket.h"
#include "HashTable.h"
#include "SecTable.h"
#include "DataPreprocess.h"

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

//Function to resolve transitivity issues on the predictions from our model
void resolve_transitivity_issues(char *filename,char *dirname,HashTable *ht){

    //Initialize a new hash table for the predicted dataset
    HashTable *predict_ht = initHashTable(TABLE_INIT_SIZE);
    
    //Initialize the hashtable for the predictions
    if(Initialize_dummy_dataset(dirname,&predict_ht)==-1)
        return;

    predict_ht = predictionsParser(filename,&predict_ht,ht);

    cliqueDeleteHashTable(&predict_ht,BUCKET_HARD_DELETE_MODE);
}