#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HashTable.h"
#include "CsvReader.h"
#include "BagOfWords.h"
#include "Metrics.h"
#include "DataPreprocess.h"

//Parser for finding pairs of spec_ids in the csv file
HashTable *csvParser(char *filename,HashTable **ht, int *linesRead)
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
        if(label == 1) //They're the same
            *ht = createCliqueHashTable(ht, left_sp,right_sp);
        else if (label == 0) //Negative relation
            *ht = negativeRelationHashTable((*ht),left_sp,right_sp);


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


void csvLearning(char *filename, HashTable *ht, secTable *vocabulary, int linesRead,char *bow_type,int vector_type){

    FILE *fp;
    fp = fopen(filename,"r");
    char *line = NULL;
    size_t len = 0;
    size_t read;
    int lines=0;

    //Create the model for the training
    logisticreg *regressor;
    int steps=5;
    int batches=2;
    double learning_rate=0.0008;
    regressor = create_logisticReg(vocabulary->num_elements,vector_type,steps,batches,learning_rate);
    //Initialize the metrics for the training
    LearningMetrics *metrics = init_LearningMetrics("Positive relations","Negative relations");
    int train_size = linesRead-linesRead*0.2;
    double **X = malloc(sizeof(double)*linesRead);
    int *y = malloc(sizeof(int)*linesRead);
    char **pairs = malloc(sizeof(char*)*linesRead);



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

        double *l_x = getBagOfWords(ht,vocabulary,left_sp,bow_type);
        double *r_x = getBagOfWords(ht,vocabulary,right_sp,bow_type);
        double *xi=vectorize(l_x,r_x,regressor->numofN,vector_type);

        X[lines-1]=xi;
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

    printf("\nShuffling data\n\n");
    //Shuffle the loaded data
    shuffle_data(X,y,pairs,linesRead,2);

    printf("\nStart training...\n\n");
    //Perform the training
    regressor = train_logisticRegression(regressor,X,y,train_size);

    //Get the predictions from the model
    double *pred = predict_logisticRegression(regressor,X,train_size,linesRead);

    for(int i=0;i<(linesRead-train_size);i++)
        printf("Target %d Prediction %f\n",y[train_size+i],pred[i]);

    //Print the metrics from the predictions after training
    metrics = calculate_LearningMetrics(metrics,y,pred,train_size,linesRead);
    metrics = evaluate_LearningMetrics(metrics);
    print_LearningMetrics(metrics);

    for(int i=0;i<linesRead;i++){
        free(pairs[i]);
        free(X[i]);
    }

    free(X);
    free(y);
    free(pairs);
    free(pred);
    //Save the statistics
    printStatistics(regressor,filename);

    free(line);
    fclose(fp);
    delete_logisticReg(&regressor);
    destroyLearningMetrics(&metrics);
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
    double **X = malloc(sizeof(double)*linesRead);
    int *y = malloc(sizeof(int)*linesRead);
    char **pairs = malloc(sizeof(char*)*linesRead);

    fp = fopen(filename,"r");


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

        double *l_x = getBagOfWords(ht,vocabulary,left_sp,bow_type);
        double *r_x = getBagOfWords(ht,vocabulary,right_sp,bow_type);
        double *xi=vectorize(l_x,r_x,model->numofN,vector_type);

        X[lines-1]=xi;
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
    double *pred = predict_logisticRegression(model,X,0,linesRead);

    for(int i=0;i<linesRead;i++)
        printf("Target %d Prediction %f\n",y[i],pred[i]);

    //Print the metrics from the predictions after training
    metrics = calculate_LearningMetrics(metrics,y,pred,0,linesRead);
    metrics = evaluate_LearningMetrics(metrics);
    print_LearningMetrics(metrics);

    for(int i=0;i<linesRead;i++){
        free(X[i]);
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