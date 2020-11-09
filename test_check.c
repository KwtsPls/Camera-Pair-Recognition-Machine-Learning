#include <stdio.h>
#include <stdlib.h>
#include "HashTable.h"
#include "acutest.h"

//Helper Functions

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

    int N=15000;

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

    //Free the allocated memory for the tests
    for(int i=0;i<N;i++) free(key[i]);
    free(key);
    free(random_index_array);
    free(num_values_array);
    deleteDictionary(&dict);
    free(values);
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

TEST_LIST = {
        { "dictionary_create", 	test_dictionary_create },
        { "dictionary_insert", 	test_dictionary_insert },
        { "dictionary_find",    test_dictionary_find   },
        { "hashtable_create",   test_hashtable_create  },
        { NULL, NULL }
};