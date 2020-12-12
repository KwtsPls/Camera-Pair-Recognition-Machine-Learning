#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "Bucket.h"
#include "Dictionary.h"
#include "HashTable.h"
#include "ErrorHandler.h"

//Creates a Bucket List
Bucket *Bucket_Create(Dictionary *spec_id, int BucketSize)
{
    Bucket *buck;
    //Allocate memory for the new Bucket
    buck = malloc(sizeof(Bucket));
    //Malloc failed making space for the bucket node, print error, and return NULL 
    if(buck == NULL)
    {
        errorCode = MALLOC_FAILURE;
        print_error();
        return NULL;
    }
    //Number of Strings of Spec Ids that can be stored inside the Array of Spec_ids in this Bucket
    buck->numofSpecs = BucketSize / sizeof(char*);
    //Number of Spec IDs already stored
    buck->cnt = 1;
    //Create an array for storing pointers to strings
    buck->spec_ids = malloc(sizeof(Dictionary*) * buck->numofSpecs);
    //Malloc failed making space for the array of spec_ids, print error, and return NULL 
    if(buck ->spec_ids == NULL)
    {
        errorCode = MALLOC_FAILURE;
        print_error();
        free(buck);
        return NULL;
    }
    //NULL every pointer of array to char*
    for(int i=0; i<buck->numofSpecs; i++)
        buck->spec_ids[i] = NULL;
    //Store First Spec Id
    buck->spec_ids[0] = spec_id;
    //Null the next pointer
    buck->next = NULL;


    return buck;
}

//Creates a Bucket List
BucketList *BucketList_Create(Dictionary *spec_id, int BucketSize)
{
    //Allocate space for the list
    BucketList *lista = malloc(sizeof(BucketList));
    //Malloc failed making space for the BucketList, print error, and return NULL 
    if(lista == NULL)
    {
        errorCode = MALLOC_FAILURE;
        print_error();
        return NULL;
    }
    //Head and tail pointers of the list point to the New Bucket Node
    lista->head = lista->tail = Bucket_Create(spec_id,BucketSize);
    if(lista->head == NULL)
    {
        //Something went wrong while initializing the bucket
        //Error was printed in the Bucket_Create()
        free(lista);
        return NULL;
    }

    //Save the number of entries inside the List
    lista->num_entries = 1;

    //Initialize the dirty bit as zero - clique is not created
    lista->dirty_bit = 0;

    //Initialize the table with the negative relations
    lista->negatives = create_secTable(ST_INIT_SIZE,SB_SIZE,HashPointer,ComparePointer,Pointer);

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
        //An error occured it was printed in Create so...
        if(newBuck == NULL)
            return buck;
        //Insert the new Bucket Node at the beginning of the List
        newBuck->next = buck;
        //Return the bucket
        return newBuck;
    }
    else    //There is space in the Bucket Node
    {
        //Add the spec_id in the Bucket
        buck->spec_ids[buck->cnt] = spec_id;
        //Increase the counter
        buck->cnt++;
        //Return the the bucket 
        return buck;
    }
}



//Insertion of a new Spec in the list
BucketList *BucketList_Insert(BucketList *buck, Dictionary *spec_id)
{
    //Insert something and make the pointer returned the head pointer
    //Every new insertion is stored in the head of the list
    buck->head = Bucket_Insert(buck->head,spec_id);
   
    //Increase the counter of Num of entries
    buck->num_entries++;
    return buck;
}

//Function to get the first entry of the bucket
//Used for rehashing
Dictionary *Bucket_Get_FirstEntry(BucketList *b){
    return b->head->spec_ids[0];
}

//Deletion of The Bucket
void Bucket_Delete(Bucket **DestroyIt,int mode)
{
    //Get the number of entries inside the bucket
    int cnt = (*DestroyIt)->cnt;
    //Check delete Mode
    if(mode==BUCKET_HARD_DELETE_MODE){
        for(int i=0; i<cnt;i++){
            //Delete the dictionary
            deleteDictionary(&(*DestroyIt)->spec_ids[i]);
        }
    }
    //Free pointer for the spec_id
    //If the mode is not BUCKET_HARD_DELETE_MODE then don't delete dictionaries and keep them in memory
    free((*DestroyIt)->spec_ids);
    //Null next pointer
    (*DestroyIt)->next = NULL;
    //Free Memory of bucket
    free((*DestroyIt));
    *DestroyIt = NULL;
}

//Prints Bucket
void Bucket_Print(Bucket *buck)
{
    Bucket *tmp = buck;
    //Iterate through the bucket list and print every spec_id stored in the bucket
    while(tmp!=NULL)
    {
        for(int i=0;i<tmp->cnt;i++) {
            if(buck->spec_ids[i]!=NULL)
                printf("%s\n", buck->spec_ids[i]->dict_name);
        }
        tmp = tmp->next;
    }
}

//Function to delete a BucketList
void BucketList_Delete(BucketList **b,int mode)
{
    //Check if this List was Deleted if not delete it!
    if(*b==NULL)
        return;

    //ST_SOFT_DELETE_MODE is 0
    //ST_HARD_DELETE_MODE is 1
    //And BUCKET_SOFT_DELETE MODE is 1
    //BUCKET_HARD_DELETE_MODE is 2
    //So to delete is mode - 1
    if((*b)->negatives != NULL)
        destroy_secTable(&((*b)->negatives),ST_SOFT_DELETE_MODE);
    //Start from the head of the list iterate through it 
    // and delete each node of the Bucket List 
    while ((*b)->head!=NULL)
    {
        Bucket *temp;
        //Temporary pointer to the head of the list
        temp = (*b)->head;
        //New head of the list is the next node of the list
        (*b)->head = (*b)->head->next;
        //Delete temporary pointer
        Bucket_Delete(&temp,mode);
        temp=NULL;
    }
    //Make all pointers of the list NULL
    (*b)->tail = NULL;
    (*b)->head = NULL;
    if(*b!=NULL) {
        //Free the space that was allocated for the BucketList
        free(*b);
        *b = NULL;
    }
}

//Function to delete the first bucket in the list
BucketList *BucketList_Delete_First(BucketList **b,int mode){
    //If the list contains only one bucket
    if((*b)->head == (*b)->tail){
        //Delete the bucket list
        if((*b)->negatives != NULL)
            destroy_secTable(&((*b)->negatives),ST_SOFT_DELETE_MODE);
        Bucket_Delete(&((*b)->head),mode);
        (*b)->head=NULL;
        (*b)->tail=NULL;
        (*b)->num_entries=0;
    }
    //If the list contains multiple buckets
    else{
        if((*b)->negatives != NULL)
            destroy_secTable(&((*b)->negatives),ST_SOFT_DELETE_MODE);
        //Delete the first bucket and make the bucket head to the next
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



//Function to write the Cliques to file fp
void bucketListWriteCliques(BucketList *lista, FILE *fp){
    Bucket *bucket = lista->head;
    //Write every set of bucket to the file
    while(bucket!=NULL){
        //Iterate through the bucket list and take an item from the dict as left_spec
        //And get the next specs after left and write them as a set in the fp
        for(int i=0;i<bucket->cnt;i++){
            char *line;
            //Create space for storing the left_spec_id
            char *left_spec = malloc(strlen(bucket->spec_ids[i]->dict_name)+1);
            if(left_spec==NULL)
            {
                errorCode = MALLOC_FAILURE;
                print_error();
                continue;
            }
            //Copy its name
            strcpy(left_spec,bucket->spec_ids[i]->dict_name);
            //Write the items of the current bucket in the chain
            //Iterate the spec_ids Dictionary array and write everything that is not written as a set inside
            //the file 
            for(int j=i+1;j<bucket->cnt;j++){
                //Create space for the line that is going to be written in the fp File
                line = malloc(strlen(bucket->spec_ids[j]->dict_name)+strlen(left_spec)+2);
                if(line==NULL)
                {
                    errorCode = MALLOC_FAILURE;
                    print_error();
                    continue;
                }
                //Copy left_spec
                strcpy(line,left_spec);
                //Add comma
                strcat(line,",");
                //Write the right_id in the file
                strcat(line,bucket->spec_ids[j]->dict_name);
                //Write it to file
                int er = fprintf(fp,"%s\n",line);
                //Returned negative written chars to file so there is an error
                if(er<0)
                {
                    errorCode = WRITING_TO_FILE;
                    print_error();
                }
                //Free space for the line
                free(line);
            }
            //Iterate through the rest bucket list
            Bucket *cur = bucket->next;
            while(cur!=NULL){
                //Iterate through the array of Dictionaries
                for(int j=0;j<cur->cnt;j++){
                    //Create space for the line that is going to be written in the fp FILE
                    line = malloc(strlen(cur->spec_ids[j]->dict_name)+strlen(left_spec)+3);
                    if(line==NULL)
                    {
                        errorCode = MALLOC_FAILURE;
                        print_error();
                        continue;
                    }
                    //Write the set to the file
                    strcpy(line,left_spec);
                    strcat(line,",");
                    strcat(line,cur->spec_ids[j]->dict_name);
                    int er = fprintf(fp,"%s\n",line);
                    //Returned negative number, error occured
                    if(er<0)
                    {
                        errorCode = WRITING_TO_FILE;
                        print_error();
                    }
                    //Free space that was allocated for the string of line
                    free(line);
                }
                //Go to the next bucket node
                cur = cur->next;
            }
            //Free space that was allocated for the string of the left_spec_id
            free(left_spec);
        }
        //Go to the next bucket node
        bucket = bucket->next;
    }
}


//Functions take the bucket that needs updating, and where it needs to be pointing correctly at...
BucketList *updateNegativeRelations(BucketList *to_upd, BucketList *to_replace){
    secTable *negatives_to_upd = to_upd->negatives;
    //Iterate through all the negative relations of the to update list
    for(int i=0;i<negatives_to_upd->numOfBuckets;i++){
        secondaryNode *tmp = negatives_to_upd->table[i];
        while (tmp!=NULL)
        {
            for(int i=0;i<tmp->num_elements;i++){
                //Get every bucketlist* and update them to point as negative
                //To the new bucketlist
                tmp->values[i] =(void*) updateSec((BucketList *) tmp->values[i],to_upd,to_replace);
            }
            tmp = tmp->next;
        }
        
    }
    to_upd->negatives = negatives_to_upd;
    return to_upd;

}

//Function to merge negative Relations
BucketList* mergeNegativeRelations(BucketList *to_merge, BucketList **to_del){
    secTable *neg = (*to_del)->negatives;
    for(int i=0;i<neg->numOfBuckets;i++)
    {
        while (neg->table[i]!=NULL)
        {
            //Get the first value of this bucket node
            void *nvalue;
            neg->table[i] = getFirstVal(neg->table[i],&nvalue);
            if(nvalue != NULL){
                //If it does'nt exist to the new table, insert it
                if(find_secTable(to_merge->negatives,nvalue)==0)
                    to_merge->negatives = insert_secTable(to_merge->negatives,nvalue);
            }
        }
    }
    //Now its time to delete the negative values of the list...
    //Because i delete it in the first val, it's better to free the negative sectable here...
    
    return to_merge;
}

//Function to find the given BucketList and update it
BucketList *updateSec(BucketList *re, BucketList *to_delete, BucketList *to_replace){
    //Check if the bucket list that is going to be replaced already exists
    //If it exists just delete the old, if it doesn't replace it....
    if((find_secTable(re->negatives,to_replace) == 1)){
        re->negatives = deletevalue_secTable(re->negatives,to_delete);
        return re;
    }
    else{
        re->negatives = replace_secTable(re->negatives,to_delete,to_replace);
        return re;
    }
}