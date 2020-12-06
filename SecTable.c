#include <stdio.h>
#include <stdlib.h>
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


//Function to create a new secondary hash table
secTable *create_secTable(int size, int bucketSize,Hash hashFunction){

    secTable *st = malloc(sizeof(secTable));
    st->loadFactor = 0;
    st->num_elements = 0;
    st->bucketSize=bucketSize;
    st->numOfBuckets = size;
    st->hashFunction = hashFunction;
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
            st->table[h] = create_secondaryNode(value,(st->bucketSize/sizeof(value));
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
        
    }
    // Reshape the hash table with doubled size
    else{

    }
    
}

//Function to replace a value from the hash table with a given new one
secTable *replace_secTable(secTable *st, void *old_value, void *new_value);

//Function to destroy a hash table
void destroy_secTable(secTable **st);

//Function for reshaping the hash table
secTable *reshape_secTable(secTable **st);

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
void destroy_secondaryNode(secondaryNode **node, int mode){
    secondaryNode *tmp = node;
    while (tmp!=NULL)
    {
        
    }
    
    
}