#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Dictionary.h"

//Function to initialize a dictionary
Dictionary *initDictionary(char *dict_name){
    //Create space for the Dictionary
    Dictionary *dict = malloc(sizeof(Dictionary));
    if(dict == NULL)
    {
        errorCode = MALLOC_FAILURE;
        print_error();
        return NULL;
    }
    //Copy the name of the dictionary
    dict->dict_name = strdup(dict_name);
    //Malloc failed in strdup
    if(dict->dict_name == NULL)
    {
        errorCode = MALLOC_FAILURE;
        free(dict);
        print_error();
        return NULL;
    }
    //Do not create the list yet
    dict->list = NULL;
    //Return the new dictionary
    return dict;
}


//Function for inserting in dictionary
Dictionary *insertDictionary(Dictionary *dic, char *key, char **val, int num_val)
{
    //Check if the key is already in the dictionary
    //and if it exists return the dictionary as is
    int index;
    if(lookUpDictionary(dic,key,&index)!=NULL) {
        //Free the space of the val array
        for(int i=0;i<num_val;i++) free(val[i]);
        free(val);
        //Return dictionary as it is
        return dic;
    }
    //Create a new dictionary node
    Dictionary_node *node = malloc(sizeof(Dictionary_node));
    if(node == NULL)
    {
        errorCode = MALLOC_FAILURE;
        print_error();
        return dic;
    }
    //Store the key of the node
    node->key = strdup(key);
    //Malloc failed in strdup
    if(node->key == NULL)
    {
        errorCode = MALLOC_FAILURE;
        print_error();
        free(node);
        return dic;
    }
    //Make node value array to point to the val array
    node->value = val;
    //Store the number of values
    node->values_num = num_val;
    //Add the node to the beginning of the list
    node->next = dic->list;
    dic->list = node;
    //Return dictionary
    return dic;
}


//function to update the value of a certain key inside the dictionary
int updateDictionary(Dictionary **dict,char *key, char **val, int num_val){
    //Iterate through the dictionary list
    Dictionary_node *tmp;
    tmp = (*dict)->list;
    while(tmp!=NULL)
    {
        //Is this the key?
        if(strcmp(tmp->key,key)==0)
        {
            //Free the space of the old values
            for(int i=0;i<tmp->values_num;i++) free(tmp->value[i]);
            free(tmp->value);
            //Store the new values to the node
            tmp->value=val;
            tmp->values_num=num_val;
            //Return that everything went fine
            return 1;
        }
        tmp = tmp->next;
    }
    //The key was not found
    return -1;
}

//Function for printing dictionary
void printDictionary(Dictionary *dic)
{
    //Iterate through the dictionary list
    Dictionary_node *tmp;
    tmp = dic->list;
    while (tmp != NULL) {
        //Print the key of the node
        printf("%s: ", tmp->key);
        //Iterate through the values of the node
        for (int i = 0; i < tmp->values_num; i++)
            //Print them
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
    //Iterate through the list of the dictionary and search for the key
    while(tmp!=NULL)
    {
        if(strcmp(tmp->key,key)==0)
        {
            //Found the key
            (*num_val) = tmp->values_num;
            //Return the array of values
            return tmp->value;
        }
        tmp = tmp->next;
    }

    //Nothing was found
    *num_val=-1;
    return NULL;
}

//Function for Deleting Dictionaries
void deleteDictionary(Dictionary **dic)
{   
    //Time to delete the dictionary
    Dictionary_node *tmp, *next;
    next = (*dic)->list;
    //Iterate through the dictionary list
    while(next != NULL)
    {
        tmp = next;
        next = next->next;
        tmp -> next = NULL;
        //Free the space of the key string
        free(tmp->key);
        for(int i=0;i<tmp->values_num;i++) {
            if(tmp->value!=NULL)
                //Free the space of the values string
                free(tmp->value[i]);
        }
        if(tmp->value!=NULL)
            //Free the space of the value array
            free(tmp->value);
        if(tmp!=NULL)   //Free the space of the dictionary node
            free(tmp);
        tmp = NULL;
    }
    (*dic)->list = NULL;
    //Free the string of the dictionary name
    free((*dic)->dict_name);
    //Free the space of the dictionary 
    free(*dic);
    dic = NULL;
}

//Function to return the number of key/value pairs in a dictionary
int sizeDictionary(Dictionary *dict){
    if(dict->list!=NULL){
        //Iterate through the list as it exists
        Dictionary_node *cur = dict->list;
        int count=0;
        //Count each node
        while(cur!=NULL){
            count++;
            cur=cur->next;
        }
        //Return the number of nodes
        return count;
    }
    else
        //List doesn't exist....
        return 0;
}

//Function to concatenate all strings into a single node in the dictionary
Dictionary *concatenateAllDictionary(Dictionary *dict){

    char *final_value=NULL;

    //Iterate through every key value pair and concatenate them into a single string
    Dictionary_node *cur = dict->list;
    while(cur!=NULL){

        //Copy every value of the current key into final_value
        for(int i=0;i<cur->values_num;i++){
            if(final_value==NULL){
                final_value = malloc(strlen(cur->value[i])+1);
                strcpy(final_value,cur->value[i]);
            }
            else{
                final_value = realloc(final_value,strlen(final_value)+1+strlen(cur->value[i])+1);
                strcat(final_value," ");
                strcat(final_value,cur->value[i]);
            }
        }

        Dictionary_node *temp = cur;
        cur = cur->next;

        for(int i=0;i<temp->values_num;i++)
            free(temp->value[i]);
        free(temp->value);
        free(temp->key);
        free(temp);
        temp=NULL;
    }
    dict->list=NULL;

    char **new_array = malloc(sizeof(char*));
    new_array[0] = final_value;
    dict = insertDictionary(dict,"BOW",new_array,1);

    return dict;
}