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
    //Check if the key is already in the dictionary
    //and if it exists return the dictionary as is
    int index;
    if(lookUpDictionary(dic,key,&index)!=NULL) {
        for(int i=0;i<num_val;i++) free(val[i]);
        free(val);
        return dic;
    }

    Dictionary_node *node = malloc(sizeof(Dictionary_node));
    node->key = strdup(key);
    node->value = val;
    node->values_num = num_val;
    node->next = dic->list;
    dic->list = node;
    return dic;
}


//function to update the value of a certain key inside the dictionary
int updateDictionary(Dictionary **dict,char *key, char **val, int num_val){

    Dictionary_node *tmp;
    tmp = (*dict)->list;
    while(tmp!=NULL)
    {
        if(strcmp(tmp->key,key)==0)
        {
            for(int i=0;i<tmp->values_num;i++) free(tmp->value[i]);
            free(tmp->value);
            tmp->value=val;
            tmp->values_num=num_val;
            return 1;
        }
        tmp = tmp->next;
    }

    return -1;
}

//Function for printing dictionary
void printDictionary(Dictionary *dic)
{
    Dictionary_node *tmp;
    tmp = dic->list;
    while (tmp != NULL) {
        printf("%s: ", tmp->key);
        for (int i = 0; i < tmp->values_num; i++)
            printf("%s,\n", tmp->value[i]);
        printf("\n\n");
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
            (*num_val) = tmp->values_num;
            return tmp->value;
        }
        tmp = tmp->next;
    }

    *num_val=-1;
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
        for(int i=0;i<tmp->values_num;i++) {
            if(tmp->value!=NULL)
                free(tmp->value[i]);
        }
        if(tmp->value!=NULL)
            free(tmp->value);
        if(tmp!=NULL)
            free(tmp);
        tmp = NULL;
    }
    (*dic)->list = NULL;
    free((*dic)->dict_name);
    free(*dic);
    dic = NULL;
}

//Function to return the number of key/value pairs in a dictionary
int sizeDictionary(Dictionary *dict){
    if(dict->list!=NULL){
        Dictionary_node *cur = dict->list;
        int count=0;
        while(cur!=NULL){
            count++;
            cur=cur->next;
        }
        return count;
    }
    else
        return 0;
}