#ifndef PROJECT_HASHTABLE_H
#define PROJECT_HASHTABLE_H

#include "Dictionary.h"
#include "Bucket.h"

#define BUCKET_SIZE 32
#define KEY_BUCKET_SIZE 128
#define KEY_BUCKET_AVAILABLE 1
#define KEY_BUCKET_FULL 2

//Struct for the entries of the bucket array
typedef struct outer_entry{
    char *key;
    BucketList *set;
} keyBucketEntry;

//Struct for the buckets containing the spec_ids
typedef struct outer_bucket{
    int bucket_size;
    int num_entries;
    int max_entries;
    keyBucketEntry **array;
} keyBucket;

//Struct for the hash table that will store the data
typedef struct hash_table{
    int buckets_num;
    keyBucket **table;
}HashTable;

//Hash function for  this hash table -> I used DJB2
unsigned long hashCode(char *,int);

//Function to create a keyBucket
keyBucket *initKeyBucket(Dictionary *spec_id);

//Check if a key bucket has space for one more entry
int keyBucketAvailable(keyBucket *kb);

//Function to create a new key bucket entry
keyBucketEntry *createEntry(Dictionary *dict);

//Insert a new entry into the key bucket
keyBucket *insertBucketEntry(keyBucket *kb,Dictionary *spec_id);

//Function to get the first spec id of a given key bucket
Dictionary *getTopKeyBucketEntry(keyBucket *kb, int pos);

//Function to double the size of the current hash table;
HashTable *reshapeHashTable(HashTable **ht,Dictionary *spec_id);

//Function to initialize a hash table
HashTable *initHashTable(int buckets_num);

//Function to add an entry to this hash table
HashTable *insertHashTable(HashTable **ht,Dictionary *spec_id);

//Function to find a key bucket entry with the given spec_id and return its index
int findKeyBucketEntry(HashTable *ht,char * spec_id);

//Function to delete the Hash Table
void deleteHashTable(HashTable **destroyed,int mode);

//Function to delete the Hash Table after the cliques are formed
void cliqueDeleteHashTable(HashTable **ht,int mode);

//Function to set all pointers pointing to a certain clique as NULL
HashTable *erasePointerHashTable(HashTable **ht,keyBucketEntry *clique_bucket);

//Function to deleteKeyBucket
void deleteKeyBucket(keyBucket **destroyed,int mode);

//Function to destroy hash table that is no longer in use
void killOldHashTable(HashTable **destroyed);

//Function to print hash table
void printHashTable(HashTable *ht);

//Function for merging same specs
HashTable *createCliqueHashTable(HashTable **ht, char *left_sp,char *right_sp);

//Function to perform set union between two sets
//Max_List : Number of entries in this bucket is greater than those in min_List
BucketList *BucketList_Merge(BucketList **Max_List, BucketList **min_List,HashTable **ht,int h,int index);

#endif //PROJECT_HASHTABLE_H 
