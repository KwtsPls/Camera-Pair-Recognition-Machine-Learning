#ifndef SECTABLE_H
#define SECTABLE_H

#define SB_SIZE 128 
#define ST_INIT_SIZE 10
#define MAX_LOAD_FACTOR 0.9
#define ST_SOFT_DELETE_MODE 0
#define ST_HARD_DELETE_MODE 1


typedef unsigned int (*Hash)(void *, int size);
typedef int (*Compare)(void *, void *);

typedef struct secondaryNode{

    //Number of elements in bucket
    int num_elements;
    // Array of values
    void **values;
    // Pointer to next block of the chain
    secondaryNode *next;

}secondaryNode;


typedef struct SecTable{

    float loadFactor;
    int numOfBuckets;
    int num_elements;

    int bucketSize;
    Hash hashFunction;
    //Pointer to compare Function
    Compare cmpFunction;

    secondaryNode **table;

}secTable;

// Used Thomas Wang's function for hashing pointers
unsigned int HashPointer(void* Ptr,int buckets);

//Used sdbm hash function to hash strings
unsigned int HashString(void* Ptr,int buckets);

//Function to create a new secondary hash table
secTable *create_secTable(int size, int bucketSize, Hash hashFunction, Compare cmpFunction);

//Function to insert a new entry into the 
secTable *insert_secTable(secTable *st, void *value);

//Function to replace a value from the hash table with a given new one
secTable *replace_secTable(secTable *st, void *old_value, void *new_value);

//Function to destroy a hash table
void destroy_secTable(secTable **st);

//Function for reshaping the hash table
secTable *reshape_secTable(secTable **st);

//Function to get the number of items in the first block of a chain
int getNumElements_secondaryNode(secondaryNode *node);

//Function to create a new secondary node
secondaryNode *create_secondaryNode(void *value,int size);

//Returns the first Value of the list
secondaryNode *getFirstVal(secondaryNode *node, void **value);

//Function to destroy a secondary node
void destroy_secondaryNode(secondaryNode **node);

//Function to delete value
secondaryNode *deletevalue(secondaryNode *node, void *value, Compare fun);

//Hash Function
unsigned int HashPointer(void* Ptr,int buckets);


#endif //SECTABLE_H 
