#ifndef PROJECT_ERRORHANDLER_H
#define PROJECT_ERRORHANDLER_H
#include <errno.h>

extern int errorCode;

//Error codes
typedef enum
{
    DIR_UNABLE_TO_OPEN=0,
    DATA_X_NOT_FOUND=1,
    JSON_FILE_UNABLE_TO_OPEN=2,
    MALLOC_FAILURE = 3,
    OPENING_FILE = 4,
    WRITING_TO_FILE = 5

}ERROR_CODES;

//Function to print current error
void print_error();

#endif //PROJECT_ERRORHANDLER_H
