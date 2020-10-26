#include <stdio.h>
#include "ErrorHandler.h"

int errorCode;

//Function to print current error
void print_error(){
    if(errorCode==DIR_UNABLE_TO_OPEN) printf("Unable to read directory.\n");
    else if(errorCode==DATA_X_NOT_FOUND) printf("Dataset X not found within program files... \nPlease insert the database in the project's folder.\n");
}