#ifndef PROJECT_ERRORHANDLER_H
#define PROJECT_ERRORHANDLER_H
#include <errno.h>

extern int errorCode;

//Error codes
typedef enum{

    DIR_UNABLE_TO_OPEN=0,
    DATA_X_NOT_FOUND=1,
    JSON_FILE_UNABLE_TO_OPEN=2,
    MALLOC_FAILURE = 3,
    OPENING_FILE = 4,
    WRITING_TO_FILE = 5,
    ARGS_ERROR=6,
    FLAG_REPEATED_ERROR=7,
    FLAG_ERROR=8

}ERROR_CODES;

//Function to print current error
void print_error();

//Function to check whether the input was incorrect
int argsCheck(int argNum);

//Function to use the values of the arguments given by the command line
int initArgs(int argc,char *argv[],char **f,int *n,char **v,char **b);

//Function to delete the given arguments for the command line
void cleanArgs(char *f,char *v,char *b);

#endif //PROJECT_ERRORHANDLER_H
