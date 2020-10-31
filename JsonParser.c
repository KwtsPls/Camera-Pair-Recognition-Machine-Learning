#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include "JsonParser.h"
#include "ErrorHandler.h"

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
                        strcpy(current,ftr->d_name);

                        //Create the path name
                        cur_path = malloc(strlen(sftr->d_name) + strlen(ftr->d_name)+2);
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
                        }
                        free(current);
                        free(cur_path);
                        closedir(target);
                    }
                }
            }
            closedir(subfolder);
        }
    }

    closedir(folder);
    //Dataset X does not exist in this folder - return NULL
    return NULL;
}

//Function to initialize the data structures with dataset X
int Initialize_dataset_X(char *name){

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

    //Begin iterating through the subdirectories
    while((dptr = readdir(dir))!=NULL){
        if(strcmp(dptr->d_name, ".") && strcmp(dptr->d_name, "..")) {
            //Save the name of the current sub directory
            web_name = malloc(strlen(name)+strlen(dptr->d_name) + 2);
            strcpy(web_name, name);
            strcat(web_name, "/");
            strcat(web_name, dptr->d_name);

            subdir = opendir(web_name);

            //Begin iterating through the files
            while ((subdptr = readdir(subdir)) != NULL) {
                if(strcmp(subdptr->d_name, ".") && strcmp(subdptr->d_name, "..")) {

                    //Get the full path to the json file
                    file_name = malloc(strlen(web_name)+strlen(subdptr->d_name)+2);
                    strcpy(file_name,web_name);
                    strcat(file_name,"/");
                    strcat(file_name,subdptr->d_name);

                    //Get the number from the json files
                    token = strtok(subdptr->d_name, ".");
                    spec_id = malloc(strlen(dptr->d_name) + strlen(token) + 3);

                    //Create the spec_id
                    strcpy(spec_id, dptr->d_name);
                    strcat(spec_id,"//");
                    strcat(spec_id,token);

                    //Read specs from json file
                    scan_json_file(file_name);

                    //DO NOT KEEP
                    //DO NOT KEEP
                    //ONLY FOR TEST DO NOT KEEP

                    free(spec_id);
                    free(file_name);
                }
            }

            free(web_name);
            closedir(subdir);
        }
    }

    closedir(dir);
    return 1;
}


//Function to read data from json files
void scan_json_file(char *name){
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(name, "r");

    if (fp == NULL){
        errorCode=JSON_FILE_UNABLE_TO_OPEN;
        print_error();
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
                array_value[0] = malloc(strlen(value) + 1);
                strcpy(array_value[0], value);

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
                    }
                    //Store the new string in the array
                    array_value[values_num] = malloc(strlen(value) + 1);
                    strcpy(array_value[values_num], value);
                    values_num++;
                }
            }
        }
    }

    fclose(fp);
    if (line)
        free(line);

    return;
}