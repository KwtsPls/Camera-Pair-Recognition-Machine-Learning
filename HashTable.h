#ifndef PROJECT_HASHTABLE_H
#define PROJECT_HASHTABLE_H

#define BUCKET_SIZE 512

//Struct for the hash table that will store the data
typedef struct hash_table{
    int buckets_num;
    int bucket_size;
    Bucket **
}HashTable;


//Hash function for this hash table -> I used DJB2
unsigned long h2(char *,int);

#endif //PROJECT_HASHTABLE_H
