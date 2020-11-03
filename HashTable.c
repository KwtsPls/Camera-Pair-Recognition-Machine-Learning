#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HashTable.h"

//Hash function for this hash table -> I used DJB2
unsigned long h2(char *str,int buckets)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c;

    return hash % buckets;
}

//Function to initialize a hash table
HashTable *initHashTable(int bucket_size,int buckets_num){

    //Allocate space for the hash table
    HashTable *ht = malloc(sizeof(HashTable));

    //Hash table is initialized empty
    ht->buckets_num=buckets_num;

    //The default size of each bucket
    ht->bucket_size=bucket_size;

    //Allocate space for the array of buckets
    ht->table = malloc(sizeof(Bucket*)*buckets_num);

    //Initialize every bucket as NULL
    for(int i=0;i<buckets_num;i++)
        ht->table[i] = NULL;

    
    return ht;
}


//Function to add an entry to this hash table
HashTable *insertHashTable(HashTable *ht,Dictionary *spec_id){

    //Hash the name of the incoming dictionary
    int h = h2(spec_id->dict_name,ht->buckets_num);

    printf("%s\n",spec_id->dict_name);
    //Check if the hashed values belongs to a bucket that is not empty
    //If the bucket is empty create a new bucket
    if(ht->table[h]==NULL){
        ht->table[h] = Bucket_Create(spec_id,ht->bucket_size);
        return ht; 
    }
    //If there is a collision the hash table must be rebuild
    else{
        
        //Create a new hash table with double the size of the old one
        HashTable *new_ht = initHashTable(ht->bucket_size,ht->buckets_num*2);
        h = h2(spec_id->dict_name,new_ht->buckets_num);
        new_ht->table[h] = Bucket_Create(spec_id,new_ht->bucket_size);
        
        //rehash every value into the new hash table
        for(int i=0;i<ht->buckets_num;i++){
            if(ht->table[i]!=NULL){
                h = h2(ht->table[i]->spec_ids[0]->dict_name,new_ht->buckets_num);

                if(new_ht->table[h]==NULL)
                    new_ht->table[h] = ht->table[i];
                else{
                    printf("%s\n",ht->table[i]->spec_ids[0]->dict_name);
                    printf("WRONG COLLISION WITH NEW BUCKETS %d OLD BUCKET %d\n",new_ht->buckets_num,ht->buckets_num);
                }
                    
            }
        }

        killOldHashTable(&ht);
        //Return new hash table
        return new_ht;
    }
}

//Function to destroy hash table that is no longer in use
void killOldHashTable(HashTable **destroyed){
    //Temporary pointer to the hash table to destroy
    HashTable *hash = *destroyed;
    //Delete the buckets    
    free(hash->table);
    //Delete the pointers
    free(*destroyed);
    hash = *destroyed = NULL; 
}

void deleteHashTable(HashTable **destroyed)
{
    //Temporary pointer to the hash table to destroy
    HashTable *hash = *destroyed;
    
    //Delete each bucket seperately
    for(int i=0;i<hash->buckets_num;i++)    
        Bucket_Delete(&(hash->table[i]));

    //Delete the buckets    
    free(hash->table);
    //Delete the pointers
    free(*destroyed);
    hash = *destroyed = NULL;
}

//Function to print hash table
void printHashTable(HashTable *ht){
    for(int i=0;i<ht->buckets_num;i++)
        Bucket_Print(ht->table[i]);
}