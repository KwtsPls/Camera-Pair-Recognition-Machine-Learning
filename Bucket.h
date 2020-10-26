#ifndef BUCKET_H
#define BUCKET_H
#include <stdio.h>
#include <stdlib.h>

typedef struct Bucket
{
    int cnt;                //Counter for already spec_ids.
    int numofSpecs;         //Number of nodes that can store one list Node
    char **spec_ids;          //Pointer's to nodes of Specs
    struct Bucket *tail;        //Pointer to the last Node
    struct Bucket *next;    //Next Bucket Node
}Bucket;



//Creates a Bucket List
Bucket *Bucket_Create(char *spec_id, int BucketSize);

//Insertion of a new Spec in the list
Bucket *Bucket_Insert(Bucket *buck, char *spec_id);

//Merging Buckets
Bucket *Bucket_Merge(Bucket *a, Bucket *b);

//Deletion of The Bucket (Apetaksamin)
void Bucket_Delete(Bucket **DestroyIt);


void Bucket_Print(Bucket *buck);


//Write to file the sets inside the bucket
// void Bucket_Write(Bucket *buck, FILE *fd);



#endif
