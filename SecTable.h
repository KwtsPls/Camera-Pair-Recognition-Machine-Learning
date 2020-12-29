#ifndef SECTABLE_H
#define SECTABLE_H

#define SB_SIZE 128 
#define ST_INIT_SIZE 2
#define MAX_LOAD_FACTOR 0.9
#define ST_SOFT_DELETE_MODE 0
#define ST_HARD_DELETE_MODE 1
#define MAX_PRIME 100000
#define MIN_TF_IDF 0.001200

// Hash functions for the different data types in the hash table
typedef unsigned int (*Hash)(void *, int size);
// Compare functions for the different data types in the hash table
typedef int (*Compare)(void *, void *);
//Functions to destroy values in secondary nodes
typedef void (*Delete)(void *);
// Data types for the hash table
typedef enum data_types {Pointer,String,indxWrd} Data;

typedef struct secondaryNode{

    //Number of elements in bucket
    int num_elements;
    //Array of values
    void **values;
    //Pointer to next block of the chain
    struct secondaryNode *next;

}secondaryNode;


typedef struct SecTable{

    //Data type of the hash table
    Data type;
    //Hash tables load factor
    float loadFactor;
    //Number of buckets in the hash table
    int numOfBuckets;
    //Current number of elements in the table
    int num_elements;
    //Size of each bucket in the table
    int bucketSize;
    //Pointer to hash function type
    Hash hashFunction;
    //Pointer to compare function type
    Compare cmpFunction;
    //Pointer to delete function type
    Delete  deleteFunction;
    //The main table
    secondaryNode **table;

}secTable;

/*######## Hash Functions ############################*/

// Used Thomas Wang's function for hashing pointers
unsigned int HashPointer(void* Ptr,int buckets);

//Used sdbm hash function to hash strings
unsigned int HashString(void* Ptr,int buckets);

//Used sdbm hash function to hash strings
unsigned int HashIndexedWord(void* Ptr,int buckets);

/*#####################################################*/

/*######## Compare Functions ############################*/

//Returns 1 if pointers are the same
int ComparePointer(void *a, void *b);

//Returns 1 if strings are the same
int CompareString(void * a, void *b);

//Returns 1 if strings if two indexed words are the same
int CompareIndexedWord(void * a, void *b);

/*#####################################################*/

/*######## Delete Functions ############################*/

//Function to delete strings
void DeleteString(void *Ptr);

//Function to delete indexed words;
void DeleteIndexedWord(void *Ptr);

/*######################################################*/

//Function to create a new secondary hash table
secTable *create_secTable(int size, int bucketSize, Hash hashFunction, Compare cmpFunction,Delete deleteFunction,Data type);

//Function to insert a new entry into the 
secTable *insert_secTable(secTable *st, void *value);

//Function to replace a value from the hash table with a given new one
secTable *replace_secTable(secTable *st, void *old_value, void *new_value);

//Function for reshaping the hash table
secTable *reshape_secTable(secTable **st);

//Function to get the next prime number
int findNextPrime(int N);

//Function to get the number of items in the first block of a chain
int getNumElements_secondaryNode(secondaryNode *node);

//Function to create a new secondary node
secondaryNode *create_secondaryNode(void *value,int size);

//Returns the first Value of the list
secondaryNode *getFirstVal(secondaryNode *node, void **value);

//Function to destroy a secondary node
void destroy_secondaryNode(secondaryNode **node,Delete deleteFunction,int mode);

//Function to delete value
secTable *deletevalue_secTable(secTable *st, void *value,int mode);

//Function to delete value
secondaryNode *deletevalue(secondaryNode *node, void *value, Compare compareFunction,Delete deleteFunction,int mode);

//Hash Function
unsigned int HashPointer(void* Ptr,int buckets);

//Function to delete a hash table
void destroy_secTable(secTable **st,int mode);

//Function to print a hash table
void print_secTable(secTable *st);

//Function to print the values of a secondary block
void print_secondaryNode(secondaryNode *node,Data type);

//Function to find if an item exists in the hash table
int find_secTable(secTable *st,void *value);

//Function to find if an item exists in a block chain
int find_secondaryNode(secondaryNode *node,void *value,Compare compare_func);

//Function to find if an item exists in the hash table
void *getIndexedWord_secTable(secTable *st,char *value);

//Function to find if an item exists in a block chain
void *getIndexedWord_secondaryNode(secondaryNode *node,char *value,Compare compare_func);

//Function to update the current tf value of a word
secTable *updateTF_secTable(secTable *st,void *value);

//Function to update the mean of tf and the idf value of all words in table unique_words
secTable *update_tf_idf_values(secTable *st,secTable *unique_words,int text_len);

//Function to update the mean of tf and idf value of a given word
secTable *update_tf_idf_word(secTable *st,char *value,int text_len);

//Function to evaluate the tf-idf mean of every word in the vocabulary
secTable *evaluate_tfidf_secTable(secTable *vocabulary,int num_texts);

//Function to write negative Cliques
void secTable_writeNegativeCliques(secTable *st, char *left_sp, FILE *fp);

#endif //SECTABLE_H 
