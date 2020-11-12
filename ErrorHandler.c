#include <stdio.h>
#include "ErrorHandler.h"

int errorCode;

//Function to print current error
void print_error(){
    if(errorCode==DIR_UNABLE_TO_OPEN) perror("Unable to read directory...\n");
    else if(errorCode==DATA_X_NOT_FOUND) perror("Dataset X not found within program files... \nPlease insert the database in the project's folder.\n");
    else if(errorCode==JSON_FILE_UNABLE_TO_OPEN) perror("Could not open .json file...\n");
    else if(errorCode==MALLOC_FAILURE) perror("Something went wrong with memory allocation\n");
    else if(errorCode==OPENING_FILE) perror("Something went wrong while openening the file...\n");
    else if(errorCode==WRITING_TO_FILE) perror("Something went wrong while writing to file...\n");
    
    
}