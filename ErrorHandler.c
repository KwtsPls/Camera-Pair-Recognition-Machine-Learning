#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ErrorHandler.h"
#include "LogisticRegression.h"

int errorCode;

//Function to print current error
void print_error(){
    if(errorCode==DIR_UNABLE_TO_OPEN) perror("Unable to read directory...\n");
    else if(errorCode==FLAG_REPEATED_ERROR) printf("Input Error! Please do not repeat already given flags...\n");
    else if(errorCode==ARGS_ERROR) printf("Input Error! Invalid arguments were given, please use the correct format...\n");
    else if(errorCode==FLAG_ERROR) printf("Input Error! Please use :\n-f for the labelled dataset's name.\n-n on for the negative relations to be saved OR -n off for to not save them.\n"
               "-v abs for absolute distance between vectors OR -v concat for concatenated vectors.\n-b bow for bag-of-words implementation OR -b tf-idf for tf-idf implementation.\n");
    else if(errorCode==DATA_X_NOT_FOUND) perror("Dataset X not found within program files... \nPlease insert the database in the project's folder.\n");
    else if(errorCode==JSON_FILE_UNABLE_TO_OPEN) perror("Could not open .json file...\n");
    else if(errorCode==MALLOC_FAILURE) perror("Something went wrong with memory allocation\n");
    else if(errorCode==OPENING_FILE) perror("Something went wrong while opening the file...\n");
    else if(errorCode==WRITING_TO_FILE) perror("Something went wrong while writing to file...\n");
}

//Function to check whether the input was incorrect
int argsCheck(int argNum)
{
    //Wrong number of arguments
    if(argNum!=11) {
        errorCode=ARGS_ERROR;
        print_error();
        return 0;
    }

    return 1;
}

//Function to use the values of the arguments given by the command line
int initArgs(int argc,char *argv[],char **f,int *n,int *v,int *max_features,char **b)
{
    int ff=0,fn=0,fv=0,fb=0,fm=0;
    for(int i=1;i<argc;i++)
    {
        if(i%2==1)
        {
            if (strcmp(argv[i], "-f") == 0)
            {
                //check if the argument was already given
                if(ff==0)
                {
                    //Get the name of the file containing the labelled dataset
                    *f = (char *) malloc(strlen(argv[i + 1]) + 1);
                    strcpy(*f, argv[i + 1]);
                    ff=1;
                }
                else
                {
                    errorCode=FLAG_REPEATED_ERROR;
                    print_error();
                    return 0;
                }
            }
            else if (strcmp(argv[i], "-n") == 0)
            {
                //check if the argument was already given
                if(fn==0)
                {
                    fn = 1;
                    //Get the flag for the printing of the negative relations
                    if(strcmp(argv[i + 1],"off")==0){
                        *n=0;
                    }
                    else if(strcmp(argv[i + 1],"on")==0){
                        *n=1;
                    }
                    else{
                        errorCode=FLAG_ERROR;
                        print_error();
                        return 0;
                    }
                }
                else
                {
                    errorCode=FLAG_REPEATED_ERROR;
                    print_error();
                    return 0;
                }
            }
            else if (strcmp(argv[i], "-m") == 0)
            {
                //check if the argument was already given
                if(fm==0)
                {
                    fm = 1;
                    *max_features = atoi(argv[i+1]);
                }
                else
                {
                    errorCode=FLAG_REPEATED_ERROR;
                    print_error();
                    return 0;
                }
            }
            else if (strcmp(argv[i], "-v") == 0)
            {
                //check if the argument was already given
                if(fv==0)
                {
                    fv = 1;
                    if(strcmp(argv[i + 1],"abs")==0){
                        *v =ABSOLUTE_DISTANCE;
                    }
                    else if(strcmp(argv[i + 1],"concat")==0){
                        *v=CONCAT_VECTORS;
                    }
                    else{
                        errorCode=FLAG_ERROR;
                        print_error();
                        return 0;
                    }
                }
                else
                {
                    errorCode=FLAG_REPEATED_ERROR;
                    print_error();
                    return 0;
                }
            }
            else if (strcmp(argv[i], "-b") == 0)
            {
                //check if the argument was already given
                if(fb==0)
                {
                    if(strcmp(argv[i + 1],"bow")==0 || strcmp(argv[i + 1],"tf-idf")==0) {
                        //Get the type of values the vectors can have tf-idf/bow
                        *b = (char *) malloc(strlen(argv[i + 1]) + 1);
                        strcpy(*b, argv[i + 1]);
                        fb = 1;
                    }
                    else{
                        errorCode=FLAG_ERROR;
                        print_error();
                        return 0;
                    }
                }
                else
                {
                    errorCode=FLAG_REPEATED_ERROR;
                    print_error();
                    return 0;
                }
            }
            //Wrong flag was given as argument
            else{
                errorCode=FLAG_ERROR;
                print_error();
                return 0;
            }
        }
    }
    return 1;
}

//Function to delete the given arguments for the command line
void cleanArgs(char *f,char *b){
    if(f!=NULL)
        free(f);
    if(b!=NULL)
        free(b);
}