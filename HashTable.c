#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HashTable.h"

//Hash function for this hash table -> I used DJB2
unsigned long hashCode(char *str,int buckets)
{
    unsigned long hash = 5381;
    int c;

    while((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash % buckets;
}


//Function to create a keyBucket
keyBucket *initKeyBucket(Dictionary *spec_id){


    //Allocate memory for the new bucket
    keyBucket *kb = malloc(sizeof(keyBucket));
    if(kb == NULL)
    {
        //Malloc failed
        errorCode = MALLOC_FAILURE;
        print_error();
        return NULL;
    }

    //Size is defined as a constant
    kb->bucket_size = KEY_BUCKET_SIZE;

    //New bucket is created with one entry
    kb->num_entries=1;

    //Max number of entries inside bucket
    kb->max_entries= KEY_BUCKET_SIZE/(sizeof(Dictionary*));

    //Allocate memory for the contents of the bucket
    kb->array = malloc(sizeof(keyBucketEntry*)*kb->max_entries);
    if(kb->array == NULL)
    {
        //Malloc failed
        errorCode = MALLOC_FAILURE;
        print_error();
        free(kb);
        return NULL;
    }


    //Initialize each array position as NULL
    for(int i=0;i<kb->max_entries;i++)
        kb->array[i]=NULL;
    
    //Insert the first entry of the array
    kb->array[0]= createEntry(spec_id);

    return kb;
}

//Function to create a new key bucket entry
keyBucketEntry *createEntry(Dictionary *dict)
{
    //Allocate memory for the new entry
    keyBucketEntry *entry = malloc(sizeof(keyBucketEntry));
    if(entry == NULL)
    {
        //Malloc failed
        errorCode = MALLOC_FAILURE;
        print_error();
        return NULL;
    }


    //Save the name of the current spec id
    entry->key = strdup(dict->dict_name); 
    if(entry->key == NULL)
    {
        //Malloc failed
        errorCode = MALLOC_FAILURE;
        free(entry);
        print_error();
        return NULL;
    }
    
    //Create Bucket for the entry set
    entry->set = BucketList_Create(dict,BUCKET_SIZE);
    if(entry->set == NULL)
    {
        //Something went wrong while initialising BucketList
        //It was printed in BucketList_Create
        free(entry->key);
        free(entry);
        return NULL;
    }

    return entry;
}

//Function to initialize a hash table
HashTable *initHashTable(int buckets_num){

    //Allocate space for the hash table
    HashTable *ht = malloc(sizeof(HashTable));
    if(ht == NULL)
    {
        //Malloc failed
        errorCode = MALLOC_FAILURE;
        print_error();
        return NULL;
    }

    //Hash table is initialized empty
    ht->buckets_num=buckets_num;

    //Allocate space for the array of buckets
    ht->table = malloc(sizeof(keyBucket*)*buckets_num);
    if(ht->table == NULL)
    {
        //Malloc failed
        errorCode = MALLOC_FAILURE;
        print_error();
        free(ht);
        return NULL;
    }


    //Initialize every bucket as NULL
    for(int i=0;i<buckets_num;i++)
        ht->table[i] = NULL;

    return ht;
}

//Check if a key bucket has space for one more entry
int keyBucketAvailable(keyBucket *kb){
    if((kb->num_entries+1)>kb->max_entries)
        return KEY_BUCKET_FULL;
    else
        return KEY_BUCKET_AVAILABLE;
}

//Insert a new entry into the key bucket -> only used if bucket has remaining space
keyBucket *insertBucketEntry(keyBucket *kb,Dictionary *spec_id){

    //Create a data bucket for the new spec id
    kb->array[kb->num_entries] = createEntry(spec_id);

    //Increase the entries counter
    kb->num_entries++;

    return kb;
}

//Function to add an entry to this hash table
HashTable *insertHashTable(HashTable **ht,Dictionary *spec_id){

    //hash the name of the spec_id
    int h = hashCode(spec_id->dict_name,(*ht)->buckets_num);

    //Check if the bucket is empty
    if((*ht)->table[h]==NULL)
        (*ht)->table[h] = initKeyBucket(spec_id);
    //If bucket is not empty check if there is enough space for a new entry
    //If there is insert it into the bucket
    else if(keyBucketAvailable((*ht)->table[h])==KEY_BUCKET_AVAILABLE){
        (*ht)->table[h] = insertBucketEntry((*ht)->table[h],spec_id);
    }
    //A bucket is full - hash table must be reshaped
    else{
        HashTable *ht_rehashed = reshapeHashTable(ht,spec_id);
        return ht_rehashed;
    }

    return *ht;
}

//Function to get the first spec id of a given key bucket
Dictionary *getTopKeyBucketEntry(keyBucket *kb, int pos){
    return Bucket_Get_FirstEntry(kb->array[pos]->set);
}

//Function to double the size of the current hash table;
HashTable *reshapeHashTable(HashTable **ht,Dictionary *spec_id){

    //Create a new hash table with double the size of the current one
    int new_size = findNextPrime((*ht)->buckets_num*2);
    HashTable *ht_reshaped = initHashTable(new_size);

    //Rehash the values of the old hash table into the new one
    for(int i=0;i<(*ht)->buckets_num;i++){

        //Iterate through the key bucket
        if((*ht)->table[i]!=NULL){
            for(int j=0;j<(*ht)->table[i]->num_entries;j++){
                Dictionary *dict = getTopKeyBucketEntry((*ht)->table[i],j);
                ht_reshaped = insertHashTable(&ht_reshaped,dict);
            }
        }
    }

    //Insert the entry that created the conflict
    ht_reshaped = insertHashTable(&ht_reshaped,spec_id);

    //Delete old hash table but do not delete the dictionaries
    deleteHashTable(ht,BUCKET_SOFT_DELETE_MODE);
    ht=NULL;

    return ht_reshaped;
}


//Function to find a key bucket entry with the given spec_id
int findKeyBucketEntry(HashTable *ht,char * spec_id){
    if(spec_id==NULL)
        return -1;

    int h = hashCode(spec_id,ht->buckets_num);

    //If current bucket is empty return NULL -> entry does not exist
    if(ht->table[h]==NULL)
        return -1;

    //Iterate through the key bucket to find the spec id
    for(int i=0;i<ht->table[h]->num_entries;i++){

            if(ht->table[h]->array[i]!=NULL && ht->table[h]->array[i]->key!=NULL){
                if(strcmp(ht->table[h]->array[i]->key,spec_id)==0)
                    return i;
            }
    }

    return -1;
}

//Function to return the number of entries inside the hashtable
int sizeHashTable(HashTable *ht){

    int sum=0;
    //Iterate through the buckets of HashTable
    for(int i=0;i<ht->buckets_num;i++){
        if(ht->table[i]!=NULL) //If its not NULL
            //Add to sum the num of entries
            sum += ht->table[i]->num_entries;
    }

    return sum;
}

//Function to delete the Hash Table
void deleteHashTable(HashTable **ht,int mode){
    //Iterate through its bucket of Hash Table
    for(int i=0;i<(*ht)->buckets_num;i++){
        //If its not NULL delete it
        if((*ht)->table[i]!=NULL) {
            deleteKeyBucket(&(*ht)->table[i],mode);
        }
    }
    //Free the space of the HashTables Cells
    free((*ht)->table);
    (*ht)->table=NULL;
    //Free hash table space
    free(*ht);
    *ht=NULL;
}

//Function to delete the Hash Table
void cliqueDeleteHashTable(HashTable **ht,int mode){
    //Iterate through its bucket of the Hash Table
    for(int i=0;i<(*ht)->buckets_num;i++){
        if((*ht)->table[i]!=NULL) {
            //Iterate through the bucket array inside the HashTable
            for(int j=0;j<(*ht)->table[i]->num_entries;j++){
                if((*ht)->table[i]->array[j]!=NULL) {
                    if((*ht)->table[i]->array[j]->set!=NULL) {
                        //If the number of elements in the buckets is less than or 1 just delete it normally
                        if ((*ht)->table[i]->array[j]->set->num_entries <= 1) {
                            BucketList_Delete(&(*ht)->table[i]->array[j]->set, mode);
                        }
                        //Delete the bucket and set all pointers that form the clique as NULL
                        else {
                            //Set all pointers showing the clique as NULL
                            (*ht) = erasePointerHashTable(ht, (*ht)->table[i]->array[j]);
                            BucketList_Delete(&(*ht)->table[i]->array[j]->set, mode);
                        }
                    }
                    //Free the space of the key Bucket Entry
                    (*ht)->table[i]->array[j]->set = NULL;
                    free((*ht)->table[i]->array[j]->key);
                    (*ht)->table[i]->array[j]->key=NULL;
                    free((*ht)->table[i]->array[j]);
                    (*ht)->table[i]->array[j] = NULL;
                }
            }
            //Free the space of the keyBucket
            free((*ht)->table[i]->array);
            (*ht)->table[i]->array=NULL;
            free((*ht)->table[i]);
            (*ht)->table[i]=NULL;
        }
    }
    //Free the space of the Hash Table
    free((*ht)->table);
    (*ht)->table=NULL;
    free(*ht);
    *ht=NULL;
}

//Function to deleteKeyBucket
void deleteKeyBucket(keyBucket **destroyed,int mode){
    keyBucket *temp;
    temp = *destroyed;
    //Iterate through the KeyBucket
    for(int i=0;i<temp->num_entries;i++){
        if(temp->array[i]!=NULL) {
            //Free the space of the KeyBUcketEntry
            free(temp->array[i]->key);
            temp->array[i]->key=NULL;
            BucketList_Delete(&temp->array[i]->set, mode);
            temp->array[i]->set=NULL;
            free(temp->array[i]);
            temp->array[i]=NULL;
        }
    }
    //Free the array of the KeyBucketEntry
    free(temp->array);
    temp->array=NULL;
    //Free the space of the KeyBucket
    free(*destroyed);
    *destroyed = temp = NULL;
}

//Function to print hash table
void printHashTable(HashTable *ht){

    //Iterate through the hashTable
    for(int i=0;i<ht->buckets_num;i++){
        if(ht->table[i]!=NULL){
            //Iterate through the KeyBucketEntries
            for(int j=0;j<ht->table[i]->num_entries;j++) {
                printf("\nBUCKET %d %d:\n",i,j);
                if (ht->table[i]->array[j] != NULL && ht->table[i]->array[j]->set != NULL) {
                    //Iterate through the bucket list
                    Bucket *cur = ht->table[i]->array[j]->set->head;
                    while(cur!=NULL){
                        for(int l=0;l<cur->cnt;l++)
                            //Print every spec_id
                            printf("%s\n",cur->spec_ids[l]->dict_name);
                        cur = cur->next;
                    }
                }
            }
        }
    }
}

//Function to change the pointers of the list with less elements
//to the list with the most elements so the union can be performed correctly
HashTable *changePointerHashTable(HashTable **ht,keyBucketEntry *old_bucket,keyBucketEntry *new_bucket){
    Bucket *current = old_bucket->set->head;
    //Iterate through the list of buckets and hash every entry
    //The pointers of old_bucket must now point to new_bucket
    while(current!=NULL){
        for(int i=0;i<current->cnt;i++){
            int hash_v = hashCode(current->spec_ids[i]->dict_name,(*ht)->buckets_num);
            int index = findKeyBucketEntry((*ht),current->spec_ids[i]->dict_name);

            //Change the pointer of the found spec id
            if((*ht)->table[hash_v]->array[index]!=old_bucket)
                (*ht)->table[hash_v]->array[index]->set = new_bucket->set;
        }

        current = current->next;
    }

    return *ht;
}

//Function to set all pointers pointing to a certain clique as NULL
HashTable *erasePointerHashTable(HashTable **ht,keyBucketEntry *clique_bucket){

    Bucket *current = clique_bucket->set->head;
    //Iterate through the list of buckets and hash every entry
    //The pointers of old_bucket must now point to new_bucket
    while(current!=NULL){
        for(int i=0;i<current->cnt;i++){
            int hash_v = hashCode(current->spec_ids[i]->dict_name,(*ht)->buckets_num);
            int index = findKeyBucketEntry((*ht),current->spec_ids[i]->dict_name);
            //Change the pointer of the found spec id
            if(index!=-1 && (*ht)->table[hash_v]->array[index]!=clique_bucket) {
                (*ht)->table[hash_v]->array[index]->set = NULL;
            }
        }

        current = current->next;
    }

    return *ht;
}

//Function to perform union of the two spec ids
HashTable *createCliqueHashTable(HashTable **ht, char *left_sp, char *right_sp){
    //Hash the values for the two spec_ids;
    int left_h = hashCode(left_sp,(*ht)->buckets_num);
    int right_h = hashCode(right_sp,(*ht)->buckets_num);

    //Get the indexes of the hashed entries
    int left_index = findKeyBucketEntry(*ht,left_sp);
    int right_index = findKeyBucketEntry(*ht,right_sp);

    //Get the number of elements in each bucket list
    int left_cnt = (*ht)->table[left_h]->array[left_index]->set->num_entries;
    int right_cnt = (*ht)->table[right_h]->array[right_index]->set->num_entries;

    //The merge is performed accordingly to the number of elements
    //If the entries are not already on the same set unify them
    if((*ht)->table[right_h]->array[right_index]->set!=(*ht)->table[left_h]->array[left_index]->set) {

        if (left_cnt < right_cnt) {
            //keyBucketEntry *old_bucket = (*ht)->table[left_h]->array[left_index];
            (*ht) = changePointerHashTable(ht, (*ht)->table[left_h]->array[left_index],
                                           (*ht)->table[right_h]->array[right_index]);


            //Change the negative Relations, from the set of the left buck to point to the right bcuk
            (*ht)->table[left_h]->array[left_index]->set = updateNegativeRelations(
                (*ht)->table[left_h]->array[left_index]->set,
                (*ht)->table[right_h]->array[right_index]->set);

            //Merge the negative values...
            (*ht)->table[right_h]->array[right_index]->set = mergeNegativeRelations(
                (*ht)->table[right_h]->array[right_index]->set,
                &((*ht)->table[left_h]->array[left_index]->set));

            (*ht)->table[right_h]->array[right_index]->set = BucketList_Merge(
                    &(*ht)->table[right_h]->array[right_index]->set, &(*ht)->table[left_h]->array[left_index]->set, ht,
                    left_h, left_index);
        }
        else {
            //keyBucketEntry *old_bucket = (*ht)->table[right_h]->array[right_index];
            (*ht) = changePointerHashTable(ht, (*ht)->table[right_h]->array[right_index],
                                           (*ht)->table[left_h]->array[left_index]);

            
            //Change the negative Relations, from the set of the left buck to point to the right bcuk
            (*ht)->table[right_h]->array[right_index]->set = updateNegativeRelations(
                (*ht)->table[right_h]->array[right_index]->set,
                (*ht)->table[left_h]->array[left_index]->set);

            //Merge the negative values...
            (*ht)->table[left_h]->array[left_index]->set = mergeNegativeRelations(
                (*ht)->table[left_h]->array[left_index]->set,
                &((*ht)->table[right_h]->array[right_index]->set));

            (*ht)->table[left_h]->array[left_index]->set = BucketList_Merge(
                    &(*ht)->table[left_h]->array[left_index]->set, &(*ht)->table[right_h]->array[right_index]->set, ht,
                    right_h, right_index);
        }
    }
    return *ht;
}

//Function to perform set union between two sets
//Max_List : Number of entries in this bucket is greater than those in min_List
BucketList *BucketList_Merge(BucketList **Max_List, BucketList **min_List,HashTable **ht,int h,int index)
{
    //The first bucket of the bucket chain with the fewer elements is full
    //So it is pushed at the end of Max_List
    if(BucketList_Bucket_Full(*min_List)==BUCKET_FIRST_FULL)
    {

        //Increase the counter of entries in Max_List
        (*Max_List)->num_entries +=  (*min_List)->num_entries;

        //Add the minimum entries List in the end of our Main List(has greater entries)
        (*Max_List)->tail->next = (*min_List)->head;
        //Update the tail of our Main List
        (*Max_List)->tail = (*min_List)->tail;

        //Delete the pointer to the list
        destroy_secTable(&((*min_List)->negatives),ST_SOFT_DELETE_MODE);
        free(*min_List);
        
        (*ht)->table[h]->array[index]->set = *Max_List;
        
        (*ht)->table[h]->array[index]->set->negatives = (*Max_List)->negatives;

        //Change the bucket as dirty so it can be stored in the disk
        (*Max_List)->dirty_bit=1;
        //Return the updated list
        return (*Max_List);
    }
    //The first bucket of the bucket chain with the fewer elements is not full
    //the first block's entries will be inserted into Max_List and the rest of the blocks will
    //be pushed at the end of Max_List
    else
    {
        //Iterate through the first bucket of min_list and insert all of its entries into Max_List
        Bucket *temp;
        temp = (*min_List)->head;
        int cnt = temp->cnt;
        for(int i=0;i<cnt;i++)
            *Max_List = BucketList_Insert((*Max_List),temp->spec_ids[i]);

        //Delete first block of min_list
        *min_List = BucketList_Delete_First(min_List,BUCKET_SOFT_DELETE_MODE);

        //If min_list only contained one block it is no longer of use so it is deleted
        if(BucketList_Empty(*min_List)==LIST_EMPTY){
            BucketList_Delete(min_List,BUCKET_SOFT_DELETE_MODE);
            (*ht)->table[h]->array[index]->set = *Max_List;
            (*ht)->table[h]->array[index]->set->negatives = (*Max_List)->negatives;

        }
        else{

            //Increase the counter of entries in Max_List
            (*Max_List)->num_entries += (*min_List)->num_entries;

            //Add the next minimum entries List in the end of our Main List(has greater entries)
            (*Max_List)->tail->next = (*min_List)->head;

            //Update the tail of our Main List
            (*Max_List)->tail = (*min_List)->tail;

            //Delete the pointer to the list
            // free((*min_List)->negatives->table);
            free((*min_List)->negatives);
            free(*min_List);
            (*ht)->table[h]->array[index]->set = *Max_List;
            (*ht)->table[h]->array[index]->set->negatives = (*Max_List)->negatives;


        }

        
        //Change the bucket as dirty so it can be stored in the disk
        (*Max_List)->dirty_bit=1;
        //Return the updated list
        return *Max_List;
    }


}

//Function to add a negative relation between two ids
HashTable *negativeRelationHashTable(HashTable *ht, char *left_sp,char *right_sp){
    
    //Hash the left entry
    int left_h = hashCode(left_sp,ht->buckets_num);
    //Get the index for the left spec
    int left_index = findKeyBucketEntry(ht,left_sp);

    //Hash the left entry
    int right_h = hashCode(right_sp,ht->buckets_num);   
    //Get the index for the right spec
    int right_index = findKeyBucketEntry(ht,right_sp);

    //Get the pointer to the left clique    
    BucketList *left_pt = ht->table[left_h]->array[left_index]->set;
    //Get the pointer to the right clique 
    BucketList *right_pt = ht->table[right_h]->array[right_index]->set;

    //Insert the left clique's pointer into the negative set of the right clique
    ht->table[left_h]->array[left_index]->set->negatives = insert_secTable(ht->table[left_h]->array[left_index]->set->negatives,right_pt);
    //Similarly for the right clique's pointer  
    ht->table[right_h]->array[right_index]->set->negatives = insert_secTable(ht->table[right_h]->array[right_index]->set->negatives,left_pt);
    
    return ht;
}



void testCSVHashTable(char *filename, HashTable *ht){
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    size_t read;

    //Open file
    fp = fopen(filename,"r");
    //Check if file Opened
    if(fp==NULL)
    {
        errorCode = OPENING_FILE;
        fclose(fp);
        print_error();
        return;
    }

    int i=0;    //Number Of Lines Counter
    //Read each line
    while((read = getline(&line, &len,fp))!=-1){
        if(i==0){ //Skip First Line cause its Left_spec, Right_Spec, label
        
            i++;
            continue;
        }
        
        char *left_sp,*right_sp,*lbl_str;
        //Take left_spec_id
        left_sp = strtok(line,",");
        //Take right_spec_id
        right_sp = strtok(NULL,",");
        //Take label
        lbl_str = strtok(NULL,",");
        //Label to integer
        int label = atoi(lbl_str);
        
        int j = -1;
        if(label == 1) //They're the same
            j = checkPositiveAs(ht,left_sp,right_sp);        
        else if (label == 0) //Negative relation
            j = checkNegativeAs(ht,left_sp,right_sp);
        
        if(j==-1)
        {
            printf("ERROR");
            printf("\n%s\n",line);
            exit(1);
        }

        i++;    //New line Read
    }
    free(line);
    fclose(fp);
    printf("OLA GOOD\n");

}

int checkPositiveAs(HashTable *ht, char *left, char *right){
    int h = hashCode(left, ht->buckets_num);
    int l_ind = findKeyBucketEntry(ht,left);


    int r_h = hashCode(right, ht->buckets_num);
    int r_ind = findKeyBucketEntry(ht,right);
    
    BucketList *l_s = ht->table[h]->array[l_ind]->set;
    BucketList *r_s = ht->table[r_h]->array[r_ind]->set;

    if(l_s == r_s)
        return 1;
    else return -1;
}   


int checkNegativeAs(HashTable *ht, char *left, char *right){
    int h = hashCode(left, ht->buckets_num);
    int l_ind = findKeyBucketEntry(ht,left);

    int r_h = hashCode(right, ht->buckets_num);
    int r_ind = findKeyBucketEntry(ht,right);

    BucketList *l_s = ht->table[h]->array[l_ind]->set;
    BucketList *r_s = ht->table[r_h]->array[r_ind]->set;

    int l_found = find_secTable(l_s->negatives,r_s);
    int r_found = find_secTable(r_s->negatives,l_s);

    if(l_found == 1 && r_found == 1)
        return 1;    
    else return -1;
}