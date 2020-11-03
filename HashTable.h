#ifndef PROJECT_HASHTABLE_H
#define PROJECT_HASHTABLE_H

#define BUCKET_SIZE 512
#include "Dictionary.h"
#include "Bucket.h"

//Struct for the hash table that will store the data
typedef struct hash_table{
    int buckets_num;
    int bucket_size;
    Bucket **table;
}HashTable;


//Hash function for  this hash table -> I used DJB2
unsigned long h2(char *,int);

//Function to initialize a hash table
HashTable *initHashTable(int bucket_size,int buckets_num);


//Function to add an entry to this hash table
HashTable *insertHashTable(HashTable *,Dictionary *spec_id);

//Fucntion to delete the Hash Table
void deleteHashTable(HashTable **destroyed);

//Function to destroy hash table that is no longer in use
void killOldHashTable(HashTable **destroyed);

//Function to print hash table
void printHashTable(HashTable *ht);

#endif //PROJECT_HASHTABLE_H 
