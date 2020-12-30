#ifndef BUCKET_H
#define BUCKET_H

#include <stdio.h>
#include <stdlib.h>
#include "Dictionary.h"
#include "SecTable.h"

#define BUCKET_SOFT_DELETE_MODE 1
#define BUCKET_HARD_DELETE_MODE 2
#define BUCKET_FIRST_FULL 3
#define BUCKET_FIRST_AVAILABLE 4
#define LIST_EMPTY 5
#define LIST_NOT_EMPTY 6

typedef struct Bucket
{
    int cnt;                //Counter for entries in bucket.
    int numofSpecs;         //Max number of entries in the bucket
    Dictionary **spec_ids;  //Pointer's to nodes of Specs
    struct Bucket *next;    //Next Bucket Node
}Bucket;


typedef struct BucketList
{
    int num_entries;
    char dirty_bit;
    secTable *negatives;
    struct Bucket *head;
    struct Bucket *tail;

}BucketList;



//Creates a Bucket Node
Bucket *Bucket_Create(Dictionary *spec_id, int BucketSize);

//Insertion of a new Spec in the node
Bucket *Bucket_Insert(Bucket *buck, Dictionary *spec_id);

//Merging Buckets
Bucket *Bucket_Merge(Bucket *a, Bucket *b);

//Deletion of The Bucket
void Bucket_Delete(Bucket **DestroyIt,int mode);

//Function to print bucket
void Bucket_Print(Bucket *buck);

//Creates a Bucket List
BucketList *BucketList_Create(Dictionary *spec_id, int BucketSize);

//Insertion of a new Spec in the list
BucketList *BucketList_Insert(BucketList *buck, Dictionary *spec_id);

//Function to return wether the first bucket of the bucket is full or not
int BucketList_Bucket_Full(BucketList *bl);

//Function to check if list is empty
int BucketList_Empty(BucketList *b);

//Function to get the first entry of the bucket used for rehasing
Dictionary *Bucket_Get_FirstEntry(BucketList *b);

//Function to delete list of buckets
void BucketList_Delete(BucketList **b, int mode);

//Function to print list of buckets
void BucketList_Print(BucketList *b);

//Function to delete the first bucket in the list
BucketList *BucketList_Delete_First(BucketList **b,int mode);


//Functions take the bucket that needs updating, and where it needs to be pointing correctly at...
BucketList *updateNegativeRelations(BucketList *to_upd, BucketList *to_replace);

//Function that writes Cliques to file
void bucketListWriteCliques(BucketList *lista, FILE *fp);

//Function that writes Negative Cliques to file
BucketList *bucketListWriteNegativeCliques(BucketList *lista,FILE *fp);

//Function for merging negative relations
BucketList* mergeNegativeRelations(BucketList *to_merge, BucketList **to_del);

//Function to find the given BucketList and update it
BucketList *updateSec(BucketList *re, BucketList *to_delete, BucketList *to_replace);

//Function to print the right Spec negative cliques
void rightSpecNegativeCliques(BucketList *set, char *left_sp, FILE *fp);



#endif
