#include <stdio.h>
#include <stdlib.h>
#include "Bucket.h"
#include "Dictionary.h"
#include "HashTable.h"

//Creates a Bucket List
Bucket *Bucket_Create(Dictionary *spec_id, int BucketSize)
{
    Bucket *buck;
    //Allocate memory for the new Bucket
    buck = malloc(sizeof(Bucket));
    //Number of Strings of Spec Ids that can be stored inside the Array of Spec_ids in this Bucket
    buck->numofSpecs = BucketSize / sizeof(char*);
    //Number of Spec IDs already stored
    buck->cnt = 1;
    //Create an array for storing pointers to strings
    buck->spec_ids = malloc(sizeof(Dictionary*) * buck->numofSpecs);
    //NULL every pointer of array to char*
    for(int i=0; i<buck->numofSpecs; i++)
        buck->spec_ids[i] = NULL;
    //Store First Spec Id
    buck->spec_ids[0] = spec_id;
    //Null the next pointer
    buck->next = NULL;

    //Last pointer to this Node
    // buck->tail = buck;

    return buck;
}

//Creates a Bucket List
BucketList *BucketList_Create(Dictionary *spec_id, int BucketSize)
{
    //Allocate space for the list
    BucketList *lista = malloc(sizeof(BucketList));
    //Head and tail pointers of the list point to the New Bucket Node
    lista->head = lista->tail = Bucket_Create(spec_id,BucketSize);

    //Save the number of entries inside the List
    lista->num_entries = 1;

    //Return the new BucketList
    return lista;
}


//Insertion of a new Spec in the list
Bucket *Bucket_Insert(Bucket* buck, Dictionary *spec_id)
{
    if(buck->cnt==buck->numofSpecs) //There is no space in the BucketNode
    {
        //Create a newBucket to store the spec_id
        Bucket *newBuck = Bucket_Create(spec_id, buck->numofSpecs * (sizeof(Dictionary*)));
        //Insert the new Bucket Node at the beggining of the List
        newBuck->next = buck;
        // newBuck->tail = buck->tail;

        return newBuck;
    }
    else    //There is space in the Bucket Node
    {
        //Add the spec_id in the Bucket
        buck->spec_ids[buck->cnt] = spec_id;
        buck->cnt++;

        return buck;
    }
}



//Insertion of a new Spec in the list
BucketList *BucketList_Insert(BucketList *buck, Dictionary *spec_id)
{
    //Insert something and make the pointer returned the head pointer
    //Every new insertion is stored in the head of the list
    buck->head = Bucket_Insert(buck->head,spec_id);
    buck->num_entries++;
    return buck;
}



// //Merging Buckets
// Bucket *Bucket_Merge(Bucket *b, Bucket *a)
// {
//     if(b->cnt == b->numofSpecs) //Bucket b is full
//     {
//         //Store the head of a
//         Bucket *head = a;
//         //Last node of a is pointing to the head of b
//         head->tail->next = b;
//         //Last node of head is b's Last node
//         head->tail = b->tail;
//         return head;
//     }
//     else{
//         if(a->cnt == a->numofSpecs) //This means bucket a is full
//         {
//             //Store the head of b
//             Bucket *head = b;
//             //Last node of b is pointing to the head of a
//             head->tail->next = a;
//             //Last node of head is a's Last node
//             head->tail = a->tail;
//             return head;
//         }
//         else
//         {
//             //While a is not empty and b is not Full
//             while(b->cnt!=b->numofSpecs && a->cnt!=0)
//             {
//                 //Then take one spec_id from bucket a
//                 a->cnt--;

//                 Dictionary *temp = a->spec_ids[a->cnt];
//                 a->spec_ids[a->cnt]=NULL;

//                 //And keep it on bucket b
//                 b->spec_ids[b->cnt]=temp;
//                 b->cnt++;
//             }
//             if(a->cnt == 0) //So there is an empty bucket, need to delete it
//             {
//                 Bucket *temp = a;
//                 a = a->next;
//                 temp->next = NULL;
//                 Bucket_Delete(&temp,BUCKET_REHASH_MODE);
//                 //Store the head of b
//                 Bucket *head = b;
//                 //Last node of b is pointing to the head of a
//                 head->tail->next = a;
//                 //Last node of head is a's Last node
//                 head->tail = a->tail;
//                 return head;
//             }

//             //So b is full and a is not empty so...
//             //Store the head of a
//             Bucket *head = a;
//             //Last node of a is pointing to the head of b
//             head->tail->next = b;
//             //Last node of head is b's Last node
//             head->tail = b->tail;
//             return head;
//         }

//     }
// }

//Function to get the first entry of the bucket
//Used for rehashing
Dictionary *Bucket_Get_FirstEntry(BucketList *b){
    return b->head->spec_ids[0];
}

//Deletion of The Bucket
void Bucket_Delete(Bucket **DestroyIt,int mode)
{
    int cnt = (*DestroyIt)->cnt;
    if(mode==BUCKET_DELETE_MODE){
        for(int i=0; i<cnt;i++){
            deleteDictionary(&(*DestroyIt)->spec_ids[i]);
        }
    }
    free((*DestroyIt)->spec_ids);
    (*DestroyIt)->next = NULL;
    free((*DestroyIt));
    *DestroyIt = NULL;
}

//Prints Bucket
void Bucket_Print(Bucket *buck)
{
    Bucket *tmp = buck;
    int numBuck = 1;
    while(tmp!=NULL)
    {
        printf("\n\nBUCKET %d:\n",numBuck);
        for(int i=0;i<tmp->cnt;i++)
            printf("%s\n",buck->spec_ids[i]->dict_name);
        numBuck++;
        tmp = tmp->next;
    }
}

//Function to delete a BucketList
void BucketList_Delete(BucketList **b,int mode)
{
    if(*b==NULL)
        return;

    while ((*b)->head!=NULL)
    {
        Bucket *temp;
        temp = (*b)->head;
        (*b)->head = (*b)->head->next;
        Bucket_Delete(&temp,mode);
        temp=NULL;
    }
    (*b)->tail = NULL;
    (*b)->head = NULL;
    if(*b!=NULL) {
        free(*b);
        *b = NULL;
    }
}

//Function to delete the first bucket in the list
BucketList *BucketList_Delete_First(BucketList **b,int mode){
    //If the list contains only one bucket
    if((*b)->head == (*b)->tail){
        Bucket_Delete(&((*b)->head),mode);
        (*b)->head=NULL;
        (*b)->tail=NULL;
        (*b)->num_entries=0;
    }
    //If the list contains multiple buckets
    else{
        Bucket *to_be_deleted=(*b)->head;
        (*b)->head = (*b)->head->next;
        (*b)->num_entries = (*b)->num_entries - to_be_deleted->cnt;
        Bucket_Delete(&to_be_deleted,mode);
    }

    return *b;
}

//Prints Bucket List
void BucketList_Print(BucketList *b){
    Bucket_Print(b->head);
}

//Function to return whether the first bucket of the bucket is full or not
int BucketList_Bucket_Full(BucketList *bl){
    if(bl->head->numofSpecs == bl->head->cnt)
        return BUCKET_FIRST_FULL;
    else
        return BUCKET_FIRST_AVAILABLE;
}

//Function to check if list is empty
int BucketList_Empty(BucketList *b){
    if(b->head==NULL)
        return LIST_EMPTY;
    else
        return LIST_NOT_EMPTY;
}

//Write to file the sets inside the bucket
//void Bucket_Write(Bucket *buck, FILE *fd);
