#include <stdio.h>
#include <stdlib.h>
#include "HashTable.h"
#include "CsvReader.h"
#include "RBtree.h"
#include "acutest.h"
#include "test_names.h"

//Helper Functions
#define DATASET_PATH "Test_Units/Helper_Files/test_labelled_dataset.csv"

//Helper function to find the total number of different pairs integer N can create
int num_pairs(int N){
    return N*(N-1)/2;
}

//Helper function to find the summation of different combinations between negative relations
int num_neg_pairs(int A[],int n){
    int sum=0;
    for(int i=0;i<n;i++){
        for(int j=i+1;j<n;j++) {
            sum += A[i] * A[j];
        }
    }
    return sum;
}

//Create a new int dynamically
int *create_int(int value){
    int *n = malloc(sizeof(int));
    *n = value;
    return n;
}

//Create a string with the format key.xxxx where xxxx is the value of key_num
char *create_key(int key_num){

    char value_to_str[12];
    sprintf(value_to_str, "%d", key_num);
    char *key = malloc(strlen(value_to_str)+strlen("key.")+1);
    strcpy(key,"key.");
    strcat(key,value_to_str);
    return key;

}

//Create an array of strings with one value with the format value.xxxx where xxxx is the value of value_num
char **create_value(int value_num){

    char value_to_str[12];
    sprintf(value_to_str, "%d", value_num);
    char **value_array=malloc(sizeof(char*));
    value_array[0] = malloc(strlen(value_to_str)+strlen("value.")+1);
    strcpy(value_array[0],"value.");
    strcat(value_array[0],value_to_str);
    return value_array;

}

//Create an array of strings with one value with the format value.xxxx where xxxx is the value of value_num
char **create_multiple_value(int *values,int num_values){

    char **value_array = malloc(sizeof(char *)*num_values);
    for(int i=0;i<num_values;i++) {
        char value_to_str[12];
        sprintf(value_to_str, "%d", values[i]);
        value_array[i] = malloc(strlen(value_to_str) + strlen("value.") + 1);
        strcpy(value_array[i], "value.");
        strcat(value_array[i], value_to_str);
        memset(value_to_str,0,12);
    }

    return value_array;
}

//Insert check for dictionaries
void insert_and_test_dictionary(Dictionary **dict,char *key,char **value,int values_num,int total){

    *dict = insertDictionary(*dict,key,value,values_num);
    TEST_ASSERT(sizeDictionary(*dict)==total);

    //Search for the new inserted pair
    int num_found_value=0;
    char **found_value = lookUpDictionary(*dict,key,&num_found_value);
    TEST_ASSERT(found_value!=NULL);
    TEST_ASSERT(values_num==num_found_value);
    for(int i=0;i<values_num;i++)
        TEST_ASSERT(strcmp(found_value[i],value[i])==0);
}

//Insert check for hashtable
void insert_and_test_hashtable(HashTable **ht,Dictionary *dict,int total){

    *ht = insertHashTable(ht,dict);
    TEST_ASSERT(sizeHashTable(*ht)==total);

    //Search for the inserted dictionary
    int h = hashCode(dict->dict_name,(*ht)->buckets_num);
    int index = findKeyBucketEntry(*ht,dict->dict_name);
    TEST_ASSERT( getTopKeyBucketEntry((*ht)->table[h],index) == dict);
}

//Insert check for secTable
void insert_and_test_secTable(secTable **st,void *entry,int total){

    *st = insert_secTable(*st,entry);
    TEST_ASSERT((*st)->num_elements==total);

    //Search for the inserted value
    TEST_ASSERT(find_secTable(*st,entry)==1);
}

/*################ DICTIONARY TEST UNITS ################*/


//Create an empty dictionary with a given name
void test_dictionary_create(){

    char name[5]="Dict";
    Dictionary *dict = initDictionary(name);

    TEST_ASSERT( dict != NULL );
    TEST_ASSERT( strcmp(dict->dict_name,name)==0 );
    TEST_ASSERT( sizeDictionary(dict) == 0 );

    deleteDictionary(&dict);
}


//Insert test for dictionary
void test_dictionary_insert(){

    char name[5]="Dict";
    Dictionary *dict = initDictionary(name);

    int N=5000;

    //Add N key/value pairs into the dictionary
    //The values are a single item array in this test
    for(int i=0;i<N;i++){
        char *key = create_key(i);
        char **value = create_value(i);
        insert_and_test_dictionary(&dict,key,value,1,i+1);

        //Free the allocated memory for the test key
        //The values are deleted when the dictionary is deleted
        free(key);
    }

    //Add a key/value pair with more than one pairs
    int M=250;
    int *num_array = malloc(sizeof(int)*M);
    for(int i=0;i<M;i++) num_array[i] = N+i;

    char *key = create_key(N+1);
    char **value = create_multiple_value(num_array,M);

    //Test insert with a key/value pair with multiple values
    insert_and_test_dictionary(&dict,key,value,M,N+1);


    //Free the allocated memory for the test
    free(key);
    free(num_array);
    deleteDictionary(&dict);
}

//Insert N items into the dictionary and search for R random of them
void test_dictionary_find(){

    char name[5]="Dict";
    Dictionary *dict = initDictionary(name);

    int N=3000;

    char **key = malloc(sizeof(char*)*N);
    char ***values = malloc(sizeof(char**)*N);
    int *num_values_array = malloc(sizeof(int)*N);

    //Add N key/value pairs into the dictionary
    for(int i=0;i<N;i++){
        //create the current key
        key[i] = create_key(i);

        //create a random number of values
        int M = rand()%1000+1;
        int *num_array = malloc(sizeof(int)*M);
        num_values_array[i]=M;
        for(int j=0;j<M;j++)
            num_array[j]=rand()+1;
        values[i] = create_multiple_value(num_array,M);

        dict = insertDictionary(dict,key[i],values[i],M);

        free(num_array);
    }


    int R=500;
    int *random_index_array = malloc(sizeof(int)*R);

    //Select random R indexes to perform R random searches
    for(int i=0;i<R;i++){
        //Select a random number
        int index = rand()%N;
        random_index_array[i]=index;
    }

    //Perform R searches based on the previous random pointers
    for(int i=0;i<R;i++){
        int cur = random_index_array[i];
        //Search for the pair
        int num_found_value=0;
        char **found_value = lookUpDictionary(dict,key[cur],&num_found_value);
        TEST_ASSERT(found_value!=NULL);
        TEST_ASSERT(num_values_array[cur]==num_found_value);
        for(int i=0;i<num_values_array[cur];i++)
            TEST_ASSERT(strcmp(found_value[i],values[cur][i])==0);
    }

    //Search for and item that does not exist in the dictionary
    //NULL must be returned
    int num_found_value=0;
    TEST_ASSERT(lookUpDictionary(dict,"Not_inside",&num_found_value)==NULL);


    //Free the allocated memory for the tests
    for(int i=0;i<N;i++) free(key[i]);
    free(key);
    free(random_index_array);
    free(num_values_array);
    deleteDictionary(&dict);
    free(values);
}

//Function to test the update function of te dictionary
void test_dictionary_update(){

    char name[5]="Dict";
    Dictionary *dict = initDictionary(name);

    int N=1000;

    //Add N key/value pairs into the dictionary
    //The values are a single item array in this test
    for(int i=0;i<N;i++){
        char *key = create_key(i);
        char **value = create_value(i);
        dict = insertDictionary(dict,key,value,1);

        //Free the allocated memory for the test key
        //The values are deleted when the dictionary is deleted
        free(key);
    }

    int M=50;
    int *num_array = malloc(sizeof(int)*M);
    for(int i=0;i<M;i++) num_array[i] = N+i;

    char *key = create_key(N-1);
    char **value = create_multiple_value(num_array,M);

    //update the dictionary
    updateDictionary(&dict,key,value,M);

    //Check if the update was actually made
    int num_found_value=0;
    char **found_value = lookUpDictionary(dict,key,&num_found_value);
    TEST_ASSERT(found_value!=NULL);
    TEST_ASSERT(M==num_found_value);
    for(int i=0;i<num_found_value;i++)
        TEST_ASSERT(strcmp(found_value[i],value[i])==0);


    deleteDictionary(&dict);
    free(key);
    free(num_array);
}

//Function to test the merge function of the dictionary struct
void test_dictionary_concatenate(){

    char name[5]="Dict";
    Dictionary *dict = initDictionary(name);

    int N=2000;
    int len=0;

    //Add N key/value pairs into the dictionary
    //The values are a single item array in this test
    for(int i=0;i<N;i++){
        char *key = create_key(i);
        char **value = create_value(i);
        len += strlen(value[0])+1;
        dict = insertDictionary(dict,key,value,1);

        //Free the allocated memory for the test key
        //The values are deleted when the dictionary is deleted
        free(key);
    }

    int M=50;
    int *num_array = malloc(sizeof(int)*M);
    for(int i=0;i<M;i++) num_array[i] = N+i;

    char *key = create_key(N);
    char **value = create_multiple_value(num_array,M);
    for(int i=0;i<M;i++){
        if(i<M-1)
            len += strlen(value[i])+1;
        else
            len += strlen(value[i]);
    }
    dict = insertDictionary(dict,key,value,M);

    //Merge the dictionary into a single key-value pair
    dict = concatenateAllDictionary(dict);

    TEST_ASSERT(sizeDictionary(dict)==1);
    TEST_ASSERT(dict->list->values_num==1);
    TEST_ASSERT(strlen(dict->list->value[0])==len);

    deleteDictionary(&dict);
    free(key);
    free(num_array);
}



/*############# HASHTABLE TEST UNITS ##########*/

//Create an empty hashtable
void test_hashtable_create(){

    HashTable *ht = initHashTable(TABLE_INIT_SIZE);

    TEST_ASSERT( ht!=NULL );
    TEST_ASSERT( ht->buckets_num == TABLE_INIT_SIZE);
    TEST_ASSERT( ht->table != NULL );

    deleteHashTable(&ht,BUCKET_HARD_DELETE_MODE);
}


//Insertion test for hashtable structure
void test_hashtable_insert(){

    //Create the hashtable
    HashTable *ht = initHashTable(TABLE_INIT_SIZE);

    int N=1000;

    for(int i=0;i<N;i++){
        //Create a dictionary to insert into the hashtable
        Dictionary *dict = initDictionary(dict_names[i]);
        char *key = create_key(i);
        char **value = create_value(i);
        dict = insertDictionary(dict,key,value,1);

        //Check the insertion
        insert_and_test_hashtable(&ht,dict,i+1);

        free(key);
    }

    //Initially the hashtable must have
    //one entry in every block matching the inserted spec id
    //check if that condition is true
    for(int i=0;i<ht->buckets_num;i++){
        //Only check non empty buckets
        if(ht->table[i]!=NULL){
            for(int j=0;j<ht->table[i]->num_entries;j++){
                if(ht->table[i]->array[j]!=NULL){
                    TEST_ASSERT(ht->table[i]->array[j]->set->num_entries==1);
                }
            }
        }
    }

    deleteHashTable(&ht,BUCKET_HARD_DELETE_MODE);
}

//Test for the changing size of the hashtable
void test_hashtable_reshape(){

    HashTable *ht = initHashTable(TABLE_INIT_SIZE);

    int N=1000;

    for(int i=0;i<N;i++){
        //Create a dictionary to insert into the hashtable
        Dictionary *dict = initDictionary(dict_names[i]);
        char *key = create_key(i);
        char **value = create_value(i);
        dict = insertDictionary(dict,key,value,1);
        ht = insertHashTable(&ht,dict);

        free(key);
    }

    //Create a new dictionary to perform the reshape
    Dictionary *dict = initDictionary("Dict");
    char *key = create_key(1000);
    char **value = create_value(1000);
    dict = insertDictionary(dict,key,value,1);

    //Current hashtable size must be the double of the old one
    int old_size = ht->buckets_num;
    ht = reshapeHashTable(&ht,dict);
    int new_size = findNextPrime(2*old_size);
    TEST_ASSERT(ht->buckets_num==new_size);
    TEST_ASSERT(sizeHashTable(ht)==N+1);

    free(key);
    deleteHashTable(&ht,BUCKET_HARD_DELETE_MODE);
}

//Function to test the creation cliques
void test_hashtable_cliques(){

    HashTable *ht = initHashTable(TABLE_INIT_SIZE);

    int N=1000;

    for(int i=0;i<N;i++){
        //Create a dictionary to insert into the hashtable
        Dictionary *dict = initDictionary(dict_names[i]);
        char *key = create_key(i);
        char **value = create_value(i);
        dict = insertDictionary(dict,key,value,1);
        ht = insertHashTable(&ht,dict);

        free(key);
    }

    //Call the csvParser function to read the csv test dataset and create the cliques from it accordingly
    int lines=0;
    int pos=0;
    int neg=0;
    ht = csvParser(DATASET_PATH,&ht,&lines,&pos,&neg);

    //In this test there are 5 cliques created
    //For each category we'll check the number of items in each clique
    //Food -> 10 total items
    //Negative words -> 27 total items
    //Positive words -> 32 total words
    //Transport vehicles -> 3 total items
    //Numbers -> 5 total items
    char food_key[5]="food";
    char neg_key[9]="cowardly";
    char pos_key[10]="grandiose";
    char veh_key[6]="plane";
    char num_key[4]="two";

    //Search for number of items in each clique created
    int h = hashCode(food_key,ht->buckets_num);
    int index = findKeyBucketEntry(ht,food_key);
    BucketList *food_ptr = ht->table[h]->array[index]->set;
    TEST_ASSERT( ht->table[h]->array[index]->set->num_entries == 10);

    h = hashCode(neg_key,ht->buckets_num);
    index = findKeyBucketEntry(ht,neg_key);
    BucketList *neg_ptr = ht->table[h]->array[index]->set;
    TEST_ASSERT( ht->table[h]->array[index]->set->num_entries == 27);

    h = hashCode(pos_key,ht->buckets_num);
    index = findKeyBucketEntry(ht,pos_key);
    BucketList *pos_ptr = ht->table[h]->array[index]->set;
    TEST_ASSERT( ht->table[h]->array[index]->set->num_entries == 32);

    h = hashCode(veh_key,ht->buckets_num);
    index = findKeyBucketEntry(ht,veh_key);
    BucketList *veh_ptr = ht->table[h]->array[index]->set;
    TEST_ASSERT( ht->table[h]->array[index]->set->num_entries == 3);

    h = hashCode(num_key,ht->buckets_num);
    index = findKeyBucketEntry(ht,num_key);
    BucketList *num_ptr = ht->table[h]->array[index]->set;
    TEST_ASSERT( ht->table[h]->array[index]->set->num_entries == 5);


    //Every item of a clique must point at the same bucket
    //check if that condition holds
    h = hashCode(pos_key,ht->buckets_num);
    index = findKeyBucketEntry(ht,pos_key);
    BucketList *bl = ht->table[h]->array[index]->set;

    //Iterate through the bucket list containing the clique
    //and check if every item inside the clique points at the
    //same at the same bucket_list
    Bucket *cur = ht->table[h]->array[index]->set->head;
    while(cur!=NULL){
        for(int i=0;i<cur->cnt;i++){
            h = hashCode(cur->spec_ids[i]->dict_name,ht->buckets_num);
            index = findKeyBucketEntry(ht,cur->spec_ids[i]->dict_name);
            TEST_ASSERT( ht->table[h]->array[index]->set == bl );
        }
        cur = cur->next;
    }

    //Extra testing for the negative relations
    //In this test dataset each clique is negatively related with each other clique
    //So for every clique check if it holds a negative pointer to each of the other cliques
    TEST_ASSERT(find_secTable(food_ptr->negatives,neg_ptr)==1);
    TEST_ASSERT(find_secTable(food_ptr->negatives,pos_ptr)==1);
    TEST_ASSERT(find_secTable(food_ptr->negatives,veh_ptr)==1);
    TEST_ASSERT(find_secTable(food_ptr->negatives,num_ptr)==1);
    TEST_ASSERT(find_secTable(food_ptr->negatives,food_ptr)==0);

    TEST_ASSERT(find_secTable(neg_ptr->negatives,food_ptr)==1);
    TEST_ASSERT(find_secTable(neg_ptr->negatives,pos_ptr)==1);
    TEST_ASSERT(find_secTable(neg_ptr->negatives,veh_ptr)==1);
    TEST_ASSERT(find_secTable(neg_ptr->negatives,num_ptr)==1);
    TEST_ASSERT(find_secTable(neg_ptr->negatives,neg_ptr)==0);

    TEST_ASSERT(find_secTable(pos_ptr->negatives,neg_ptr)==1);
    TEST_ASSERT(find_secTable(pos_ptr->negatives,food_ptr)==1);
    TEST_ASSERT(find_secTable(pos_ptr->negatives,veh_ptr)==1);
    TEST_ASSERT(find_secTable(pos_ptr->negatives,num_ptr)==1);
    TEST_ASSERT(find_secTable(pos_ptr->negatives,pos_ptr)==0);

    TEST_ASSERT(find_secTable(veh_ptr->negatives,neg_ptr)==1);
    TEST_ASSERT(find_secTable(veh_ptr->negatives,pos_ptr)==1);
    TEST_ASSERT(find_secTable(veh_ptr->negatives,food_ptr)==1);
    TEST_ASSERT(find_secTable(veh_ptr->negatives,num_ptr)==1);
    TEST_ASSERT(find_secTable(veh_ptr->negatives,veh_ptr)==0);

    TEST_ASSERT(find_secTable(num_ptr->negatives,neg_ptr)==1);
    TEST_ASSERT(find_secTable(num_ptr->negatives,pos_ptr)==1);
    TEST_ASSERT(find_secTable(num_ptr->negatives,veh_ptr)==1);
    TEST_ASSERT(find_secTable(num_ptr->negatives,food_ptr)==1);
    TEST_ASSERT(find_secTable(num_ptr->negatives,num_ptr)==0);


    cliqueDeleteHashTable(&ht,BUCKET_HARD_DELETE_MODE);
}

//Function to test the integrity of the file containing the pairs
void test_hashtable_write_file(){

    HashTable *ht = initHashTable(TABLE_INIT_SIZE);

    int N=1000;

    for(int i=0;i<N;i++){
        //Create a dictionary to insert into the hashtable
        Dictionary *dict = initDictionary(dict_names[i]);
        char *key = create_key(i);
        char **value = create_value(i);
        dict = insertDictionary(dict,key,value,1);
        ht = insertHashTable(&ht,dict);

        free(key);
    }

    //Call the csvParser function to read the csv test dataset and create the cliques from it accordingly
    int lines=0;
    int pos=0;
    int neg=0;
    ht = csvParser(DATASET_PATH,&ht,&lines,&pos,&neg);
    //Create the file containing the pairs
    csvWriteCliques(&ht);
    //Create file containing the negative relations
    csvWriteNegativeCliques(&ht);


    FILE *fp;

    fp = fopen("cliques.csv","r");
    TEST_ASSERT( fp != NULL );

    //The number of lines in the file must be the summation
    //of each unique pair in the clique
    //In this test there are 5 cliques created
    //For each category we'll check the number of items in each clique
    //Food -> 10 total items
    //Negative words -> 27 total items
    //Positive words -> 32 total words
    //Transport vehicles -> 3 total items
    //Numbers -> 5 total items

    //One line for the title of the csv file
    int line_num = 1;
    line_num += num_pairs(10);
    line_num += num_pairs(27);
    line_num += num_pairs(32);
    line_num += num_pairs(3);
    line_num += num_pairs(5);

    //Count the lines inside the file
    int count=0;
    for (char c = getc(fp); c != EOF; c = getc(fp)) {
        if (c == '\n')
            count = count + 1;
    }

    //Check if the lines of the created file are equal to the number of total pairs
    TEST_ASSERT( count == line_num );

    //For the negative relations file
    //In the test dataset each type clique is negative related to all the other cliques
    //thus the number of negative relations to be printed must be equal to
    //the summation of the mults between the number of elements of each different clique
    //always considering not to print duplicates

    FILE *nfp;

    nfp = fopen("neg_cliques.csv","r");
    TEST_ASSERT( nfp != NULL );
    //One line for the title of the csv file
    int cliques_array[5];
    cliques_array[0]=10;
    cliques_array[1]=27;
    cliques_array[2]=32;
    cliques_array[3]=3;
    cliques_array[4]=5;

    //Get the actual number of negative pairs
    int neg_num = num_neg_pairs(cliques_array,5);


    //Count the lines inside the file
    int neg_count=0;
    for (char c = getc(nfp); c != EOF; c = getc(nfp)) {
        if (c == '\n')
            neg_count = neg_count + 1;
    }

    //neg_count is decreased by 1 to ignore the title of the csv output
    TEST_ASSERT((neg_count-1)==neg_num);

    fclose(fp);
    remove("cliques.csv");
    fclose(nfp);
    remove("neg_cliques.csv");
    cliqueDeleteHashTable(&ht,BUCKET_HARD_DELETE_MODE);
}



/*############# SECTABLE TEST UNITS ##########*/

//Function to create three empty secTable
//One to test an empty table for strings
//One to test an empty table for storing pointers
//One to test an empty table for storing indexed word struct
void test_sectable_create(){
    //Create hashtable for strings
    secTable *st_str = create_secTable(ST_INIT_SIZE,SB_SIZE,HashString,CompareString,DeleteString,String);
    TEST_ASSERT( st_str!=NULL );
    TEST_ASSERT( st_str->numOfBuckets == ST_INIT_SIZE);
    TEST_ASSERT( st_str->table != NULL );
    TEST_ASSERT( st_str->cmpFunction != NULL );
    TEST_ASSERT( st_str->hashFunction != NULL );
    TEST_ASSERT( st_str->deleteFunction != NULL);
    TEST_ASSERT( st_str->type == String);

    //Create hash table for void pointers
    secTable *st_ptr = create_secTable(ST_INIT_SIZE,SB_SIZE,HashPointer,ComparePointer,NULL,Pointer);
    TEST_ASSERT( st_ptr!=NULL );
    TEST_ASSERT( st_ptr->numOfBuckets == ST_INIT_SIZE);
    TEST_ASSERT( st_ptr->table != NULL );
    TEST_ASSERT( st_ptr->cmpFunction != NULL );
    TEST_ASSERT( st_ptr->hashFunction != NULL );
    TEST_ASSERT( st_ptr->deleteFunction == NULL);
    TEST_ASSERT( st_ptr->type == Pointer);

    //Create hash table for indexed words
    secTable *st_indx = create_secTable(ST_INIT_SIZE,SB_SIZE,HashIndexedWord,CompareIndexedWord,DeleteIndexedWord,indxWrd);
    TEST_ASSERT( st_indx!=NULL );
    TEST_ASSERT( st_indx->numOfBuckets == ST_INIT_SIZE);
    TEST_ASSERT( st_indx->table != NULL );
    TEST_ASSERT( st_indx->cmpFunction != NULL );
    TEST_ASSERT( st_indx->hashFunction != NULL );
    TEST_ASSERT( st_indx->deleteFunction != NULL);
    TEST_ASSERT( st_indx->type == indxWrd);

    destroy_secTable(&st_str,ST_HARD_DELETE_MODE);
    destroy_secTable(&st_indx,ST_HARD_DELETE_MODE);
    destroy_secTable(&st_ptr,ST_HARD_DELETE_MODE);
}


//Function to test the insertion of new elements in a secTable
void test_sectable_insert(){

    //Create a hash table for strings for the test
    secTable *st = create_secTable(ST_INIT_SIZE,SB_SIZE,HashString,CompareString,DeleteString,String);

    int N=40000;

    for(int i=0;i<N;i++){
        char *key = create_key(i);
        insert_and_test_secTable(&st,key,i+1);
    }

    destroy_secTable(&st,ST_HARD_DELETE_MODE);
}

//Function to test the search of an items in the secTable
void test_sectable_find(){

    //Create a hash table for strings for the test
    secTable *st = create_secTable(ST_INIT_SIZE,SB_SIZE,HashString,CompareString,DeleteString,String);

    int N=40000;

    //Check for values that surely exist in the table
    for(int i=0;i<N;i++){
        char *key = create_key(i);
        st = insert_secTable(st,key);
        TEST_ASSERT(find_secTable(st,key)==1);
    }

    int M=1000;

    //Check for values that don't exist in the table
    for(int i=N;i<N+M;i++){
        char *key = create_key(i);
        TEST_ASSERT(find_secTable(st,key)==0);
        free(key);
    }

    //Do some random searches
    for(int i=0;i<M;i++){
        int random_index = rand()%(N*2);
        char *key=create_key(random_index);

        //Check if the key actually exists
        int flag=0;
        int h = st->hashFunction(key,st->numOfBuckets);
        secondaryNode *node = st->table[h];
        if(node!=NULL){
            secondaryNode *cur = node;
            //Iterate through the list and check if the value exists in a block
            while(cur!=NULL){
                for(int i=0;i<cur->num_elements;i++){
                    if(st->cmpFunction(key,cur->values[i])==1) {
                        flag = 1;
                        break;
                    }
                }
                cur = cur->next;
            }
        }

        //Test the find function with the results of the manual search
        TEST_ASSERT(find_secTable(st,key)==flag);

        free(key);
    }

    destroy_secTable(&st,ST_HARD_DELETE_MODE);
}


//Function to check the reshape function of secTable struct
void test_sectable_reshape(){

    secTable *st = create_secTable(ST_INIT_SIZE,SB_SIZE,HashString,CompareString,DeleteString,String);

    int N=1000;

    for(int i=0;i<N;i++){
        char *key = create_key(i);
        st = insert_secTable(st,key);
    }

    //Current hashtable size must be the double of the old one
    int old_size = st->numOfBuckets;
    st = reshape_secTable(&st);
    int new_size = findNextPrime(2*old_size);
    TEST_ASSERT(st->numOfBuckets=new_size);
    //Check if the actual size of the table has changed
    TEST_ASSERT(st->num_elements==N);

    destroy_secTable(&st,BUCKET_HARD_DELETE_MODE);
}


//Function to test the delete function of secTable
void test_sectable_delete(){

    //Create a hash table for strings for the test
    secTable *st = create_secTable(ST_INIT_SIZE,SB_SIZE,HashString,CompareString,DeleteString,String);

    int N=10000;

    //Insert values into the table
    for(int i=0;i<N;i++){
        char *key = create_key(i);
        st = insert_secTable(st,key);
    }

    int random_index = rand()%N;
    char *key = create_key(random_index);

    TEST_ASSERT(find_secTable(st,key)==1);
    int old_size = st->num_elements;
    st = deletevalue_secTable(st,key,ST_HARD_DELETE_MODE);
    TEST_ASSERT(find_secTable(st,key)==0);
    TEST_ASSERT(st->num_elements== old_size-1);

    free(key);
    destroy_secTable(&st,BUCKET_HARD_DELETE_MODE);
}

void test_list_create(){
    LinkedList *list = init_LinkedList();
    TEST_ASSERT(list!=NULL);
    TEST_ASSERT(list->head==NULL);
    TEST_ASSERT(list->tail==NULL);
    TEST_ASSERT(list->num_elements==0);

    destroy_LinkedList(list);
}

TEST_LIST = {
        { "dictionary_create", 	    test_dictionary_create       },
        { "dictionary_insert", 	    test_dictionary_insert       },
        { "dictionary_find",        test_dictionary_find         },
        { "dictionary_update",      test_dictionary_update       },
        { "dictionary_concatenate", test_dictionary_concatenate  },
        { "hashtable_create",       test_hashtable_create        },
        { "hashtable_insert",       test_hashtable_insert        },
        { "hashtable_reshape",      test_hashtable_reshape       },
        { "hashtable_cliques",      test_hashtable_cliques       },
        { "hashtable_write_file",   test_hashtable_write_file    },
        { "sectable_create",        test_sectable_create         },
        { "sectable_insert",        test_sectable_insert         },
        { "sectable_find",          test_sectable_find           },
        { "sectable_reshape",       test_sectable_reshape        },
        { "sectable_delete",        test_sectable_delete         },
        { "list_create",            test_list_create             },
        { NULL, NULL }
};