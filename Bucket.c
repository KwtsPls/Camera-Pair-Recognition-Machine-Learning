#include <stdio.h>
#include <stdlib.h>
#include "Bucket.h"
#include "Dictionary.h"

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
    return buck;
}



// //Merging Buckets
// Bucket *Bucket_Merge(Bucket *b, Bucket *a)
// {
//     Bucket *temp;
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
//Used for rehasing
Dictionary *Bucket_Get_FirstEntry(BucketList *b){
    return b->head->spec_ids[0];
}

//Deletion of The Bucket (Apetaksamin)
void Bucket_Delete(Bucket **DestroyIt,int mode)
{
    Bucket *next = *DestroyIt;
    while(next!=NULL)
    {
        Bucket *temp = next;
        next = next->next;
        for(int i=0;i<temp->cnt;i++) {
            if(mode==BUCKET_DELETE_MODE)
                deleteDictionary(&temp->spec_ids[i]);
            temp->spec_ids[i] = NULL;
        }

        free(temp->spec_ids); 

        temp->next = NULL;
        // temp->tail = NULL;
        temp->spec_ids = NULL;
        free(temp);
        temp = NULL;
    }
    DestroyIt = NULL;
}

//Prints Bucket
void Bucket_Print(Bucket *buck)
{
    Bucket *tmp = buck;
    int numBuck = 1;
    while(tmp!=NULL)
    {
        printf("Bucket #%d\n",numBuck);
        for(int i=0;i<tmp->cnt;i++)
            printDictionary(tmp->spec_ids[i]);
        numBuck++;
        tmp = tmp->next;
    }
}


void BucketList_Delete(BucketList **b,int mode)
{
    
    Bucket_Delete(&((*b)->head),mode);
    (*b)->tail = NULL;
    free(*b);
    b = NULL;
}


//Prints Bucket List
void BucketList_Print(BucketList *b)
{
    Bucket_Print(b->head);
}



//Write to file the sets inside the bucket
//void Bucket_Write(Bucket *buck, FILE *fd);


