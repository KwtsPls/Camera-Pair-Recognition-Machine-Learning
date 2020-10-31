#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Dictionary.h"

//Function to initialize a dictionary
Dictionary initDictionary(){
    Dictionary *dict = malloc(sizeof(Dictionary));
    return dict;
}


//Function for inserting in dictionary
Dictionary *insertDictionary(Dictionary *dic, char *key, char **val, int num_val)
{
    Dictionary_node *node = malloc(sizeof(Dictionary_node));
    node->key = strdup(key);
    node->value = val;
    node->values_num = num_val;
    node->next = dic->list;
    dic->list = node;
    return dic;
}