#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HashTable.h"
#include "CsvReader.h"
#include "BagOfWords.h"


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


void csvLearning(char *filename, HashTable *ht, secTable *vocabulary, int linesRead){
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    size_t read;


    logisticreg *regressor;
    regressor = create_logisticReg(vocabulary->num_elements+1);
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

    // int olakala = 0;
    int lines = 0;
    int i=0;    //Number Of Lines Counter

    int train_size = linesRead-linesRead*0.1;
    int test_size = linesRead-linesRead*0.9;
    printf("%d %d %d %d\n",train_size,test_size,train_size+test_size,linesRead);

    //double **X_train = malloc(sizeof(double *)*train_size);
    //int *y_train = malloc(sizeof(int) *train_size);

    double **X_train;
    int *y_train;
    double **X_test;
    int *y_test;

    //double **X_test = malloc(sizeof(double*)*(test_size+1));
    //int *y_test = malloc(sizeof(int)*(test_size+1)); 

    double **X = malloc(sizeof(double*)*linesRead);
    int *y = malloc(sizeof(int)*linesRead);    

    //Read each line
    while((read = getline(&line, &len,fp))!=-1)
    {
        if(i==0) //Skip First Line cause its Left_spec, Right_Spec, label
        {
            i++;
            continue;
        }
        lines++;


        char *left_sp,*right_sp,*lbl_str;
        //Take left_spec_id
        left_sp = strtok(line,",");
        //Take right_spec_id
        right_sp = strtok(NULL,",");
        //Take label
        lbl_str = strtok(NULL,",");
        //Label to integer
        int label = atoi(lbl_str);

        double *l_x = getBagOfWords(ht,vocabulary,left_sp,"tf-idf");
        double *r_x = getBagOfWords(ht,vocabulary,right_sp,"tf-idf");
  
        //Load data 
        /*if(lines<train_size){
            X_train[lines - 1] = euclidean_distance(l_x,r_x,vocabulary->num_elements);
            y_train[lines - 1] = label; 
        }
        else{
            X_test[lines - train_size] = euclidean_distance(l_x,r_x,vocabulary->num_elements);
            y_test[lines - train_size] = label; 
        }*/

        X[lines - 1] = euclidean_distance(l_x,r_x,vocabulary->num_elements);
        y[lines - 1] = label; 
            
    }

    X_train = X;
    X_test = X + train_size;
    y_train = y;
    y_test = y + train_size;

    regressor= fit_logisticRegression(regressor,X_train,y_train,train_size-1);
    double *y_pred = inference_logisticRegression(regressor,X_test,y_test,test_size-1);


    for(int i=0;i<test_size;i++){
        printf("\nTarget: %d Prediction %f\n",y_test[i],y_pred[i]);
    }


    free(X_train);
    free(X_test);
    free(line);
    fclose(fp);
    delete_logisticReg(&regressor);
}
