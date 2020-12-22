#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "SecTable.h"
#include "BagOfWords.h"


/*######################### Hash Functions ###############*/

// Used Thomas Wang's function for hashing pointers
unsigned int HashPointer(void* Ptr,int buckets){
    long Value = (long)Ptr;
    Value = ~Value + (Value << 15);
    Value = Value ^ (Value >> 12);
    Value = Value + (Value << 2);
    Value = Value ^ (Value >> 4);
    Value = Value * 2057;
    Value = Value ^ (Value >> 16);
    return Value%buckets;

}

// Used sdbm hash function to hash strings
unsigned int HashString(void* Ptr,int buckets){
    unsigned int hash = 0;
    char *str = (char*)Ptr;
    int c;
    while ((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash%buckets;
}

//Used sdbm hash function to hash strings
unsigned int HashIndexedWord(void* Ptr,int buckets){
    unsigned int hash = 0;
    indexedWord *word = (indexedWord*)Ptr;
    char *str = word->word;
    int c;
    while ((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash%buckets;
}

/*############################################################*/

/*################ Compare Functions #########################*/

//Returns 1 if pointers are the same
int ComparePointer(void *a, void *b)
{
    return a==b;
}

//Returns 1 if strings are the same
int CompareString(void * a, void *b)
{
    return (strcmp((char *)a, (char *)b) == 0);
}

//Returns 1 if strings if two indexed words are the same
int CompareIndexedWord(void * a, void *b)
{
    indexedWord *a_w = (indexedWord*)a;
    indexedWord *b_w = (indexedWord*)b;
    return (strcmp(a_w->word,b_w->word) == 0);
}

/*############################################################*/

/*############# Delete Functions #############################*/

//Function to delete strings
void DeleteString(void *Ptr){
    free(Ptr);
}

//Function to delete indexed words;
void DeleteIndexedWord(void *Ptr){
    deleteIndexedWord((indexedWord*)Ptr);
}

/*#############################################################*/


//Function to create a new secondary hash table
secTable *create_secTable(int size, int bucketSize,Hash hashFunction, Compare cmpFunction,Delete deleteFunction,Data type){

    secTable *st = malloc(sizeof(secTable));
    st->loadFactor = 0;
    st->num_elements = 0;
    st->bucketSize=bucketSize;
    st->numOfBuckets = size;
    st->hashFunction = hashFunction;
    st->cmpFunction = cmpFunction;
    st->deleteFunction = deleteFunction;
    st->type = type;
    st->table = malloc(sizeof(secondaryNode*) * st->numOfBuckets);

    // Initialize every bucket of the table as NULL
    for(int i=0;i<size;i++)
        st->table[i]=NULL;

    return st;

}

//Function to insert a new entry into the 
secTable *insert_secTable(secTable *st, void *value){

    int h = st->hashFunction(value, st->numOfBuckets);
    // If the load factor of the hash table is too high reshape the hash table
    if(st->loadFactor<0.85){
        
        if(st->table[h]==NULL){
            st->table[h] = create_secondaryNode(value,(st->bucketSize/sizeof(value)));
            //Update the number of elements
            st->num_elements++;
            // Update load factor
            st->loadFactor = st->num_elements/(st->numOfBuckets*(st->bucketSize/sizeof(value)));
        }
        else{
            int current_num = getNumElements_secondaryNode(st->table[h]);
        
            //There is enough space in the first block of the bucket insert the new entry
            if(current_num < (st->bucketSize/sizeof(value))){
                st->table[h]->values[current_num] = value;
                st->table[h]->num_elements++;
            }
            else{
                //The first block is the one containing free space in every bucket
                //If the first block is full add a new one at the start of the bucket chain
                secondaryNode *new_node = create_secondaryNode(value,(st->bucketSize/sizeof(value)));
                new_node->next = st->table[h];
                st->table[h] = new_node;
            }
            st->num_elements++;
            st->loadFactor = st->num_elements/(st->numOfBuckets*(st->bucketSize/sizeof(value)));
        }
        return st;
    }
    // Reshape the hash table with doubled size
    else{
        secTable *new_st = reshape_secTable(&st);
        new_st = insert_secTable(new_st, value);
        return new_st;
    }
    
}

//Function to replace a value from the hash table with a given new one
secTable *replace_secTable(secTable *st, void *old_value, void *new_value){
    int h = st->hashFunction(old_value, st->numOfBuckets);
    st->table[h] = deletevalue(st->table[h],old_value,st->cmpFunction,st->deleteFunction,ST_SOFT_DELETE_MODE);
    return insert_secTable(st, new_value);
}

//Function to get the next prime number
int findNextPrime(int N){
    int prime = 0;
    for(int i=N;i<MAX_PRIME;i++){
        int flag=0;
        for (int j = 2; j <= i / 2; ++j) {
            // condition for non-prime
            if (i % j == 0) {
                flag=1;
                break;
            }
        }
        if(flag==0){
            prime = i;
            break;
        }
    }
    return prime;
}

//Function for reshaping the hash table
secTable *reshape_secTable(secTable **st){
    //Find the closest prime number of the current table size*2
    int new_size = findNextPrime((*st)->numOfBuckets*2);
    //Create a new hash table with doubled the size of the previous one
    secTable *new_st = create_secTable(new_size,(*st)->bucketSize,(*st)->hashFunction,(*st)->cmpFunction,(*st)->deleteFunction,(*st)->type);
    //Iterate through the old hashtable and get every value
    //and insert it into the new hashtable
    for(int i=0;i<(*st)->numOfBuckets;i++){
        while((*st)->table[i]!=NULL){
            void *nvalue;
            (*st)->table[i] = getFirstVal((*st)->table[i],&nvalue);
            if(nvalue!=NULL)
                new_st = insert_secTable(new_st,nvalue);
        }
    }
    free((*st)->table);
    free(*st);
    return new_st;
}

//Function to print a hash table
void print_secTable(secTable *st){
    for(int i=0;i<st->numOfBuckets;i++){
        if(st->table[i]!=NULL) {
            printf("BUCKET %d:\n",i);
            print_secondaryNode(st->table[i],st->type);
        }
    }
}

//Function to print the values of a secondary block
void print_secondaryNode(secondaryNode *node,Data type){
    secondaryNode *cur = node;
    while(cur!=NULL){
        for(int i=0;i<cur->num_elements;i++){
            if(type==String) {
                if(cur->values[i]!=NULL)
                    printf("%s\n", (char *) cur->values[i]);
            }
            else if (type==Pointer) {
                if(cur->values[i]!=NULL)
                    printf("%p\n", cur->values[i]);
            }
            else if (type==indxWrd){
                if(cur->values[i]!=NULL) {
                    indexedWord *temp = (indexedWord*)cur->values[i];
                    printf("%s : %d %lf %lf\n", temp->word,temp->index,temp->idf,temp->tf_idf_mean);
                }
            }
        }
        cur = cur->next;
    }
}

//Function to get the number of items in the first block of a chain
int getNumElements_secondaryNode(secondaryNode *node){
    return node->num_elements;
}

//Function to create a new secondary node
secondaryNode *create_secondaryNode(void *value,int size){
    secondaryNode *sn = malloc(sizeof(secondaryNode));
    sn->num_elements = 1;
    sn->values = malloc(sizeof(void*) * size);
    for(int i=0;i<size;i++)
        sn->values[i]=NULL;
    sn->values[0] = value;
    sn->next = NULL;
    return sn;
}


//Function to delete a hash table
void destroy_secTable(secTable **st,int mode){
    for(int i=0;i<(*st)->numOfBuckets;i++){
        destroy_secondaryNode(&((*st)->table[i]),(*st)->deleteFunction,mode);
    }
    free((*st)->table);
    free(*st);
    *st=NULL;
}

//Function to destroy a secondary node
void destroy_secondaryNode(secondaryNode **node,Delete deleteFunction,int mode){

    secondaryNode *cur = *node;
    while(cur!=NULL){
        for(int i=0;i<cur->num_elements;i++){
            if(mode==ST_SOFT_DELETE_MODE || deleteFunction==NULL)
                cur->values[i]=NULL;
            else
                deleteFunction(cur->values[i]);
        }
        free(cur->values);
        secondaryNode *temp = cur;
        cur = cur->next;
        free(temp);
    }

    *node = NULL;
}

//Returns the first Value of the list
secondaryNode *getFirstVal(secondaryNode *node, void **value){

    //If the first block is empty delete it
    if(node->num_elements == 0){
        secondaryNode *tmp = node;
        node = node->next;
        tmp->next = NULL;
        *value = NULL;
        free(tmp->values);
        free(tmp);

        return node;
    }

    //Get the last element from the current block and return it
    node->num_elements--;
    *value =  node->values[node->num_elements];
    node->values[node->num_elements] = NULL;
    return node;

}


//Function to delete value
secTable *deletevalue_secTable(secTable *st, void *value,int mode){
    int h = st->hashFunction(value,st->numOfBuckets);
    st->table[h] = deletevalue(st->table[h],value,st->cmpFunction,st->deleteFunction,mode);
    st->num_elements--;
    return st;
}

//Function to delete value
secondaryNode *deletevalue(secondaryNode *node, void *value, Compare compareFunction,Delete deleteFunction,int mode){
    node->num_elements--;
    void *tmp_value = node->values[node->num_elements];
    node->values[node->num_elements] = NULL;
    
    if(node->num_elements == 0){
        secondaryNode *tmp = node;
        node = node->next;
        tmp->next=NULL;
        free(tmp->values);
        free(tmp);
        tmp = NULL;
    }
    
    if(compareFunction(value,tmp_value) == 1) {
        if(mode==ST_SOFT_DELETE_MODE)
            return node;
        else{
            deleteFunction(tmp_value);
            return node;
        }
    }

    secondaryNode *ptr = node;
    while (ptr!=NULL){
        for(int i=0;i<ptr->num_elements;i++){
            if(compareFunction(value,ptr->values[i]) == 1){
                if(mode==ST_HARD_DELETE_MODE)
                    deleteFunction(ptr->values[i]);
                ptr->values[i] = tmp_value;
                return node;
            }
        }
        ptr = ptr->next;
    }

    return node;
    
}

//Function to find if an item exists in the hash table
int find_secTable(secTable *st,void *value){
    int h = st->hashFunction(value,st->numOfBuckets);
    return find_secondaryNode(st->table[h],value,st->cmpFunction);
}

//Function to find if an item exists in a block chain
int find_secondaryNode(secondaryNode *node,void *value,Compare compare_func){

    //Bucket list is empty - the item cannot exist in this list
    if(node==NULL){
        return 0;
    }
    else{
        secondaryNode *cur = node;

        //Iterate through the list and check if the value exists in a block
        while(cur!=NULL){
            for(int i=0;i<cur->num_elements;i++){
                if(compare_func(value,cur->values[i])==1)
                    return 1;
            }

            cur = cur->next;
        }

        return 0;
    }

}

//Function to find if an item exists in the hash table
void *getIndexedWord_secTable(secTable *st,char *value){
    int h = HashString(value,st->numOfBuckets);
    return getIndexedWord_secondaryNode(st->table[h],value,st->cmpFunction);
}

//Function to find if an item exists in a block chain
void *getIndexedWord_secondaryNode(secondaryNode *node,char *value,Compare compare_func){
    //Bucket list is empty - the item cannot exist in this list
    if(node==NULL){
        return NULL;
    }
    else{
        secondaryNode *cur = node;

        //Iterate through the list and check if the value exists in a block
        while(cur!=NULL){
            for(int i=0;i<cur->num_elements;i++){
                if(strcmp(((indexedWord*)cur->values[i])->word,value)==0)
                    return cur->values[i];
            }

            cur = cur->next;
        }

        return NULL;
    }
}

//Function to update the current tf value of a word
secTable *updateTF_secTable(secTable *st,void *value){

    //Function was called by hashtable that does not contain indexed words
    if(st->type!=indxWrd)
        return st;

    int h = st->hashFunction(value,st->numOfBuckets);
    secondaryNode *node = st->table[h];
    //Bucket list is empty - the item cannot exist in this list
    if(node==NULL){
        return st;
    }
    else{
        secondaryNode *cur = node;

        //Iterate through the list and check if the value exists in a block
        while(cur!=NULL){
            for(int i=0;i<cur->num_elements;i++){
                if(st->cmpFunction(value,cur->values[i])==1) {
                    ((indexedWord *) cur->values[i])->tf += 1.0;
                    return st;
                }
            }

            cur = cur->next;
        }

        return st;
    }
}

//Function to update the mean of tf and the idf value
secTable *update_tf_idf_values(secTable *st,secTable *unique_words,int text_len){

    for(int i=0;i<unique_words->numOfBuckets;i++){
        secondaryNode *node = unique_words->table[i];
        while(node!=NULL){
            for(int j=0;j<node->num_elements;j++)
                st = update_tf_idf_word(st,(char *)node->values[j],text_len);

            node = node->next;
        }
    }

    return st;
}


//Function to update the mean of tf and idf value of a given word
secTable *update_tf_idf_word(secTable *st,char *value,int text_len){
    indexedWord *iw = createIndexedWord(value,-1);
    int h = st->hashFunction(iw,st->numOfBuckets);
    secondaryNode *node = st->table[h];
    //Bucket list is empty - the item cannot exist in this list
    if(node==NULL){
        deleteIndexedWord(iw);
        return st;
    }
    else{
        secondaryNode *cur = node;

        //Iterate through the list and check if the value exists in a block
        while(cur!=NULL){
            for(int i=0;i<cur->num_elements;i++){
                if(st->cmpFunction(iw,cur->values[i])==1) {
                    ((indexedWord *) cur->values[i])->idf+=1.0;
                    ((indexedWord *) cur->values[i])->tf_idf_mean += ((indexedWord *) cur->values[i])->tf/(double)text_len;
                    ((indexedWord *) cur->values[i])->tf=0.0;
                    deleteIndexedWord(iw);
                    return st;
                }
            }

            cur = cur->next;
        }
        deleteIndexedWord(iw);
        return st;
    }
}


//Function to evaluate the tf-idf mean of every word in the vocabulary
secTable *evaluate_tfidf_secTable(secTable *vocabulary,int num_texts){

    //Helping variable probably will be removed
    int valid_counter=0;

    //Create the new vocabulary
    secTable *new_vocab = create_secTable(ST_INIT_SIZE,SB_SIZE,HashIndexedWord,CompareIndexedWord,DeleteIndexedWord,indxWrd);

    //Function was called by a table that does not store indexed words
    if(vocabulary->type!=indxWrd)
        return vocabulary;

    //Iterate through every bucket and update the tfidf mean value for every word
    for(int i=0;i<vocabulary->numOfBuckets;i++){
        secondaryNode *node = vocabulary->table[i];
        while(node!=NULL){
            for(int j=0;j<node->num_elements;j++){
                // n = num_texts : the total number of texts
                // nt = ((indexedWord*)node->values[j])->idf : number of texts containing the current word
                // idf = log(n/nt);
                ((indexedWord*)node->values[j])->idf = log((double)num_texts/((indexedWord*)node->values[j])->idf);
                // tfidf = tf*idf
                // We calculate the mean by dividing with the total number of texts
                // tfidf_mean = (tf*idf)/num_texts
                ((indexedWord*)node->values[j])->tf_idf_mean = (((indexedWord*)node->values[j])->idf*((indexedWord*)node->values[j])->tf_idf_mean)/(double)num_texts;


                //Current word has a good tf-idf mean keep it
                if(((indexedWord*)node->values[j])->tf_idf_mean > MIN_TF_IDF) {
                    indexedWord *iw = createIndexedWord(((indexedWord*)node->values[j])->word,valid_counter);
                    iw->tf_idf_mean = ((indexedWord*)node->values[j])->tf_idf_mean;
                    iw->tf = ((indexedWord*)node->values[j])->tf;
                    iw->idf = ((indexedWord*)node->values[j])->idf;
                    new_vocab = insert_secTable(new_vocab,iw);
                    valid_counter++;
                }
            }


            node = node->next;
        }
    }

    //Destroy previous vocabulary
    destroy_secTable(&vocabulary,ST_HARD_DELETE_MODE);

    return new_vocab;
}


//Function to write negative Cliques
void secTable_writeNegativeCliques(secTable *st, char *left_sp, FILE *fp){
    //Iterate through every negative association
    for(int i=0; i<st->numOfBuckets; i++){
        secondaryNode *temp;
        temp = st->table[i];
        //Iterate every node of the list in the bucket
        while (temp!=NULL){
            for(int j=0; j<temp->num_elements; j++){
                //Get pointer to the bucket list
                BucketList *set = (BucketList *) (temp->values[j]);
                //It was not printed
                if(set->dirty_bit == 0){
                    rightSpecNegativeCliques(set, left_sp, fp);
                }

            }
            temp = temp->next;
        }
        
    }
}
