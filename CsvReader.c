#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "HashTable.h"
#include "CsvReader.h"
#include "BagOfWords.h"
#include "Metrics.h"
#include "DataPreprocess.h"
#include "DataLoading.h"
#include "RBtree.h"
#include "sparseVector.h"
#include "JobScheduler.h"

//Parser for finding pairs of spec_ids in the csv file
HashTable *csvParser(char *filename,HashTable **ht, int *linesRead,int *pos_num,int *neg_num)
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    size_t read;
    *linesRead = 0;

    //Open file
    fp = fopen(filename,"r");
    //Check if file Opened
    if(fp==NULL)
    {
        errorCode = OPENING_FILE;
        fclose(fp);
        print_error();
        return NULL;
    }

    int i=0;    //Number Of Lines Counter
    //Read each line
    while((read = getline(&line, &len,fp))!=-1)
    {
        if(i==0) //Skip First Line cause its Left_spec, Right_Spec, label
        {
            i++;
            continue;
        }
        (*linesRead)++;
        
        char *left_sp,*right_sp,*lbl_str;
        //Take left_spec_id
        left_sp = strtok(line,",");
        //Take right_spec_id
        right_sp = strtok(NULL,",");
        //Take label
        lbl_str = strtok(NULL,",");
        //Label to integer
        int label = atoi(lbl_str);
        //They're the same
        if(label == 1) {
            *ht = createCliqueHashTable(ht, left_sp, right_sp);
            (*pos_num)++;
        }
        //Negative relation
        else if (label == 0) {
            *ht = negativeRelationHashTable((*ht), left_sp, right_sp);
            (*neg_num)++;
        }


        i++;    //New line Read
    }

    //Free space and close file
    free(line);
    //Close the file
    fclose(fp);

    //Return num of files read
    return *ht;
}


//Function to create a new csv to write the cliques into
void csvWriteCliques(HashTable **ht){
    FILE *fp;
    //Open file to write to...
    fp = fopen("cliques.csv","w+");
    //Check if file Opened
    if(fp==NULL)
    {
        errorCode = OPENING_FILE;
        fclose(fp);
        print_error();
        return;
    }

    //Write first line into file of the line
    int err = fprintf(fp,"left_spec_id,right_spec_id\n");
    //Something went wrong while writing to the file....
    if(err<0)
    {
        errorCode = WRITING_TO_FILE;
        print_error();
    }

    //Iterate through the Hash Table
    for(int i=0;i<(*ht)->buckets_num;i++)
    {
        //If the table is not Null iterate through it
        if((*ht)->table[i]!=NULL)
        {
            //Iterate through the array of table...
            for(int j=0;j<(*ht)->table[i]->num_entries;j++)
            {
                //So dirty bit is 1 we have to write it in the file as a clique
                if((*ht)->table[i]->array[j]->set->dirty_bit==1)
                {
                    //Make the bit of the list to 0 so we dont write it again in the file
                    (*ht)->table[i]->array[j]->set->dirty_bit=0;
                    //Write the bucketList to the file
                    bucketListWriteCliques((*ht)->table[i]->array[j]->set,fp);
                }
            }
        }
    }

    //Close the file
    fclose(fp);
}

void csvWriteNegativeCliques(HashTable **ht){
    FILE *fp;
    //Open file to write to...
    fp = fopen("neg_cliques.csv","w+");
    //Check if file Opened
    if(fp==NULL)
    {
        errorCode = OPENING_FILE;
        fclose(fp);
        print_error();
        return;
    }

    //Write first line into file of the line
    int err = fprintf(fp,"left_spec_id,right_spec_id\n");
    //Something went wrong while writing to the file....
    if(err<0)
    {
        errorCode = WRITING_TO_FILE;
        print_error();
    }

    //Iterate through the Hash Table
    for(int i=0;i<(*ht)->buckets_num;i++)
    {
        //If the table is not Null iterate through it
        if((*ht)->table[i]!=NULL)
        {
            //Iterate through the array of table...
            for(int j=0;j<(*ht)->table[i]->num_entries;j++)
            {
                //Write the bucketList to the file
                if((*ht)->table[i]->array[j]->set->dirty_bit==0) {
                    (*ht)->table[i]->array[j]->set = bucketListWriteNegativeCliques((*ht)->table[i]->array[j]->set, fp);
                    (*ht)->table[i]->array[j]->set->dirty_bit = 1;
                }
            }
        }
    }

    //Close the file
    fclose(fp);
}


void csvLearning(char *filename, HashTable *ht, secTable *vocabulary, int linesRead,char *bow_type,int vector_type,int ratio){

    //Create the model for the training
    logisticreg *regressor;
    int steps=5;
    int batches=32;
    double learning_rate=0.08;
    printf("#########################\n");
    printf("Training procedure is now starting...\n\n");
    printf("Model statistics:\n\n");
    printf("-Steps = %d\n",steps);
    printf("-Batch size = %d\n",batches);
    printf("-Learning Rate = %f\n",learning_rate);
    printf("#########################\n\n");

    regressor = create_logisticReg(vocabulary->num_elements,vector_type,steps,batches,learning_rate,ratio);
    sparseVector **X=NULL;int *y=NULL;char **pairs=NULL;

    //Load data from the given file
    load_data(filename,linesRead,ht,vocabulary,regressor,bow_type,vector_type,&X,&y,&pairs);

    printf("\nShuffling data\n\n");
    //Shuffle the loaded data
    int train_size=0;
    int test_size=0;
    int valid_size=0;
    datasets *data  = split_train_test(X,y,pairs,linesRead,7,0.4,&train_size,&test_size);
    //Split the test set into half to get a validation
    // and a test set
    int old_test_size=test_size;
    valid_size = test_size/2;
    test_size = valid_size;


    printf("\nStart training...\n\n");
    //Perform the training
    sparseVector **X_train = data->X_train;
    sparseVector **X_test = data->X_test;
    sparseVector **X_valid = data->X_test+valid_size;
    char **pairs_train = data->pairs_train;
    int *y_train = data->y_train;
    int *y_test = data->y_test;
    int *y_valid = data->y_test+valid_size;


    //Get all the pairs
    csvWriteCliques(&ht);
    csvWriteNegativeCliques(&ht);

    //Initialize the scheduler
    JobScheduler *scheduler = initialize_scheduler(MAX_THREADS);
    float threshold=0.0;
    float step_value=0.1;
    while(1){
        //Train the model based on the current train set
        regressor = train_logisticRegression(regressor,X_train,y_train,train_size,scheduler);

        //Print the metrics from the predictions after training
        LearningMetrics *metrics = init_LearningMetrics("Positive relations","Negative relations");
        waitUntilJobsHaveFinished(scheduler);
        pthread_mutex_unlock(&(scheduler->locking_queue));
        
        double *pred = predict_logisticRegression(regressor,X_valid,valid_size,scheduler);
        pthread_mutex_unlock(&(scheduler->locking_queue));
        
        metrics = calculate_LearningMetrics(metrics,y_valid,pred,test_size);
        metrics = evaluate_LearningMetrics(metrics);
        print_LearningMetrics(metrics);
        free(pred);
        destroyLearningMetrics(&metrics);

        threshold += step_value;
        if(threshold>=0.5)
            break;

        //Create a binary heap to save the pairs that are above the current threshold
        printf("Resolving transitivity issues\n");
        RBtree *rbt = predict_all_pairs(regressor,threshold,ht,vocabulary,bow_type,vector_type);
        train_size = resolve_transitivity_issues(&pairs_train,&X_train,&y_train,train_size,rbt,
                                            ht,vocabulary,bow_type,vector_type,regressor);

        shuffle_data(X_train,y_train,pairs_train,train_size,7);
    }

    waitUntilJobsHaveFinished(scheduler);
    pthread_mutex_unlock(&(scheduler->locking_queue));


    //Get the predictions from the model
    printf("\n\nFinal Evaluation on test set:\n\n");
    double *pred = predict_logisticRegression(regressor,X_test,test_size,scheduler);
    //Print the metrics from the predictions after training
    LearningMetrics *metrics = init_LearningMetrics("Positive relations","Negative relations");
    metrics = calculate_LearningMetrics(metrics,y_test,pred,test_size);
    metrics = evaluate_LearningMetrics(metrics);
    print_LearningMetrics(metrics);
    destroyLearningMetrics(&metrics);

    threads_must_exit(scheduler);
    destroy_JobScheduler(&scheduler);
    //Creating file for the predictions
    csvWritePredictions(data,pred,test_size);
    //Print the statistics of the training
    printStatistics(regressor,filename,bow_type,vector_type);

    for(int i=0;i<train_size;i++) {
        destroy_sparseVector(X_train[i]);
        free(pairs_train[i]);
    }
    for(int i=0;i<old_test_size;i++) {
        destroy_sparseVector(data->X_test[i]);
        free(data->pairs_test[i]);
    }

    free(X);
    free(y);
    free(pred);
    free(y_train);
    free(X_train);
    free(pairs_train);
    data->X_train=NULL;
    data->y_train=NULL;
    data->pairs_train=NULL;
    destroy_dataset(&data);
    delete_logisticReg(&regressor);
}


//Function to get the predictions from all pairs from the data
RBtree *predict_all_pairs(logisticreg *regressor,float threshold,HashTable *ht,secTable *vocabulary,char *bow_type,int vector_type){
    FILE *fp_neg;
    //Open file to read...
    fp_neg = fopen("neg_cliques.csv","r");
    //Check if file Opened
    if(fp_neg==NULL){
        errorCode = OPENING_FILE;
        print_error();
        return NULL;
    }

    FILE *fp_pos;
    //Open file to read...
    fp_pos = fopen("cliques.csv","r");
    //Check if file Opened
    if(fp_pos==NULL){
        errorCode = OPENING_FILE;
        print_error();
        return NULL;
    }

    char *line = NULL;
    size_t len = 0;
    size_t read;

    //Create the tree structure to save the incoming pairs
    RBtree *rbt = initRB();

    int i=0;
    while((read = getline(&line, &len,fp_pos))!=-1)
    {
        if(i==0) //Skip First Line cause its Left_spec, Right_Spec, label
        {
            i++;
            continue;
        }
        
        char *str = strdup(line);
        str = strtok(str,"\n");
        char *left_sp,*right_sp;
        //Take left_spec_id
        left_sp = strtok(str,",");
        //Take right_spec_id
        right_sp = strtok(NULL,",");

        double *x_l = getBagOfWords(ht,vocabulary,left_sp,bow_type);
        double *x_r = getBagOfWords(ht,vocabulary,right_sp,bow_type);
        int sparse_size=0;
        double *x = vectorize(x_l,x_r,regressor->numofN,vector_type,&sparse_size);
        sparseVector *vx = init_sparseVector(x,regressor->numofN,sparse_size);

        //Get the predicted value
        double pred = hypothesis(regressor,vx);

        //Keep only the predictions that are above the threshold
        if(pred < threshold || (pred > 1.0-threshold)){
            predictionPair *pair = initPredictionPair(left_sp,right_sp,pred);
            rbt = insertRB(rbt,pair);
        }

        i++;
        free(x_l);
        free(x_r);
        destroy_sparseVector(vx);
        free(str);
    }

    free(line);
    line=NULL;

    i=0;
    while((read = getline(&line, &len,fp_neg))!=-1)
    {
        if(i==0) //Skip First Line cause its Left_spec, Right_Spec, label
        {
            i++;
            continue;
        }
        char *str = strdup(line);
        str = strtok(str,"\n");
        char *left_sp,*right_sp;
        //Take left_spec_id
        left_sp = strtok(str,",");
        //Take right_spec_id
        right_sp = strtok(NULL,",");

        double *x_l = getBagOfWords(ht,vocabulary,left_sp,bow_type);
        double *x_r = getBagOfWords(ht,vocabulary,right_sp,bow_type);
        int sparse_size=0;
        double *x = vectorize(x_l,x_r,regressor->numofN,vector_type,&sparse_size);
        sparseVector *vx = init_sparseVector(x,regressor->numofN,sparse_size);

        //Get the predicted value
        double pred = hypothesis(regressor,vx);

        //Keep only the predictions that are above the threshold
        if(pred < threshold || (pred > 1.0-threshold)){
            predictionPair *pair = initPredictionPair(left_sp,right_sp,pred);
            rbt = insertRB(rbt,pair);
        }

        i++;    //New line Read
        free(x_l);
        free(x_r);
        destroy_sparseVector(vx);
        free(str);

    }

    fclose(fp_neg);
    fclose(fp_pos);
    free(line);

    return rbt;
}


//Function for inference
void csvInference(char *filename, HashTable *ht, secTable *vocabulary, logisticreg *model, char *bow_type, int vector_type){
    FILE *fp;
    fp = fopen(filename,"r");
    char *line = NULL;
    size_t len = 0;
    size_t read;
    int lines=0;

    //Firstly count the lines inside the file
    int linesRead = 0;

    while((read = getline(&line, &len,fp))!=-1)
        linesRead++;

    free(line);
    line = NULL;
    //Close the file and reopen it
    fclose(fp);

    //Decrease the linesRead cause left_spec,right_spec,label line was read...
    linesRead--;

    //Initialize the metrics for the training
    LearningMetrics *metrics = init_LearningMetrics("Positive relations","Negative relations");
    sparseVector **X = malloc(sizeof(sparseVector)*linesRead);
    int *y = malloc(sizeof(int)*linesRead);
    char **pairs = malloc(sizeof(char*)*linesRead);

    fp = fopen(filename,"r");
   
    char **right_spec_array;
    char **left_spec_array; 

    right_spec_array = malloc(sizeof(char*)*linesRead);
    left_spec_array = malloc(sizeof(char*)*linesRead);


    while((read = getline(&line, &len,fp))!=-1){

        if(lines==0){ //Skip First Line cause its Left_spec, Right_Spec, label
            lines++;
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

        //Storing right_spec and left_spec to an array for printing them to file

        right_spec_array[lines - 1] = strdup(right_sp);
        left_spec_array[lines-1] = strdup(left_sp);    
    
        double *l_x = getBagOfWords(ht,vocabulary,left_sp,bow_type);
        double *r_x = getBagOfWords(ht,vocabulary,right_sp,bow_type);
        int sparse_size=0;
        double *xi=vectorize(l_x,r_x,model->numofN,vector_type,&sparse_size);
        sparseVector *v_xi = init_sparseVector(xi,model->numofN,sparse_size);

        X[lines-1]=v_xi;
        y[lines-1]=label;
        char *new_pair = malloc(strlen(left_sp)+1+strlen(right_sp)+1);
        strcpy(new_pair,left_sp);
        strcat(new_pair,",");
        strcat(new_pair,right_sp);
        pairs[lines-1]=new_pair;

        lines++;
        free(l_x);
        free(r_x);
    }

    //Get the predictions from the model
    // double *pred = malloc(sizeof(double)*linesRead);

    JobScheduler *scheduler = initialize_scheduler(MAX_THREADS);
    model->batches = 512;
    double *pred = predict_logisticRegression(model,X,linesRead,scheduler);

    threads_must_exit(scheduler);

    destroy_JobScheduler(&scheduler);
    
    
    //Creating file for the predictions
    FILE *fp2;
    fp2 = fopen("predictions.csv","w+");
    int err = fprintf(fp2,"left_sp,right_sp,label\n");
    if(err<0){
        errorCode = WRITING_TO_FILE;
        print_error();
    }
    for(int i=0;i<linesRead;i++){
        err = fprintf(fp2,"%s,%s,%f\n",left_spec_array[i],right_spec_array[i],pred[i]);
        if(err<0){
            errorCode = WRITING_TO_FILE;
            print_error();
        }
        free(left_spec_array[i]);
        free(right_spec_array[i]);
    }

    free(left_spec_array);
    free(right_spec_array);

    fclose(fp2);



    //Print the metrics from the predictions after training
    metrics = calculate_LearningMetrics(metrics,y,pred,linesRead);
    metrics = evaluate_LearningMetrics(metrics);
    print_LearningMetrics(metrics);

    for(int i=0;i<linesRead;i++){
        destroy_sparseVector(X[i]);
        free(pairs[i]);
    }
    free(pred);
    free(X);
    free(y);
    free(pairs);
    destroyLearningMetrics(&metrics);
    free(line);
    fclose(fp);

}

//Function for writing predictions to file
void csvWritePredictions(datasets *data, double *pred, int test_size){
    char **pairs_test = data->pairs_test;
    FILE *fp2;
    fp2 = fopen("predictions.csv","w+");
    int err = fprintf(fp2,"left_sp,right_sp,label\n");
    if(err<0){
        errorCode = WRITING_TO_FILE;
        print_error();
        return;
    }
    for(int i=0;i<test_size;i++){
        err = fprintf(fp2,"%s,%f\n",pairs_test[i],pred[i]);
        if(err<0){
            errorCode = WRITING_TO_FILE;
            print_error();
            return;
        }
    }
    fclose(fp2);
}