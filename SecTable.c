#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SecTable.h"


// Used Thomas Wang's function for hashing pointers
unsigned int HashPointer(void* Ptr,int buckets)
{
    long Value = (long)Ptr;
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
    return (strcmp((char *)a, (char *)b) == 0);
}

//Function to create a new secondary hash table
secTable *create_secTable(int size, int bucketSize,Hash hashFunction, Compare cmpFuncion,Data type){

    secTable *st = malloc(sizeof(secTable));
    st->loadFactor = 0;
    st->num_elements = 0;
    st->bucketSize=bucketSize;
    st->numOfBuckets = size;
    st->hashFunction = hashFunction;
    st->cmpFunction = cmpFuncion;
    st->type = type;
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
                //The first block is the one containing free space in every bucket
                //If the first block is full add a new one at the start of the bucket chain
                secondaryNode *new_node = create_secondaryNode(value,(st->bucketSize/sizeof(value)));
                new_node->next = st->table[h];
                st->table[h] = new_node;
            }
            st->num_elements++;
            st->loadFactor = st->num_elements/(st->numOfBuckets*(st->bucketSize/sizeof(value)));
        }
        return st;
    }
    // Reshape the hash table with doubled size
    else{
        secTable *new_st = reshape_secTable(&st);
        new_st = insert_secTable(new_st, value);
        return new_st;
    }
    
}

//Function to replace a value from the hash table with a given new one
secTable *replace_secTable(secTable *st, void *old_value, void *new_value){
    int h = st->hashFunction(old_value, st->numOfBuckets);
    st->table[h] = deletevalue(st->table[h],old_value,st->cmpFunction);
    return insert_secTable(st, new_value);
}

//Function to get the next prime number
int findNextPrime(int N){
    for(int i=N;i<MAX_PRIME;i++){
        int flag=0;
        for (int j = 2; j <= i / 2; ++j) {
            // condition for non-prime
            if (i % j == 0) {
                flag=1;
                break;
            }
        }
        if(flag==0)
            return i;
    }
}

//Function for reshaping the hash table
secTable *reshape_secTable(secTable **st){
    //Find the closest prime number of the current table size*2
    int new_size = findNextPrime((*st)->numOfBuckets*2);
    //Create a new hash table with doubled the size of the previous one
    secTable *new_st = create_secTable(new_size,(*st)->bucketSize,(*st)->hashFunction,(*st)->cmpFunction,(*st)->type);
    //Iterate through the old hashtable and get every value
    //and insert it into the new hashtable
    for(int i=0;i<(*st)->numOfBuckets;i++){
        while((*st)->table[i]!=NULL){
            void *nvalue;
            (*st)->table[i] = getFirstVal((*st)->table[i],&nvalue);
            if(nvalue!=NULL)
                new_st = insert_secTable(new_st,nvalue);
        }
    }
    free((*st)->table);
    free(*st);
    return new_st;
}

//Function to print a hash table
void print_secTable(secTable *st){
    for(int i=0;i<st->numOfBuckets;i++){
        if(st->table[i]!=NULL) {
            printf("BUCKET %d:\n",i);
            print_secondaryNode(st->table[i],st->type);
        }
    }
}

//Function to print the values of a secondary block
void print_secondaryNode(secondaryNode *node,Data type){
    secondaryNode *cur = node;
    while(cur!=NULL){
        for(int i=0;i<cur->num_elements;i++){
            if(type==String) {
                if(node->values[i]!=NULL)
                    printf("%s\n", (char *) node->values[i]);
            }
            else if (type==Pointer) {
                if(node->values[i]!=NULL)
                    printf("%p\n", node->values[i]);
            }
        }
        cur = cur->next;
    }
}

//Function to get the number of items in the first block of a chain
int getNumElements_secondaryNode(secondaryNode *node){
    return node->num_elements;
}

//Function to create a new secondary node
secondaryNode *create_secondaryNode(void *value,int size){
    secondaryNode *sn = malloc(sizeof(secondaryNode));
    sn->num_elements = 1;
    sn->values = malloc(sizeof(void*) * size);
    for(int i=0;i<size;i++)
        sn->values[i]=NULL;
    sn->values[0] = value;
    sn->next = NULL;
    return sn;
}


//Function to delete a hash table
void destroy_secTable(secTable **st,int mode){
    for(int i=0;i<(*st)->numOfBuckets;i++){
        destroy_secondaryNode(&((*st)->table[i]),mode);
    }
    free((*st)->table);
    free(*st);
    *st=NULL;
}

//Function to destroy a secondary node
void destroy_secondaryNode(secondaryNode **node,int mode){

    secondaryNode *cur = *node;
    while(cur!=NULL){
        for(int i=0;i<cur->num_elements;i++){
            if(mode==ST_SOFT_DELETE_MODE)
                cur->values[i]=NULL;
            else
                free(cur->values[i]);
        }
        free(cur->values);
        secondaryNode *temp = cur;
        cur = cur->next;
        free(temp);
    }

    *node = NULL;
}

//Returns the first Value of the list
secondaryNode *getFirstVal(secondaryNode *node, void **value){

    //If the first block is empty delete it
    if(node->num_elements == 0){
        secondaryNode *tmp = node;
        node = node->next;
        tmp->next = NULL;
        *value = NULL;
        free(tmp->values);
        free(tmp);

        return node;
    }

    //Get the last element from the current block and return it
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

//Function to find if an item exists in the hash table
int find_secTable(secTable *st,void *value){
    int h = st->hashFunction(value,st->numOfBuckets);
    return find_secondaryNode(st->table[h],value,st->cmpFunction);
}

//Function to find if an item exists in a block chain
int find_secondaryNode(secondaryNode *node,void *value,Compare compare_func){

    //Bucket list is empty - the item cannot exist in this list
    if(node==NULL){
        return 0;
    }
    else{
        secondaryNode *cur = node;

        //Iterate through the list and check if the value exists in a block
        while(cur!=NULL){
            for(int i=0;i<cur->num_elements;i++){
                if(compare_func(value,cur->values[i])==1)
                    return 1;
            }

            cur = cur->next;
        }

        return 0;
    }

}