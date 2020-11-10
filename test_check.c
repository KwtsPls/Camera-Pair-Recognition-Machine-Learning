#include <stdio.h>
#include <stdlib.h>
#include "HashTable.h"
#include "CsvReader.h"
#include "acutest.h"
#include "test_names.h"

//Helper Functions
#define DATASET_PATH "Test_Units/Helper_Files/test_labelled_dataset.csv"

//Helper function to find the total number of different pairs integer N can create
int num_pairs(int N){
    return N*(N-1)/2;
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

    int N=10000;

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
    TEST_ASSERT(ht->buckets_num==2*old_size);
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
    ht = csvParser(DATASET_PATH,&ht);

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
    TEST_ASSERT( ht->table[h]->array[index]->set->num_entries == 10);

    h = hashCode(neg_key,ht->buckets_num);
    index = findKeyBucketEntry(ht,neg_key);
    TEST_ASSERT( ht->table[h]->array[index]->set->num_entries == 27);

    h = hashCode(pos_key,ht->buckets_num);
    index = findKeyBucketEntry(ht,pos_key);
    TEST_ASSERT( ht->table[h]->array[index]->set->num_entries == 32);

    h = hashCode(veh_key,ht->buckets_num);
    index = findKeyBucketEntry(ht,veh_key);
    TEST_ASSERT( ht->table[h]->array[index]->set->num_entries == 3);

    h = hashCode(num_key,ht->buckets_num);
    index = findKeyBucketEntry(ht,num_key);
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
    ht = csvParser(DATASET_PATH,&ht);
    //Create the file containing the pairs
    csvWriteCliques(&ht);

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

    fclose(fp);
    remove("cliques.csv");
    cliqueDeleteHashTable(&ht,BUCKET_HARD_DELETE_MODE);
}


TEST_LIST = {
        { "dictionary_create", 	  test_dictionary_create    },
        { "dictionary_insert", 	  test_dictionary_insert    },
        { "dictionary_find",      test_dictionary_find      },
        { "dictionary_update",    test_dictionary_update    },
        { "hashtable_create",     test_hashtable_create     },
        { "hashtable_insert",     test_hashtable_insert     },
        { "hashtable_reshape",    test_hashtable_reshape    },
        { "hashtable_cliques",    test_hashtable_cliques    },
        { "hashtable_write_file", test_hashtable_write_file },
        { NULL, NULL }
};