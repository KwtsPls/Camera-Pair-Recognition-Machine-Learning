#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HashTable.h"

//Hash function for this hash table -> I used DJB2
unsigned long hashCode(char *str,int buckets)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c;

    return hash % buckets;
}


//Function to create a keyBucket
keyBucket *initKeyBucket(Dictionary *spec_id){

    //Allocate memory for the new bucket
    keyBucket *kb = malloc(sizeof(keyBucket));

    //Size is defined as a constant
    kb->bucket_size = BUCKET_SIZE/4;

    //New bucket is created with one entry
    kb->num_entries=1;

    //Max number of entries inside bucket
    kb->max_entries= BUCKET_SIZE/(4*sizeof(Dictionary*));

    //Allocate memory for the contents of the bucket
    kb->array = malloc(sizeof(keyBucketEntry*)*kb->max_entries);

    //Initialize each array position as NULL
    for(int i=0;i<kb->max_entries;i++)
        kb->array[i]=NULL;
    
    //Insert the first entry of the array
    kb->array[0]= createEntry(spec_id);

    return kb;
}

//Function to create a new key bucket entry
keyBucketEntry *createEntry(Dictionary *dict)
{
    //Allocate memory for the new entry
    keyBucketEntry *entry = malloc(sizeof(keyBucketEntry));
    
    //Save the name of the current spec id
    entry->key = strdup(dict->dict_name); 

    //Save the number of entries inside the outer_entry
    entry->num_entries = 1;

    //Create Bucket for the entry set
    entry->set = BucketList_Create(dict,BUCKET_SIZE);

    return entry;
}

//Function to initialize a hash table
HashTable *initHashTable(int buckets_num){

    //Allocate space for the hash table
    HashTable *ht = malloc(sizeof(HashTable));

    //Hash table is initialized empty
    ht->buckets_num=buckets_num;

    //Allocate space for the array of buckets
    ht->table = malloc(sizeof(keyBucket*)*buckets_num);

    //Initialize every bucket as NULL
    for(int i=0;i<buckets_num;i++)
        ht->table[i] = NULL;

    return ht;
}

//Check if a key bucket has space for one more entry
int keyBucketAvailable(keyBucket *kb){
    if((kb->num_entries+1)>kb->max_entries)
        return KEY_BUCKET_FULL;
    else
        return KEY_BUCKET_AVAILABLE;
}

//Insert a new entry into the key bucket -> only used if bucket has remaining space
keyBucket *insertBucketEntry(keyBucket *kb,Dictionary *spec_id){

    //Create a data bucket for the new spec id
    kb->array[kb->num_entries] = createEntry(spec_id);

    //Increase the entries counter
    kb->num_entries++;

    return kb;
}

//Function to add an entry to this hash table
HashTable *insertHashTable(HashTable **ht,Dictionary *spec_id){

    //hash the name of the spec_id
    int h = hashCode(spec_id->dict_name,(*ht)->buckets_num);

    //Check if the bucket is empty
    if((*ht)->table[h]==NULL)
        (*ht)->table[h] = initKeyBucket(spec_id);
    //If bucket is not empty check if there is enough space for a new entry
    //If there is insert it into the bucket
    else if(keyBucketAvailable((*ht)->table[h])==KEY_BUCKET_AVAILABLE){
        (*ht)->table[h] = insertBucketEntry((*ht)->table[h],spec_id);
    }
    //A bucket is full - hash table must be reshaped
    else{
        HashTable *ht_rehashed = reshapeHashTable(ht,spec_id);
        return ht_rehashed;
    }

    return *ht;
}

//Function to get the first spec id of a given key bucket
Dictionary *getTopKeyBucketEntry(keyBucket *kb, int pos){
    return Bucket_Get_FirstEntry(kb->array[pos]->set);
}

//Function to double the size of the current hash table;
HashTable *reshapeHashTable(HashTable **ht,Dictionary *spec_id){

    //Create a new hash table with double the size of the current one
    HashTable *ht_reshaped = initHashTable((*ht)->buckets_num*2);

    //Rehash the values of the old hash table into the new one
    for(int i=0;i<(*ht)->buckets_num;i++){

        //Iterate through the key bucket
        if((*ht)->table[i]!=NULL){
            for(int j=0;j<(*ht)->table[i]->num_entries;j++){
                Dictionary *dict = getTopKeyBucketEntry((*ht)->table[i],j);
                ht_reshaped = insertHashTable(&ht_reshaped,dict);
            }
        }
    }

    //Insert the entry that created the conflict
    ht_reshaped = insertHashTable(&ht_reshaped,spec_id);

    deleteHashTable(ht,BUCKET_REHASH_MODE);
    ht=NULL;

    return ht_reshaped;
}


//Function to find a key bucket entry with the given spec_id
keyBucketEntry *findKeyBucketEntry(HashTable *ht,char * spec_id){
    int h = hashCode(spec_id,ht->buckets_num);

    //If current bucket is empty return NULL -> entry does not exist
    if(ht->table[h]==NULL)
        return NULL;

    //Iterate through the key bucket to find the spec id
    for(int i=0;i<ht->table[h]->num_entries;i++){

            if(ht->table[h]->array[i]!=NULL){
                if(strcmp(ht->table[h]->array[i]->key,spec_id)==0)
                    return ht->table[h]->array[i];
            }
    }

    return NULL;
}

//Function to delete the Hash Table
void deleteHashTable(HashTable **destroyed,int mode){
    HashTable *temp;
    temp = *destroyed;
    for(int i=0;i<temp->buckets_num;i++){
        if(temp->table[i]!=NULL)
            deleteKeyBucket(&(temp->table[i]),mode);
    }
    free(temp->table);
    free(*destroyed);
    *destroyed = temp = NULL;   
}

//Function to deleteKeyBucket
void deleteKeyBucket(keyBucket **destroyed,int mode){
    keyBucket *temp;
    temp = *destroyed;
    for(int i=0;i<temp->num_entries;i++){
        if(temp->array[i]!=NULL)
            deleteOuterEntry(&(temp->array[i]),mode);
    }
    free(temp->array);
    free(*destroyed);
    *destroyed = temp = NULL;
}

//Function to deleteOuterEntry
void deleteOuterEntry(keyBucketEntry **destroyed,int mode){
    keyBucketEntry *temp;
    temp = *destroyed;
    free(temp->key);
    BucketList_Delete(&temp->set,mode);
    free(*destroyed);
    *destroyed = temp = NULL;
}

//Function to print hash table
void printHashTable(HashTable *ht){
    printf("CURRENT HASH TABLE NUM OF BUCKETS : %d\n\n",ht->buckets_num);
    for(int i=0;i<ht->buckets_num;i++){
        if(ht->table[i]!=NULL)
            printf("\nNUMBER OF ENTRIES IN BUCKET %d ---> %d\n",i,ht->table[i]->num_entries);
    }
}


HashTable *mergeHashTable(HashTable **ht, char *left_sp, char *right_sp){
}