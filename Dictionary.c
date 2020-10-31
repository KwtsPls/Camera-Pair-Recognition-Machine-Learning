#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Dictionary.h"

//Function to initialize a dictionary
Dictionary initDictionary(){
    Dictionary *dict = malloc(sizeof(Dictionary));
    return dict;
}
