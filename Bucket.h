#ifndef BUCKET_H
#define BUCKET_H

#include <stdio.h>
#include <stdlib.h>
#include "Dictionary.h"

#define BUCKET_REHASH_MODE 1
#define BUCKET_DELETE_MODE 2

typedef struct Bucket
{
    int cnt;                //Counter for entries in bucket.
    int numofSpecs;         //Max number of entries in the bucket
    Dictionary **spec_ids;  //Pointer's to nodes of Specs
    struct Bucket *next;    //Next Bucket Node
}Bucket;


typedef struct BucketList
{
    struct Bucket *head;
    struct Bucket *tail;

}BucketList;



//Creates a Bucket Node
Bucket *Bucket_Create(Dictionary *spec_id, int BucketSize);

//Insertion of a new Spec in the node
Bucket *Bucket_Insert(Bucket *buck, Dictionary *spec_id);

//Merging Buckets
Bucket *Bucket_Merge(Bucket *a, Bucket *b);

//Deletion of The Bucket (Apetaksamin)
void Bucket_Delete(Bucket **DestroyIt,int mode);

//Function to get the first entry of the bucket
//Used for rehasing
// Dictionary *Bucket_Get_FirstEntry(BucketList *b);

void Bucket_Print(Bucket *buck);


//Creates a Bucket List
BucketList *BucketList_Create(Dictionary *spec_id, int BucketSize);


//Insertion of a new Spec in the list
BucketList *BucketList_Insert(BucketList *buck, Dictionary *spec_id);


//Function to get the first entry of the bucket
//Used for rehasing
Dictionary *Bucket_Get_FirstEntry(BucketList *b);

void BucketList_Delete(BucketList **b, int mode);

void BucketList_Print(BucketList *b);


// BucketList *BucketList_Merge(BucketList *a, BucketList *b);

//Write to file the sets inside the bucket
// void Bucket_Write(Bucket *buck, FILE *fd);



#endif
