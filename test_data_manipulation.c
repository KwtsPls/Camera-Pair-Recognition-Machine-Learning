#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "HashTable.h"
#include "CsvReader.h"
#include "DataPreprocess.h"
#include "JsonParser.h"
#include "BagOfWords.h"
#include "LogisticRegression.h"
#include "test_names.h"
#include "DataLoading.h"
#include "acutest.h"

//Helper Functions
#define DATASET_PATH "Test_Units/Helper_Files/test_labelled_dataset.csv"
#define FILE_PATH "Test_Units/Helper_Files/Data_Files/4444.json"

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

/*########## TEST UNITS FOR DATA ##############*/


//Function to test the creation of the random int
void test_random_int(){
    int r = rand();
    int a = random_int(r);
    int b = random_int(r);
    TEST_ASSERT(a!=b);
}

//Function to create an empty dataset
void test_dataset_create(){
    datasets *data = init_dataset();
    TEST_ASSERT(data!=NULL);
    TEST_ASSERT(data->X_train==NULL);
    TEST_ASSERT(data->X_test==NULL);
    TEST_ASSERT(data->y_train==NULL);
    TEST_ASSERT(data->y_test==NULL);
    TEST_ASSERT(data->pairs_train==NULL);
    TEST_ASSERT(data->pairs_test==NULL);
    destroy_dataset(&data);
}

//Function to load data from a given csv file
void test_load_data(){
    //Initialize the hashtable with the dictionary names
    int N=1000;
    HashTable *ht = initHashTable(TABLE_INIT_SIZE);
    for(int i=0;i<N;i++){
        //Create a dictionary to insert into the hashtable
        Dictionary *dict = initDictionary(dict_names[i]);
        char *key = create_key(i);
        char **value = create_value(i);
        dict = insertDictionary(dict,key,value,1);
        ht = insertHashTable(&ht,dict);
        free(key);
    }
    int linesRead=0;
    int pos=0;
    int neg=0;
    ht = csvParser(DATASET_PATH,&ht,&linesRead,&pos,&neg);

    //Initialize a dummy vocabulary
    secTable *vocabulary = create_dummy_vocab();
    //Initialize a dummy model
    logisticreg *model = create_dummy_model();
    char bow_type[4]="bow";
    int vector_type=CONCAT_VECTORS;

    sparseVector **X=NULL;int *y=NULL;char **pairs=NULL;

    //Call the function to load the set
    load_data(DATASET_PATH,linesRead,ht,vocabulary,model,bow_type,vector_type,&X,&y,&pairs);

    FILE *fp;
    fp = fopen(DATASET_PATH,"r");
    char *line = NULL;
    size_t len = 0;
    size_t read;
    int lines=0;

    //Run through the file and check if the data was loaded correctly
    while((read = getline(&line, &len,fp))!=-1){

        if(lines==0){ //Skip First Line cause its Left_spec, Right_Spec, label
            lines++;
            continue;
        }

        char *left_sp,*right_sp,*lbl_str;
        char *cur_left,*cur_right;
        //Take left_spec_id
        left_sp = strtok(line,",");
        //Take right_spec_id
        right_sp = strtok(NULL,",");
        //Take label
        lbl_str = strtok(NULL,",");
        //Label to integer
        int label = atoi(lbl_str);

        char *cur_line=strdup(pairs[lines-1]);
        cur_left = strtok(cur_line,",");
        cur_right = strtok(NULL,",");

        double *l_x = getBagOfWords(ht,vocabulary,left_sp,bow_type);
        double *r_x = getBagOfWords(ht,vocabulary,right_sp,bow_type);
        int sparse_size=0;
        double *xi=vectorize(l_x,r_x,model->numofN,vector_type,&sparse_size);
        sparseVector *v_xi = init_sparseVector(xi,model->numofN,sparse_size);

        TEST_ASSERT(strcmp(left_sp,cur_left)==0);
        TEST_ASSERT(strcmp(right_sp,cur_right)==0);
        TEST_ASSERT(label==y[lines-1]);
        for(int i=0;i<v_xi->num_elements;i++){
            TEST_ASSERT(v_xi->index_array[i]==X[lines-1]->index_array[i]);
            TEST_ASSERT(v_xi->concentrated_matrix[i]==X[lines-1]->concentrated_matrix[i]);
        }

        lines++;
        free(l_x);
        free(r_x);
        free(cur_line);
        destroy_sparseVector(v_xi);
    }

    for(int i=0;i<linesRead;i++){
        destroy_sparseVector(X[i]);
        free(pairs[i]);
    }
    free(line);
    free(X);
    free(pairs);
    free(y);
    fclose(fp);
    cliqueDeleteHashTable(&ht,BUCKET_HARD_DELETE_MODE);
    destroy_secTable(&vocabulary,ST_HARD_DELETE_MODE);
    delete_logisticReg(&model);
}

//Function to test the function to shuffles given data
void test_shuffle_data(){
    //Initialize the hashtable with the dictionary names
    int N=1000;
    HashTable *ht = initHashTable(TABLE_INIT_SIZE);
    for(int i=0;i<N;i++){
        //Create a dictionary to insert into the hashtable
        Dictionary *dict = initDictionary(dict_names[i]);
        char *key = create_key(i);
        char **value = create_value(i);
        dict = insertDictionary(dict,key,value,1);
        ht = insertHashTable(&ht,dict);
        free(key);
    }
    int linesRead=0;
    int pos=0;
    int neg=0;
    ht = csvParser(DATASET_PATH,&ht,&linesRead,&pos,&neg);

    //Initialize a dummy vocabulary
    secTable *vocabulary = create_dummy_vocab();
    //Initialize a dummy model
    logisticreg *model = create_dummy_model();
    char bow_type[4]="bow";
    int vector_type=CONCAT_VECTORS;

    sparseVector **X=NULL;int *y=NULL;char **pairs=NULL;
    sparseVector **X_check=NULL;int *y_check=NULL;char **pairs_check=NULL;

    //Call the function to load the set
    load_data(DATASET_PATH,linesRead,ht,vocabulary,model,bow_type,vector_type,&X,&y,&pairs);
    load_data(DATASET_PATH,linesRead,ht,vocabulary,model,bow_type,vector_type,&X_check,&y_check,&pairs_check);

    //Shuffle the data
    shuffle_data(X,y,pairs,linesRead,8);

    int counter=0;
    for(int i=0;i<linesRead;i++){
        if(strcmp(pairs[i],pairs_check[i])!=0)
            counter++;
    }
    TEST_ASSERT(counter!=0);

    //Check if the data has changed
    for(int i=0;i<linesRead;i++){
        destroy_sparseVector(X[i]);
        free(pairs[i]);
        destroy_sparseVector(X_check[i]);
        free(pairs_check[i]);
    }
    free(X);
    free(pairs);
    free(y);
    free(X_check);
    free(pairs_check);
    free(y_check);
    cliqueDeleteHashTable(&ht,BUCKET_HARD_DELETE_MODE);
    destroy_secTable(&vocabulary,ST_HARD_DELETE_MODE);
    delete_logisticReg(&model);
}

//Function to test the splitting of data into train and test sets
void test_split_and_train(){
    //Initialize the hashtable with the dictionary names
    int N=1000;
    HashTable *ht = initHashTable(TABLE_INIT_SIZE);
    for(int i=0;i<N;i++){
        //Create a dictionary to insert into the hashtable
        Dictionary *dict = initDictionary(dict_names[i]);
        char *key = create_key(i);
        char **value = create_value(i);
        dict = insertDictionary(dict,key,value,1);
        ht = insertHashTable(&ht,dict);
        free(key);
    }
    int linesRead=0;
    int pos=0;
    int neg=0;
    ht = csvParser(DATASET_PATH,&ht,&linesRead,&pos,&neg);

    //Initialize a dummy vocabulary
    secTable *vocabulary = create_dummy_vocab();
    //Initialize a dummy model
    logisticreg *model = create_dummy_model();
    char bow_type[4]="bow";
    int vector_type=CONCAT_VECTORS;

    sparseVector **X=NULL;int *y=NULL;char **pairs=NULL;

    //Call the function to load the set
    load_data(DATASET_PATH,linesRead,ht,vocabulary,model,bow_type,vector_type,&X,&y,&pairs);

    float split_value=0.3;
    //Set the size of the train set
    int n_train = (int)(((float)linesRead)*(1.0-split_value));
    //Set the size of the test set
    int n_test = linesRead - n_train;

    int train_size=0;
    int test_size=0;

    //Split the data without shuffling them to check the validity of the splitting
    datasets *data=split_train_test(X,y,pairs,linesRead,0,split_value,&train_size,&test_size);

    TEST_ASSERT(data!=NULL);
    TEST_ASSERT(data->X_train!=NULL);
    TEST_ASSERT(data->X_test!=NULL);
    TEST_ASSERT(data->y_train!=NULL);
    TEST_ASSERT(data->y_test!=NULL);
    TEST_ASSERT(data->pairs_train!=NULL);
    TEST_ASSERT(data->pairs_test!=NULL);
    TEST_ASSERT(n_train==train_size);
    TEST_ASSERT(n_test==test_size);

    //Check if the data is the same in the new sets
    for(int i=0;i<train_size;i++){
        TEST_ASSERT(X[i]==data->X_train[i]);
        TEST_ASSERT(y[i]==data->y_train[i]);
    }
    for(int i=0;i<test_size;i++){
        TEST_ASSERT(X[train_size+i]==data->X_test[i]);
        TEST_ASSERT(y[train_size+i]==data->y_test[i]);
    }

    //Check if the data has changed
    for(int i=0;i<train_size;i++){
        destroy_sparseVector(data->X_train[i]);
        free(data->pairs_train[i]);
    }
    for(int i=0;i<test_size;i++) {
        destroy_sparseVector(data->X_test[i]);
        free(data->pairs_test[i]);
    }
    free(X);
    free(y);
    destroy_dataset(&data);
    cliqueDeleteHashTable(&ht,BUCKET_HARD_DELETE_MODE);
    destroy_secTable(&vocabulary,ST_HARD_DELETE_MODE);
    delete_logisticReg(&model);
}

TEST_LIST = {
        { "random_int",      test_random_int       },
        { "dataset_create",  test_dataset_create   },
        { "load_data",       test_load_data        },
        { "shuffle_data",    test_shuffle_data     },
        { "split_and_train", test_split_and_train  },
        { NULL, NULL }
};

