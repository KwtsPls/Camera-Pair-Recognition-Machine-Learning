#include <stdio.h>
#include <stdlib.h>
#include "JsonParser.h"
#include "ErrorHandler.h"

int errorCode;

int main(){

    char *X_name = get_datasetX_name();
    //Dataset X not found - program will terminate
    if(X_name==NULL) {
        errorCode=DATA_X_NOT_FOUND;
        print_error();
        return 1;
    }

    int check = Initialize_dataset_X(X_name);

    free(X_name);
    return 0;
}