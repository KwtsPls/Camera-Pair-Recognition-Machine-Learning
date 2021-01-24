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
#include "acutest.h"

//Helper Functions
#define DATASET_PATH "Test_Units/Helper_Files/test_labelled_dataset.csv"
#define FILE_PATH "Test_Units/Helper_Files/Data_Files/4444.json"

//Function to check if a string was cleaning properly
void test_string_cleaning(char *text){
    int i=0;
    while(text[i]){
        TEST_ASSERT(text[i]!='\\' && text[i+1]!='\n');
        TEST_ASSERT(ispunct(text[i])==0);
        TEST_ASSERT(isupper(text[i])==0);
        i++;
    }
}

//Function to test if stopwords,empty words and useless words were removed
void test_word_removal(char *text,secTable *stopwords){

    char *str = strdup(text);
    char *temp = str;
    char *cur;
    char empty[1];
    empty[0]=0;

    //Check if a word in the text is rid of useless words
    while((cur = strsep(&str, " "))!= NULL) {
        TEST_ASSERT(find_secTable(stopwords,cur)==0);
        TEST_ASSERT(check_utf16(cur)==0);
        TEST_ASSERT(strcmp(cur,empty)!=0);
    }
    free(temp);
}

//Function to check if the returned length is correct
void test_string_len(char *text,int len){
    int count=0;
    char *str = strdup(text);
    char *temp = str;
    char *cur;
    while((cur = strsep(&str, " "))!= NULL) {
        count++;
    }
    TEST_ASSERT(count==len);
    free(temp);
}

//Function to add the given text as an entry into the structs
void add_word_to_vocab(char *text,char *key,HashTable **ht,secTable **vocab,secTable *stopwords){
    //Initialize table for unique words
    secTable *unique_words = create_secTable(ST_INIT_SIZE,SB_SIZE,HashString,CompareString,DeleteString,String);

    int len=0;
    Dictionary *dict = initDictionary(key);
    char **array = malloc(sizeof(char*));
    char *str = strdup(text);
    array[0] = preprocess(str,stopwords,vocab,&unique_words,&len);
    dict = insertDictionary(dict,key,array,1);
    *vocab = update_tf_idf_values(*vocab,unique_words,len);
    dict = concatenateAllDictionary(dict);
    *ht = insertHashTable(ht,dict);
    free(str);
    destroy_secTable(&unique_words,ST_HARD_DELETE_MODE);
}

//Function to test the idf value of a given word
int test_idf_score(char *word,double idf,secTable *vocab,double idf_array[]){
    indexedWord *idx = getIndexedWord_secTable(vocab,word);
    TEST_ASSERT(idx->idf==idf);
    //return the words index
    idf_array[idx->index]=idx->idf;
    return idx->index;
}

//Function to test the tf-idf values for a given sentence
void test_tfidf_score(HashTable *ht,secTable *vocab,double idf_array[],char *key){

    double *bow = getBagOfWords(ht,vocab,key,"tf-idf");

    Dictionary *dict = findSpecHashTable(ht,key);

    double tf_array[6]={0.0,0.0,0.0,0.0,0.0,0.0};
    double tf_idf_array[6]={0.0,0.0,0.0,0.0,0.0,0.0};

    char *str = strdup(dict->list->value[0]);
    char *temp = str;
    char *cur;
    //Count the occurrences of each word in the sentence
    int len=0;;
    while((cur = strsep(&str, " "))!= NULL) {
        len++;
        indexedWord *idx = getIndexedWord_secTable(vocab,cur);
        tf_array[idx->index]=+1.0;
    }

    //Calculate the tf-idf for each word
    for(int i=0;i<6;i++){
        tf_array[i]=tf_array[i]/(double)len;
        tf_idf_array[i]=idf_array[i]*tf_array[i];
    }

    //Check if the tf-idf values are correct
    for(int i=0;i<6;i++) {
        TEST_ASSERT(bow[i] == tf_idf_array[i]);
    }

    free(bow);
    free(temp);
}

//Function to test the bow vector
void test_bow_vector(HashTable *ht,secTable *vocab,char *key){

    double *bow = getBagOfWords(ht,vocab,key,"bow");

    Dictionary *dict = findSpecHashTable(ht,key);

    char *str = strdup(dict->list->value[0]);
    char *temp = str;
    char *cur;
    //Count the occurrences of each word in the sentence
    double bow_array[6]={0.0,0.0,0.0,0.0,0.0,0.0};
    while((cur = strsep(&str, " "))!= NULL) {
        indexedWord *idx = getIndexedWord_secTable(vocab,cur);
        bow_array[idx->index]=+1.0;
    }


    //Check if the tf-idf values are correct
    for(int i=0;i<6;i++) {
        TEST_ASSERT(bow[i] == bow_array[i]);
    }

    free(bow);
    free(temp);
}

sparseVector *create_dummy_input(logisticreg *model,int N){
    double *xi = malloc(sizeof(double)*model->numofN);
    int len=0;
    for(int i=1;i<model->numofN;i++){
        int r = rand()%100;
        xi[i] = (double)r;
    }
    //Set some of the input's components as zero
    for(int i=0;i<N/2;i++){
        int r = rand()%(model->numofN);
        xi[r]=0.0;
    }
    xi[0]=1.0;

    for(int i=0;i<model->numofN;i++){
        if(xi[i]!=0) len++;
    }

    sparseVector *x = init_sparseVector(xi,model->numofN,len);
    return x;
}

/*###### PRE-PROCESSING TEST UNITS ########################*/

//Function to test the json parser
void test_preprocess(){
    //Text to be processed
    char *text = strdup("Does the rampant jelly rattle? An exhausted tone launches every superficial resistance... The journalist hesitates under another genocide. An emphasized arcade threads our personal reluctance. The eminent impulse speculates before the icon. A bird sleeps.");
    //Initialize table with stopwords
    secTable *stopwords = init_stopwords();
    //Initialize vocabulary
    secTable *vocab = create_secTable(ST_INIT_SIZE,SB_SIZE,HashIndexedWord,CompareIndexedWord,DeleteIndexedWord,indxWrd);
    //Initialize text's length
    int len=0;
    //Initialize table to hold unique words
    secTable *unique_words = create_secTable(ST_INIT_SIZE,SB_SIZE,HashString,CompareString,DeleteString,String);

    char *temp = text;
    text = preprocess(text,stopwords,&vocab,&unique_words,&len);

    //Check if the string was cleaned correctly
    test_string_cleaning(text);
    //Check if stopwords were removed
    test_word_removal(text,stopwords);
    //Check the cleaned sentences length
    test_string_len(text,len);
    //Check if the number of unique of words is the current number of words in the vocabulary
    TEST_ASSERT(unique_words->num_elements == vocab->num_elements);

    destroy_secTable(&vocab,ST_HARD_DELETE_MODE);
    destroy_secTable(&unique_words,ST_HARD_DELETE_MODE);
    destroy_secTable(&stopwords,ST_SOFT_DELETE_MODE);
    free(text);
    free(temp);
}

//Function to test json parsing
void test_json_parser(){

    //Initialize table with stopwords
    secTable *stopwords = init_stopwords();
    //Initialize vocabulary
    secTable *vocab = create_secTable(ST_INIT_SIZE,SB_SIZE,HashIndexedWord,CompareIndexedWord,DeleteIndexedWord,indxWrd);
    //Initialize unique words
    secTable *unique_words = create_secTable(ST_INIT_SIZE,SB_SIZE,HashString,CompareString,DeleteString,String);


    Dictionary *dict = parse_json_file(FILE_PATH,"4444.json",stopwords,&vocab);

    //Check the dictionary's size
    TEST_ASSERT(strcmp(dict->dict_name,"4444.json")==0);
    TEST_ASSERT(sizeDictionary(dict)==json_size);

    //Check if every key in the json file is inside the dictionary and it has the same number of values
    for(int i=0;i<json_size;i++){
        int num_value=0;
        char **value = lookUpDictionary(dict,json_keys[i],&num_value);
        TEST_ASSERT(value!=NULL);
        TEST_ASSERT(num_value==json_values[i]);
    }

    //Check if the value of page title is the same as in the dictionary
    char *page_title = strdup(title_value);
    char *temp = page_title;
    int len=0;
    page_title = preprocess(page_title,stopwords,&vocab,&unique_words,&len);

    len=0;
    char **value = lookUpDictionary(dict,json_keys[0],&len);
    TEST_ASSERT(strcmp(value[0],page_title)==0);

    destroy_secTable(&stopwords,ST_SOFT_DELETE_MODE);
    destroy_secTable(&vocab,ST_HARD_DELETE_MODE);
    destroy_secTable(&unique_words,ST_HARD_DELETE_MODE);
    deleteDictionary(&dict);
    free(temp);
    free(page_title);
}


//Function to test the reduction of the vocabulary based on the mean tf-idf score of every word
void test_word_cutting(){
    //To run the test properly set MIN_TF_IDF constant less than 0.01

    //Initialize vocabulary
    secTable *vocab = create_secTable(ST_INIT_SIZE,SB_SIZE,HashIndexedWord,CompareIndexedWord,DeleteIndexedWord,indxWrd);

    //Insert some entries into the vocabulary
    indexedWord *idx = createIndexedWord("a",0);
    idx->idf=0.005;
    idx->tf=0.001;
    vocab = insert_secTable(vocab,idx);
    idx = createIndexedWord("b",1);
    idx->idf=0.007;
    idx->tf=0.002;
    vocab = insert_secTable(vocab,idx);
    idx = createIndexedWord("c",2);
    idx->idf=1.5;
    idx->tf=1.4;
    vocab = insert_secTable(vocab,idx);
    idx = createIndexedWord("d",3);
    idx->idf=0.1;
    idx->tf=0.12;
    vocab = insert_secTable(vocab,idx);
    idx = createIndexedWord("d",4);
    idx->idf=0.0005;
    idx->tf=0.0014;
    vocab = insert_secTable(vocab,idx);

    //Run the function
    int old_size = vocab->num_elements;
    vocab = evaluate_tfidf_secTable(vocab,5,100);

    //Test the new size of the table
    TEST_ASSERT(vocab->num_elements < old_size);

    //Check if every entry in the table is less than MIN_TF_IDF
    for(int i=0;i<vocab->numOfBuckets;i++){
        secondaryNode *node = vocab->table[i];
        while(node!=NULL){
            for(int j=0;j<node->num_elements;j++){
                indexedWord *cur = (indexedWord*)node->values[j];
                TEST_ASSERT(cur->tf_idf_mean < MIN_TF_IDF);
            }
        }
    }

    destroy_secTable(&vocab,ST_HARD_DELETE_MODE);
}


/*####### VECTOR TEST UNITS ##################*/

//Function to test tf-idf vectors
void test_tf_idf(){

    //Sentences as given in our homework
    char *text1 = "the quick fox jumped";
    char *text2 = "the quick cat sat";
    char *text3 = "the dog jumped at the fox";

    double texts_num=3.0;

    //Hashtable containing the cliques
    HashTable *ht = initHashTable(TABLE_INIT_SIZE);
    //Hashtable for the vocabulary
    secTable *vocabulary = create_secTable(ST_INIT_SIZE,SB_SIZE,HashIndexedWord,CompareIndexedWord,DeleteIndexedWord,indxWrd);
    //Initialize stopwords
    secTable *stopwords = init_stopwords();


    add_word_to_vocab(text1,"a",&ht,&vocabulary,stopwords);
    add_word_to_vocab(text2,"b",&ht,&vocabulary,stopwords);
    add_word_to_vocab(text3,"c",&ht,&vocabulary,stopwords);

    //Get the updated vocabulary
    vocabulary = evaluate_tfidf_secTable(vocabulary,sizeHashTable(ht),100);

    //Check the idf for every word
    //After the data cleaning the words remaining should be
    // quick,fox,cat,sat,jumped,dog
    double idf_array[6]={0.0,0.0,0.0,0.0,0.0,0.0};
    test_idf_score("cat",log10(texts_num/1.0),vocabulary,idf_array);
    test_idf_score("sat",log10(texts_num/1.0),vocabulary,idf_array);
    test_idf_score("dog",log10(texts_num/1.0),vocabulary,idf_array);
    test_idf_score("jumped",log10(texts_num/2.0),vocabulary,idf_array);
    test_idf_score("quick",log10(texts_num/2.0),vocabulary,idf_array);
    test_idf_score("fox",log10(texts_num/2.0),vocabulary,idf_array);

    //Test the tf-idf values for every sentence
    test_tfidf_score(ht,vocabulary,idf_array,"a");
    test_tfidf_score(ht,vocabulary,idf_array,"b");
    test_tfidf_score(ht,vocabulary,idf_array,"c");

    deleteHashTable(&ht,BUCKET_HARD_DELETE_MODE);
    destroy_secTable(&vocabulary,ST_HARD_DELETE_MODE);
    destroy_secTable(&stopwords,ST_SOFT_DELETE_MODE);
}


//Function to test simple bag of words
void test_bow(){
    //Sentences as given in our homework
    char *text1 = "the quick fox jumped";
    char *text2 = "the quick cat sat";
    char *text3 = "the dog jumped at the fox";

    //Hashtable containing the cliques
    HashTable *ht = initHashTable(TABLE_INIT_SIZE);
    //Hashtable for the vocabulary
    secTable *vocabulary = create_secTable(ST_INIT_SIZE,SB_SIZE,HashIndexedWord,CompareIndexedWord,DeleteIndexedWord,indxWrd);
    //Initialize stopwords
    secTable *stopwords = init_stopwords();

    add_word_to_vocab(text1,"a",&ht,&vocabulary,stopwords);
    add_word_to_vocab(text2,"b",&ht,&vocabulary,stopwords);
    add_word_to_vocab(text3,"c",&ht,&vocabulary,stopwords);

    test_bow_vector(ht,vocabulary,"a");
    test_bow_vector(ht,vocabulary,"b");
    test_bow_vector(ht,vocabulary,"c");

    deleteHashTable(&ht,BUCKET_HARD_DELETE_MODE);
    destroy_secTable(&vocabulary,ST_HARD_DELETE_MODE);
    destroy_secTable(&stopwords,ST_SOFT_DELETE_MODE);
}


//Function to test concatenated vectors
void test_concat(){

    int N=1000;

    double *x1 = malloc(sizeof(double)*N);
    double *x2 = malloc(sizeof(double)*N);

    //Randomly initialize the arrays
    for(int i=0;i<N;i++){
        int r = rand()%RAND_MAX;
        x1[i]=(double)r;
        r = rand()%RAND_MAX;
        x2[i]=(double)r;
    }

    //Create the vector
    int sparse_size=0;
    double *x = concatenate_vectors(x1,x2,(N*2)+1,&sparse_size);

    //First entry must be 1.0 for the bias (weight w0)
    TEST_ASSERT(x[0]==1.0);

    //Check if the first half of the array is equal to the first array
    //Check the second half as well
    for(int i=0;i<N;i++){
        TEST_ASSERT(x[i+1]==x1[i]);
        TEST_ASSERT(x[N+1+i]==x2[i]);
    }

    free(x1);
    free(x2);
    free(x);
}


//Function to check absolute vectors
void test_absolute(){

    int N=1000;

    double *x1 = malloc(sizeof(double)*N);
    double *x2 = malloc(sizeof(double)*N);

    //Randomly initialize the arrays
    for(int i=0;i<N;i++){
        int r = rand()%RAND_MAX;
        x1[i]=(double)r;
        r = rand()%RAND_MAX;
        x2[i]=(double)r;
    }

    //Create the vector
    int sparse_size=0;
    double *x = absolute_distance(x1,x2,N+1,&sparse_size);

    //First entry must be 1.0 for the bias (weight w0)
    TEST_ASSERT(x[0]==1.0);

    //Check if the first half of the array is equal to the first array
    //Check the second half as well
    for(int i=0;i<N;i++){
        TEST_ASSERT(x[i+1]==fabs(x1[i]-x2[i]));
    }

    free(x1);
    free(x2);
    free(x);
}


/*######### MACHINE LEARNING TEST UNITS ###############*/

//Function to test the creation of a new model
void test_create_model(){

    int N=1000;

    //Create a model for logistic regression for vector with absolute distance
    logisticreg *model = create_logisticReg(N,ABSOLUTE_DISTANCE,5,4,0.01,1);
    TEST_ASSERT(model!=NULL);
    TEST_ASSERT(model->numofN==N+1);
    TEST_ASSERT(model->steps==5);
    TEST_ASSERT(model->batches==4);
    TEST_ASSERT(model->learning_rate==0.01);
    for(int i=0;i<model->numofN;i++)
        TEST_ASSERT(model->vector_weights[i]==0.0);

    //Create a model for logistic regression for vector with concatenated vectors
    logisticreg *model_concat = create_logisticReg(N,CONCAT_VECTORS,5,4,0.01,1);
    TEST_ASSERT(model_concat!=NULL);
    TEST_ASSERT(model_concat->numofN==(N*2)+1);
    TEST_ASSERT(model_concat->steps==5);
    TEST_ASSERT(model_concat->batches==4);
    TEST_ASSERT(model_concat->learning_rate==0.01);
    for(int i=0;i<model_concat->numofN;i++)
        TEST_ASSERT(model_concat->vector_weights[i]==0.0);


    delete_logisticReg(&model);
    delete_logisticReg(&model_concat);
}

//Function to test the training of the test
void test_train_model(){

    int N=1000;
    int M=500;

    //Create a model for logistic regression for vector with concatenated vectors
    logisticreg *model = create_logisticReg(N,CONCAT_VECTORS,5,4,0.75,1);

    //Create a dummy input for the model
    sparseVector **X = malloc(sizeof(sparseVector*)*M);
    int *y = malloc(sizeof(int)*M);
    sparseVector *xi = create_dummy_input(model,N);
    for(int i=0;i<M;i++){
        X[i]=xi;
        y[i]=0;
    }

    //Perform a very basic training
    JobScheduler *scheduler = initialize_scheduler(MAX_THREADS);
    model = train_logisticRegression(model,X,y,M,scheduler);
    //waitUntilJobsHaveFinished(scheduler);
    pthread_mutex_lock(&(scheduler->locking_queue));
    threads_must_exit(scheduler);
    destroy_JobScheduler(&scheduler);

    //Check if the correct weights were updated
    int sparse_index=0;
    for(int i=0;i<model->numofN;i++){
        if((sparse_index = find_index_sparseVector(xi,i))!=-1) {
            TEST_ASSERT(model->vector_weights[i] != 0.0);
        }
        else
            TEST_ASSERT(model->vector_weights[i]==0.0);
    }

    destroy_sparseVector(xi);
    free(X);
    free(y);
    delete_logisticReg(&model);
}

//Function to test the predictions of the model
void test_predict_model(){
    int N=1000;
    int M=500;

    //Create a model for logistic regression for vector with concatenated vectors
    logisticreg *model = create_logisticReg(N,CONCAT_VECTORS,15,4,0.75,1);

    //Create a dummy input for the model
    sparseVector **X = malloc(sizeof(double*)*M);
    int *y = malloc(sizeof(int)*M);
    sparseVector *xi = create_dummy_input(model,N);
    for(int i=0;i<M;i++){
        X[i]=xi;
        y[i]=0;
    }


    //Perform a very basic training
    JobScheduler *scheduler = initialize_scheduler(MAX_THREADS);
    model = train_logisticRegression(model,X,y,M,scheduler);
    // pthread_mutex_lock(&(scheduler->locking_queue));

    sparseVector **X_test = malloc(sizeof(double*)*20);
    sparseVector *xi_test = create_dummy_input(model,N);
    for(int i=0;i<20;i++){
        X_test[i]=xi_test;
    }
    //Predict some results after the training
    double *y_pred = predict_logisticRegression(model,X_test,20,scheduler);
    threads_must_exit(scheduler);
    destroy_JobScheduler(&scheduler);
    //check that the predictions are between 0 and 1
    for(int i=0;i<20;i++)
        TEST_ASSERT(y_pred[i]>=0.0 && y_pred[i]<=1.0);

    destroy_sparseVector(xi);
    destroy_sparseVector(xi_test);
    free(X_test);
    free(X);
    free(y);
    free(y_pred);
    delete_logisticReg(&model);
}

TEST_LIST = {
        { "preprocess",    test_preprocess    },
        { "json_parser",   test_json_parser   },
        { "word_cutting",  test_word_cutting  },
        { "tf_idf_test",   test_tf_idf        },
        { "bow_test",      test_bow           },
        { "concat_test",   test_concat        },
        { "absolute_test", test_absolute      },
        { "create_model",  test_create_model  },
        { "train_model",   test_train_model   },
        { "predict_model", test_predict_model },
        { NULL, NULL }
};