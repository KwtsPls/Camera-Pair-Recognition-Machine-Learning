#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include "Dictionary.h"
#include "JsonParser.h"
#include "ErrorHandler.h"
#include "Bucket.h"
#include "HashTable.h"
#include "SecTable.h"
#include "DataPreprocess.h"

int errorCode;

//Function to find the name of the directory holding the dataset X
char *get_datasetX_name(){

    //Open the current directory to search for the dataset
    DIR *folder;
    struct dirent *ftr = NULL;
    DIR *subfolder;
    struct dirent *sftr = NULL;
    DIR *target;
    struct dirent *tgtr = NULL;
    char *current=NULL;
    char *cur_path=NULL;
    folder = opendir(".");

    //Check if the folder opened with no mistakes
    if(folder==NULL){
        errorCode=DIR_UNABLE_TO_OPEN;
        print_error();
        return NULL;
    }


    while((ftr = readdir(folder))!=NULL){
        //Start searching if current folder is the one containing the dataset
        if(strcmp(ftr->d_name, ".") && strcmp(ftr->d_name, "..")){

            subfolder = opendir(ftr->d_name);
            //Current item is a directory check if is the one with the dataset
            if(subfolder!=NULL){

                //Search every subfolder
                while((sftr = readdir(subfolder))!=NULL){
                    if(strcmp(sftr->d_name, ".") && strcmp(sftr->d_name, "..")){
                        //Get the name of the current directory
                        current = malloc(strlen(ftr->d_name)+1);
                        if(current == NULL)
                        {   
                            //Something went wrong with Malloc
                            errorCode = MALLOC_FAILURE;
                            print_error();
                            closedir(subfolder);
                            return NULL;
                        }
                        strcpy(current,ftr->d_name);

                        //Create the path name
                        cur_path = malloc(strlen(sftr->d_name) + strlen(ftr->d_name)+2);
                        if(cur_path == NULL)
                        {
                            //Something went wrong with Malloc
                            errorCode = MALLOC_FAILURE;
                            print_error();
                            closedir(subfolder);
                            free(current);
                            return NULL;
                        }
                        strcpy(cur_path,ftr->d_name);
                        strcat(cur_path,"/");
                        strcat(cur_path,sftr->d_name);

                        target = opendir(cur_path);

                        //Search for files in a xxxxx.json format
                        //if such a file is found return the name of the
                        // directory holding the current subdirectory
                        if(target!=NULL){
                            while((tgtr = readdir(target))!=NULL){

                                //Get the last five chars of the current file
                                //to check if it is in a json format
                                int len = strlen(tgtr->d_name);
                                if(len>6) {
                                    const char *last_five = &tgtr->d_name[len - 5];

                                    //json file found return the name of the directory
                                    if (!strcmp(last_five, ".json")) {
                                        closedir(folder);
                                        closedir(subfolder);
                                        closedir(target);
                                        free(cur_path);
                                        return current;
                                    }
                                }
                            }
                            closedir(target);
                        }
                        //Number 20 is the number if the file is not a directory
                        else if(errno != 20 && target == NULL)
                        {
                            //Could not open target file
                            free(current);
                            free(cur_path);
                            closedir(subfolder);
                            errorCode = DIR_UNABLE_TO_OPEN;
                            print_error();
                            return NULL;
                        }
                        free(current);
                        free(cur_path);
                    }
                }
                closedir(subfolder);
            }
            //Number 20 is the number if the file is not a directory
            //There is no error here cause in the subfolder it can be junk files
            //That are not subfolders....
            else if(errno != 20 && subfolder == NULL)
            {
                errorCode=DIR_UNABLE_TO_OPEN;
                print_error();
            }
        }
    }

    closedir(folder);
    //Dataset X does not exist in this folder - return NULL
    return NULL;
}

//Function to initialize the data structures with dataset X
int Initialize_dataset_X(char *name,HashTable **ht,secTable **vocabulary){

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

            printf("%s\n",web_name);

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

                    //Read specs from json file
                    Dictionary *dict = parse_json_file(file_name,spec_id,stopwords,vocabulary);
                    //Concatenate all key-values to one sentence and insert the dictionary into the hashtable
                    dict = concatenateAllDictionary(dict);
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


//Function to read data from json files
Dictionary *parse_json_file(char *name,char* spec_id,secTable *stopwords,secTable **vocabulary){
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    //Create a new dictionary for the current file
    Dictionary *dict = initDictionary(spec_id);
    //Create a hashtable to save all unique words
    //of the current file to help in the calculation of tf-idf
    secTable *unique_words = create_secTable(ST_INIT_SIZE,SB_SIZE,HashString,CompareString,DeleteString,String);
    //Length of the current json file
    int text_len=0;

    fp = fopen(name, "r");

    if (fp == NULL){
        errorCode=JSON_FILE_UNABLE_TO_OPEN;
        print_error();
        return NULL;
    }

    //String for strtok
    char *token=NULL;
    char *key=NULL;
    char *value=NULL;

    //Read the specs from the json file
    while ((read = getline(&line, &len, fp)) != -1) {
        //Remove the change line character
        token = strtok(line, "\n");

        //Break the string into two parts. The key and the value
        //Left of the : character is the key and right of it the value
        if(strcmp(token,"{")!=0 && strcmp(token,"}")!=0) {
            key = strtok(token,":");
            value = strtok(NULL,"");

            //Start parsing the key
            key = strtok(key,"\"");
            key = strtok(NULL,"\"");

            //Start parsing the value
            //If the value part is not an array simply remove the needless characters
            if(strcmp(value," [")!=0) {
                value = strtok(value, "\"");
                value = strtok(NULL, "");
                //Remove the two last characters from the string
                if(value[strlen(value) - 1]==',')
                    value[strlen(value) - 2] = 0;
                else
                    value[strlen(value) - 1] = 0;

                char **array_value=NULL;
                array_value = malloc(sizeof(char*));
                if(array_value == NULL){
                    //MALLOC FAILED
                    errorCode = MALLOC_FAILURE;
                    print_error();
                    fclose(fp);
                    return NULL;
                }
                value = preprocess(value,stopwords,vocabulary,&unique_words,&text_len);
                array_value[0] = malloc(strlen(value) + 1);
                if(array_value[0] == NULL){
                    //MALLOC FAILED
                    errorCode = MALLOC_FAILURE;
                    print_error();
                    fclose(fp);
                    free(array_value);
                    return NULL;
                }
                strcpy(array_value[0], value);
                free(value);
                //Check the value of key, that was parsed
                if(key==NULL) {
                    dict = insertDictionary(dict,"Non-defined",array_value,1);
                }
                else
                    dict = insertDictionary(dict,key,array_value,1);

            }
            //The value is an array iterate to get every element of the current array
            else{
                char * array_line = NULL;
                size_t len = 0;
                ssize_t read;
                char *value=NULL;
                int values_num=0;

                //Initialize array of strings
                char **array_value=NULL;
                array_value = malloc(sizeof(char*));
                if(array_value == NULL){
                    //MallocFailure
                    errorCode = MALLOC_FAILURE;
                    print_error();
                    fclose(fp);
                    return NULL;
                }

                while ((read = getline(&array_line, &len, fp)) != -1) {
                    //Remove the change line character
                    token = strtok(array_line, "\n");

                    //end of array reached stop the iteration
                    if(strcmp(token,"    ],")==0 || strcmp(token,"    ]")==0 )
                        break;

                    //increase the number of items

                    token = strtok(token,"\"");
                    value = strtok(NULL,"");
                    //Remove the two last characters from the string
                    if(value[strlen(value) - 1]==',')
                        value[strlen(value) - 2] = 0;
                    else
                        value[strlen(value) - 1] = 0;

                    //Reallocate space in array for new string
                    if(values_num!=0) {
                        array_value = realloc(array_value,sizeof(char*)*(values_num+1));
                        if(array_value == NULL){
                            //MallocFailure
                            errorCode = MALLOC_FAILURE;
                            print_error();
                            fclose(fp);
                            return NULL;
                        }
                    }
                    //Store the new string in the array
                    value = preprocess(value,stopwords,vocabulary,&unique_words,&text_len);
                    array_value[values_num] = malloc(strlen(value) + 1);
                    if(array_value[values_num] == NULL){
                        //MALLOC FAILURE
                        for(int i=0;i<values_num;i++)
                            free(array_value[i]);
                        free(array_value);
                        fclose(fp);
                        errorCode = MALLOC_FAILURE;
                        print_error();
                        return NULL;
                    }
                    strcpy(array_value[values_num], value);
                    free(value);
                    values_num++;
                }

                if(key==NULL) {
                    dict = insertDictionary(dict,"Non-defined",array_value,values_num);
                }
                else
                    dict = insertDictionary(dict,key,array_value,values_num);

                if (array_line)
                    free(array_line);
            }
        }
    }

    if(vocabulary!=NULL)
        *vocabulary = update_tf_idf_values(*vocabulary,unique_words,text_len);

    fclose(fp);
    if (line)
        free(line);
    destroy_secTable(&unique_words,ST_HARD_DELETE_MODE);

    return dict;
}