#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Dictionary.h"

//Function to initialize a dictionary
Dictionary *initDictionary(char *dict_name){
    Dictionary *dict = malloc(sizeof(Dictionary));
    dict->dict_name = strdup(dict_name);
    dict->list = NULL;
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


//Function for printing dictionary
void printDictionary(Dictionary *dic)
{
    Dictionary_node *tmp;
    printf("Kati den doulevei?\n");
    tmp = dic->list;
    while(tmp!=NULL)
    {
        printf("KEY:%s\n", tmp->key);
        for(int i=0;i<tmp->values_num;i++)
            printf("Value %d: %s\n",i,tmp->value[i]);
        tmp = tmp->next;
    }
    
}


//Give Key in the Dictionary to get the values
char **lookUpDictionary(Dictionary *dic, char *key, int *num_val)
{
    Dictionary_node *tmp;
    tmp = dic->list;
    while(tmp!=NULL)
    {
        if(strcmp(tmp->key,key)==0)
        {
            printf("TOVRIKA\n");
            (*num_val) = tmp->values_num;
            return tmp->value;
        }
        tmp = tmp->next;
    }
    return NULL;
}

//Function for Deleting Dictionaries
void deleteDictionary(Dictionary **dic)
{   
    Dictionary_node *tmp, *next;
    next = (*dic)->list;
    while(next != NULL)
    {
        tmp = next;
        next = next->next;
        tmp -> next = NULL;
        free(tmp->key);
        for(int i=0;i<tmp->values_num;i++)
            free(tmp->value[i]);
        free(tmp->value);
        free(tmp);
        tmp = NULL;
    }
    (*dic)->list = NULL;
    free((*dic)->dict_name);
    free(*dic);
    dic = NULL;
}