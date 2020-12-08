#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SecTable.h"


// Used Thomas Wang's function for hashing pointers
unsigned int HashPointer(void* Ptr,int buckets)
{
    unsigned int Value = (unsigned int)Ptr;
    Value = ~Value + (Value << 15);
    Value = Value ^ (Value >> 12);
    Value = Value + (Value << 2);
    Value = Value ^ (Value >> 4);
    Value = Value * 2057;
    Value = Value ^ (Value >> 16);
    return Value%buckets;

}


// Used sdbm hash function to hash strings
unsigned int HashString(void* Ptr,int buckets)
{
    unsigned int hash = 0;
    char *str = (char*)Ptr;
    int c;
    while (c = *str++)
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash%buckets;
}

//Returns 1 if pointers are the same
int ComparePointer(void *a, void *b)
{
    return a==b;
}

//Returns 1 if strings are the same
int CompareString(void * a, void *b)
{
    return strcmp((char *)a, (char *)b) == 0;
}

//Function to create a new secondary hash table
secTable *create_secTable(int size, int bucketSize,Hash hashFunction, Compare cmpFuncion){

    secTable *st = malloc(sizeof(secTable));
    st->loadFactor = 0;
    st->num_elements = 0;
    st->bucketSize=bucketSize;
    st->numOfBuckets = size;
    st->hashFunction = hashFunction;
    st->cmpFunction = cmpFuncion;
    st->table = malloc(sizeof(secondaryNode*) * st->numOfBuckets);

    // Initialize every bucket of the table as NULL
    for(int i=0;i<size;i++)
        st->table[i]=NULL;

    return st;

}

//Function to insert a new entry into the 
secTable *insert_secTable(secTable *st, void *value){
    
    int h = st->hashFunction(value, st->numOfBuckets);
    // If the load factor of the hash table is too high reshape the hash table
    if(st->loadFactor<0.85){
        
        if(st->table[h]==NULL){
            st->table[h] = create_secondaryNode(value,(st->bucketSize/sizeof(value)));
            //Update the number of elements
            st->num_elements++;
            // Update load factor
            st->loadFactor = st->num_elements/(st->numOfBuckets*(st->bucketSize/sizeof(value)));
        }
        else{
            int current_num = getNumElements_secondaryNode(st->table[h]);
            //There is enough space in the first block of the bucket insert the new entry
            if(current_num < (st->bucketSize/sizeof(value))){
                st->table[h]->values[current_num] = value;
                st->table[h]->num_elements++;
            }
            else{
                //Na grapseis sxolia
                //Ta sxolia den exoun kapoion na katigorisoun
                //KWSTA
                //Onomata den leme oikogeneies den thigoume
                secondaryNode *new_node = create_secondaryNode(value,st->bucketSize);
                new_node = st->table[h]->next;
                new_node->next=NULL;
                st->table[h] = new_node;
            }
            st->num_elements++;
            st->loadFactor = st->num_elements/(st->numOfBuckets*(st->bucketSize/sizeof(value)));
        }
        return st;
    }
    // Reshape the hash table with doubled size
    else{
        secTable *newst = reshape_secTable(&st);
        newst = insert_secTable(newst, value);
        return newst;
    }
    
}

//Function to replace a value from the hash table with a given new one
secTable *replace_secTable(secTable *st, void *old_value, void *new_value){
    int h = st->hashFunction(old_value, st->numOfBuckets);
    st->table[h] = deletevalue(st->table[h],old_value,st->cmpFunction);
    return insert_secTable(st, new_value);
}

//Function to destroy a hash table
void destroy_secTable(secTable **st){
    for(int i=0;i<(*st)->numOfBuckets;i++){
        if((*st)->table[i]!=NULL)
            destroy_secondaryNode(&((*st)->table[i]));
    }
    free((*st)->table);
    free(*st);
    *st = NULL;
}

//Function for reshaping the hash table
secTable *reshape_secTable(secTable **st){
    secTable *newst = create_secTable((*st)->numOfBuckets*2,(*st)->bucketSize,(*st)->hashFunction,(*st)->cmpFunction);
    for(int i=0;i<(*st)->numOfBuckets;i++){
        while((*st)->table[i]!=NULL){
            void *nvalue;
            (*st)->table[i] = getFirstVal((*st)->table[i],&nvalue);
            if(nvalue!=NULL)
                newst = insert_secTable(newst,nvalue);
        }
    }
    free((*st)->table);
    free(*st);
    return newst;
}

//Function to get the number of items in the first block of a chain
int getNumElements_secondaryNode(secondaryNode *node){
    return node->num_elements;
}

//Function to create a new secondary node
secondaryNode *create_secondaryNode(void *value,int size){
    secondaryNode *sn = malloc(sizeof(sizeof(secondaryNode)));
    sn->num_elements = 1;
    //Nomizw alla den eimai kai sigours
    sn->values = malloc(sizeof(value) * size);
    sn->values[0] = value;

    sn->next = NULL;
    return sn;
}


//Function to destroy a secondary node
void destroy_secondaryNode(secondaryNode **node){
    secondaryNode *next = *node;
    while (next!=NULL){
        secondaryNode *tmp = next;
        next = next->next;
        for(int i=0;i<tmp->num_elements;i++)
            tmp->values[i] = NULL;
        free(tmp->values);
        free(tmp);
    }

    *node = NULL;    
}


//Returns the first Value of the list
secondaryNode *getFirstVal(secondaryNode *node, void **value){
    if(node->num_elements == 0){
        secondaryNode *tmp = node;
        node = node->next;
        tmp->next = NULL;
        *value = NULL;
        free(tmp->values);
        free(tmp);

        return node;
    }
    
    node->num_elements--;
    *value =  node->values[node->num_elements];
    node->values[node->num_elements] = NULL;
    return node;

}

//Function to delete value
secondaryNode *deletevalue(secondaryNode *node, void *value, Compare fun){
    node->num_elements--;
    void *tmp_value = node->values[node->num_elements];
    node->values[node->num_elements] = NULL;
    if(fun(value,tmp_value) == 1){
        return node;
    }
    secondaryNode *ptr = node;
    while (ptr!=NULL){
        for(int i=0;i<ptr->num_elements;i++){
            if(fun(value,ptr->values[i]) == 1){
                ptr->values[i] = tmp_value;
                return node;
            }
        }
        ptr = ptr->next;
    }
    
}