#include <stdio.h>
#include <stdlib.h>
#include "HashTable.h"
#include "CsvReader.h"
#include "RBtree.h"
#include "JobScheduler.h"
#include "BagOfWords.h"
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

//Helper function to create a dummy prediction pair
predictionPair *create_pair(int N){
    char value_to_str_left[16];
    sprintf(value_to_str_left,"left%d",N);
    char value_to_str_right[17];
    sprintf(value_to_str_right,"right%d",N);
    predictionPair *pair = initPredictionPair(value_to_str_left,value_to_str_right,0.0);
    return pair;
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

//Insert check for secTable
void insert_and_test_list(LinkedList *l,predictionPair *pair,int total){

    l = insert_LinkedList(l,pair);
    TEST_ASSERT(l->num_elements==total);

    //Search for the inserted value
    TEST_ASSERT(search_LinkedList(l,pair)==1);
}

//Function to return max of two values
int max(int a,int b){
    if(a>=b) return a;
    else return b;
}

//Function to return min of two values
int min(int a,int b){
    if(a<b) return a;
    else return b;
}

//Helper function to check the balance of a given red black tree
int isBalancedRBT(RBnode *node,int *maxh,int *minh){
    // Base case
    if (node == NULL)
    {
        maxh = minh = 0;
        return 1;
    }

    int lmxh=0, lmnh=0; // To store max and min heights of left subtree
    int rmxh=0, rmnh=0; // To store max and min heights of right subtree

    // Check if left subtree is balanced, also set lmxh and lmnh
    if (isBalancedRBT(node->left, &lmxh, &lmnh) == 0)
        return 0;

    // Check if right subtree is balanced, also set rmxh and rmnh
    if (isBalancedRBT(node->right, &rmxh, &rmnh) == 0)
        return 0;

    // Set the max and min heights of this node for the parent call
    *maxh = max(lmxh, rmxh) + 1;
    *minh = min(lmnh, rmnh) + 1;

    // See if this node is balanced
    if(*maxh <= 2*(*minh))
        return 1;

    return 0;
}

//Function to create a dummy vocabulary to test the resolution of transitivity issues
secTable *create_dummy_vocab(){
    secTable *vocab = create_secTable(ST_INIT_SIZE,SB_SIZE,HashIndexedWord,CompareIndexedWord,DeleteIndexedWord,indxWrd);

    for(int i=0;i<1000;i++){
        indexedWord *id = createIndexedWord(dict_names[i],i);
        id->tf_idf_mean=(double) rand()/RAND_MAX;
        id->tf=(double) rand()/RAND_MAX;
        id->idf=(double) rand()/RAND_MAX;
        vocab = insert_secTable(vocab,id);
    }

    return  vocab;
}

//Function to create a dummy model for the test
logisticreg *create_dummy_model(){
    logisticreg *model = create_logisticReg(1000,CONCAT_VECTORS,4,2,0.6,2);
    for(int i=0;i<1000;i++){
        model->vector_weights[i] =(double) rand()/RAND_MAX;
    }

    return model;
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

/*########### MISC STRUCTS TEST UNITS ################*/

//Function to test the creation of a prediction pair
void test_prediction_pair_create(){
    double M=0.7;
    predictionPair *pair = initPredictionPair("left","right",M);
    TEST_ASSERT(pair!=NULL);
    TEST_ASSERT(strcmp(pair->left_sp,"left")==0);
    TEST_ASSERT(strcmp(pair->right_sp,"right")==0);
    TEST_ASSERT(pair->pred==M);
    TEST_ASSERT(pair->corrected==-1);

    deletePredictionPair(pair);
}

//Function to test the creation of a job
void test_job_create(){
    Job *job = create_job(1,NULL,NULL);
    TEST_ASSERT(job!=NULL);
    TEST_ASSERT(job->function==NULL);
    TEST_ASSERT(job->args==NULL);

    free(job);
}

/*################ LINKEDLIST TEST UNITS ################*/

//Function to test the creation of an empty list
void test_list_create(){
    LinkedList *list = init_LinkedList();
    TEST_ASSERT(list!=NULL);
    TEST_ASSERT(list->head==NULL);
    TEST_ASSERT(list->tail==NULL);
    TEST_ASSERT(list->num_elements==0);

    destroy_LinkedList(list);
}

//Function to test the insertion of an entry in the list
void test_list_insert(){
    LinkedList *list = init_LinkedList();

    int N=1000;

    for(int i=0;i<N;i++){
        predictionPair *pair = create_pair(i);
        insert_and_test_list(list,pair,i+1);
    }

    destroy_LinkedList(list);
}

//Function to test the search function of a list
void test_list_search(){
    LinkedList *list = init_LinkedList();

    int N=1000;

    //Search for items that exist in the list
    for(int i=0;i<N;i++){
        predictionPair *pair = create_pair(i);
        list = insert_LinkedList(list,pair);
        TEST_ASSERT(search_LinkedList(list,pair)==1);
    }

    int M=1000;

    //Search for items that don't exist on the list
    for(int i=N;i<N+M;i++){
        predictionPair *pair = create_pair(i);
        TEST_ASSERT(search_LinkedList(list,pair)!=1);
        deletePredictionPair(pair);
    }

    destroy_LinkedList(list);
}

/*############### QUEUE TEST UNITS ############*/


//Function to test the creation of a queue
void test_queue_create(){
    Queue *q = initQueue();
    TEST_ASSERT(q!=NULL);
    TEST_ASSERT(q->head==NULL);

    destroyQueue(q);
}

//Function to test the push function of a queue
void test_queue_insert(){

    int N=1000;

    Queue *q = initQueue();
    Job **job_array = malloc(sizeof(Job*)*N);


    //Check if the insertion is performed correctly
    for(int i=0;i<N;i++){
        job_array[i] = create_job(i,NULL,NULL);
        pushQueue(q,job_array[i]);

        int found=0;
        QueueNode *node = q->head;
        while(node!=NULL){
            if(node->node==job_array[i]){
                found=1;
                break;
            }
            node=node->next;
        }
        TEST_ASSERT(found==1);
    }

    for(int i=0;i<N;i++)
        free(job_array[i]);
    free(job_array);
    destroyQueue(q);
}

//Function to check the pop function and the fifo property of the queue
void test_queue_pop(){
    int N=1000;

    Queue *q = initQueue();
    Job **job_array = malloc(sizeof(Job*)*N);


    //Check if the insertion is performed correctly
    for(int i=0;i<N;i++){
        job_array[i] = create_job(i,NULL,NULL);
        pushQueue(q,job_array[i]);
    }

    //check the deletion from the queue
    //since the struct is queue fifo must be true
    //so the elements must exit the queue in the same order they were inserted
    for(int i=0;i<N;i++){
        Job *job = popQueue(&q->head);
        TEST_ASSERT(job==job_array[i]);
    }

    //Queue must be empty
    TEST_ASSERT(emptyQueue(q)==1);

    for(int i=0;i<N;i++)
        free(job_array[i]);
    free(job_array);
    destroyQueue(q);
}

/*############## RED BLACK TREE TEST UNITS ##########*/


//Function to test the creation of a red black tree
void test_red_black_tree_create(){
    RBtree *rbt = initRB();
    TEST_ASSERT(rbt!=NULL);
    TEST_ASSERT(rbt->root==NULL);
    TEST_ASSERT(rbt->num_elements==0);

    destroyRB(rbt);
}


//Function to test the insertion of the tree
void test_red_black_tree_insert(){
    int N=5000;

    RBtree *rbt = initRB();


    //Test the insertion of a new entry on the red black tree
    for(int i=0;i<N;i++){
        predictionPair *pair = create_pair(i);
        pair->pred = (double) rand()/RAND_MAX;
        rbt = insertRB(rbt,pair);
        TEST_ASSERT(search_LinkedList(searchRB(rbt,pair)->l,pair)==1);
        TEST_ASSERT(rbt->num_elements==i+1);
    }

    destroyRB(rbt);
}

//Function to test the red black property of the tree
void test_red_black_tree_property(){
    int N=1000;

    RBtree *rbt = initRB();
    //Test the insertion of a new entry on the red black tree
    for(int i=0;i<N;i++){
        predictionPair *pair = create_pair(i);
        pair->pred = (double) rand()/RAND_MAX;
        rbt = insertRB(rbt,pair);
    }

    //Function to check if the tree is balanced like a red-black tree
    int maxh=0;
    int minh=0;
    TEST_ASSERT(isBalancedRBT(rbt->root,&maxh,&minh)==1);

    destroyRB(rbt);
}

//Function to check the rotations of a red black tree
void test_red_black_tree_rotations(){

    RBtree *rbt = initRB();

    /*Create a tree of the form
            Q
           /  \
          P    C
         / \
        A   B
     To test the rotations*/
    predictionPair *pair =create_pair(0);
    pair->pred=0.2;
    rbt = insertRB(rbt,pair);
    pair =create_pair(0);
    pair->pred=0.1;
    rbt = insertRB(rbt,pair);
    pair =create_pair(0);
    pair->pred=0.3;
    rbt = insertRB(rbt,pair);
    pair =create_pair(0);
    pair->pred=0.25;
    rbt = insertRB(rbt,pair);
    pair =create_pair(0);
    pair->pred=0.35;
    rbt = insertRB(rbt,pair);

    //check the first head of the tree
    TEST_ASSERT(rbt->root->l->head->entry->pred==0.2);
    TEST_ASSERT(rbt->root->left->l->head->entry->pred==0.3);
    TEST_ASSERT(rbt->root->right->l->head->entry->pred==0.1);


    //Perform a right rotation
    right_rotation(rbt,rbt->root);

    //The head of the the tree must now be P(0.3)
    TEST_ASSERT(rbt->root->l->head->entry->pred==0.3);
    TEST_ASSERT(rbt->root->left->l->head->entry->pred==0.35);
    TEST_ASSERT(rbt->root->right->l->head->entry->pred==0.2);

    //Perform a left rotation to get the tree to its starting state
    //Perform a right rotation
    left_rotation(rbt,rbt->root);
    TEST_ASSERT(rbt->root->l->head->entry->pred==0.2);
    TEST_ASSERT(rbt->root->left->l->head->entry->pred==0.3);
    TEST_ASSERT(rbt->root->right->l->head->entry->pred==0.1);

    destroyRB(rbt);
}

/*############# TEST UNIT FOR TRANSITIVITY ISSUES #######*/

void test_resolve_transitivity(){
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

    //Create dummy arguments for the function
    int vector_type = CONCAT_VECTORS;
    char bow_type[4] = "bow";
    //Get a dummy vocabulary
    secTable *vocabulary = create_dummy_vocab();
    //Get a dummy model
    logisticreg *regressor = create_dummy_model();

    //Call the csvParser function to read the csv test dataset and create the cliques from it accordingly
    int lines=0;
    int pos=0;
    int neg=0;
    ht = csvParser(DATASET_PATH,&ht,&lines,&pos,&neg);

    //Load data from the given file to create a dummy file
    sparseVector **X=NULL;int *y=NULL;char **pairs=NULL;
    load_data(DATASET_PATH,lines,ht,vocabulary,regressor,bow_type,vector_type,&X,&y,&pairs);

    int M=100;
    RBtree *rbt = initRB();
    for(int i=0;i<M;i++){
        predictionPair *pair = initPredictionPair(dict_names[rand()%1000],dict_names[rand()%1000],(double) rand()/RAND_MAX);
        rbt = insertRB(rbt,pair);
    }

    //Create a hash table corresponding to the training set data
    int temp_lines=0;
    int temp_pos=0;
    int temp_neg=0;
    HashTable *data_ht=initHashTable(TABLE_INIT_SIZE);
    //Create a hash table to resolve possible conflicts from the prediction data set
    HashTable *pred_ht=initHashTable(TABLE_INIT_SIZE);
    //Create the cliques from the training set
    pos=0;
    neg=0;
    for(int i=0;i<N;i++){
        //Create a dictionary to insert into the hashtable
        Dictionary *dict = initDictionary(dict_names[i]);
        char *key = create_key(i);
        char **value = create_value(i);
        dict = insertDictionary(dict,key,value,1);
        data_ht = insertHashTable(&data_ht,dict);
        free(key);
    }
    data_ht = csvParser(DATASET_PATH,&data_ht,&temp_lines,&temp_pos,&temp_neg);

    for(int i=0;i<N;i++){
        //Create a dictionary to insert into the hashtable
        Dictionary *dict = initDictionary(dict_names[i]);
        char *key = create_key(i);
        char **value = create_value(i);
        dict = insertDictionary(dict,key,value,1);
        pred_ht = insertHashTable(&pred_ht,dict);
        free(key);
    }
    pred_ht = csvParser(DATASET_PATH,&pred_ht,&temp_lines,&temp_pos,&temp_neg);


    //Resolve any transitivity issues that occurred
    int size = rbt->num_elements;
    char **pairs_corrected=malloc(sizeof(char*)*size);
    sparseVector **X_corrected=malloc(sizeof(sparseVector*)*size);
    int *y_corrected = malloc(sizeof(int)*size);
    resolveRB(&data_ht,&pred_ht,rbt,&pairs_corrected,&X_corrected,&y_corrected,ht,vocabulary,bow_type,vector_type,regressor,&pos,&neg);

    //Concatenate the old training set with the new pairs
    int new_size = lines + size;
    sparseVector **new_X_train = malloc(sizeof(sparseVector*)*new_size);
    int *new_y_train = malloc(sizeof(int)*new_size);
    char **new_pairs_train = malloc(sizeof(char*)*new_size);

    for(int i=0;i<lines;i++){
        new_X_train[i] = X[i];
        new_y_train[i] = y[i];
        new_pairs_train[i] = pairs[i];
    }

    for(int i=0;i<size;i++){
        new_X_train[i+lines] = X_corrected[i];
        new_y_train[i+lines] = y_corrected[i];
        new_pairs_train[i+lines] = pairs_corrected[i];
    }

    free(X);
    X = new_X_train;
    free(y);
    y = new_y_train;
    free(pairs);
    pairs = new_pairs_train;
    destroyRB(rbt);

    //Test if there are any transitivity issues
    HashTable *check_ht=initHashTable(TABLE_INIT_SIZE);
    //Create the cliques from the training set
    for(int i=0;i<N;i++){
        //Create a dictionary to insert into the hashtable
        Dictionary *dict = initDictionary(dict_names[i]);
        char *key = create_key(i);
        char **value = create_value(i);
        dict = insertDictionary(dict,key,value,1);
        check_ht = insertHashTable(&check_ht,dict);
        free(key);
    }
    check_ht = csvParser(DATASET_PATH,&check_ht,&temp_lines,&temp_pos,&temp_neg);

    for(int i=0;i<new_size;i++){
        char *line = strdup(pairs[i]);
        char *left_sp,*right_sp;
        //Take left_spec_id
        left_sp = strtok(line,",");
        //Take right_spec_id
        right_sp = strtok(NULL,",");

        int relation = checkRelation(check_ht,left_sp,right_sp);
        if(relation==-1){
            if (y[i] == 1)
                check_ht = createCliqueHashTable(&check_ht,left_sp,right_sp);
            else
                check_ht = negativeRelationHashTable(check_ht,left_sp,right_sp);
        }
        else{
            TEST_ASSERT(relation==y[i]);
        }

        free(line);
    }

    for(int i=0;i<new_size;i++){
        destroy_sparseVector(X[i]);
        free(pairs[i]);
    }

    free(pairs_corrected);
    free(X_corrected);
    free(y_corrected);
    free(y);
    free(X);
    free(pairs);
    cliqueDeleteHashTable(&data_ht,BUCKET_HARD_DELETE_MODE);
    cliqueDeleteHashTable(&pred_ht,BUCKET_HARD_DELETE_MODE);
    cliqueDeleteHashTable(&check_ht,BUCKET_HARD_DELETE_MODE);
    cliqueDeleteHashTable(&ht,BUCKET_HARD_DELETE_MODE);
    destroy_secTable(&vocabulary,ST_HARD_DELETE_MODE);
    delete_logisticReg(&regressor);

}


TEST_LIST = {
        { "dictionary_create", 	      test_dictionary_create       },
        { "dictionary_insert", 	      test_dictionary_insert       },
        { "dictionary_find",          test_dictionary_find         },
        { "dictionary_update",        test_dictionary_update       },
        { "dictionary_concatenate",   test_dictionary_concatenate  },
        { "hashtable_create",         test_hashtable_create        },
        { "hashtable_insert",         test_hashtable_insert        },
        { "hashtable_reshape",        test_hashtable_reshape       },
        { "hashtable_cliques",        test_hashtable_cliques       },
        { "hashtable_write_file",     test_hashtable_write_file    },
        { "sectable_create",          test_sectable_create         },
        { "sectable_insert",          test_sectable_insert         },
        { "sectable_find",            test_sectable_find           },
        { "sectable_reshape",         test_sectable_reshape        },
        { "sectable_delete",          test_sectable_delete         },
        { "prediction_pair_create",   test_prediction_pair_create  },
        { "job_create",               test_job_create              },
        { "list_create",              test_list_create             },
        { "list_insert",              test_list_insert             },
        { "list_search",              test_list_search             },
        { "queue_create",             test_queue_create            },
        { "queue_insert",             test_queue_insert            },
        { "queue_pop",                test_queue_pop               },
        { "red_black_tree_create",    test_red_black_tree_create   },
        { "red_black_tree_insert",    test_red_black_tree_insert   },
        { "red_black_tree_property",  test_red_black_tree_property },
        { "red_black_tree_rotations", test_red_black_tree_rotations},
        { "resolve_transitivity",     test_resolve_transitivity    },
        { NULL, NULL }
};