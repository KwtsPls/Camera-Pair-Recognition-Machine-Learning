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
    fp = fopen(filename,"r");
    char *line = NULL;
    size_t len = 0;
    size_t read;
    int i=0;    //Number Of Lines Counter

    logisticreg *regressor;
    regressor = create_logisticReg(vocabulary->num_elements,ABSOLUTE_DISTANCE);

    
    int train_size = linesRead-linesRead*0.2;
    printf("%d\n",train_size);
    double error = 0.0;

    double precision_1=0.0;
    double recall_1=0.0;
    double f1_1=0.0;

    int true_positive_1=0;
    int false_positive_1=0;
    int false_negative_1=0;

    double precision_0=0.0;
    double recall_0=0.0;
    double f1_0=0.0;

    int true_positive_0=0;
    int false_positive_0=0;
    int false_negative_0=0;

    int lines=0;
    int epoch = 1000;



    while((read = getline(&line, &len,fp))!=-1){

        if(i==0){               //Skip First Line cause its Left_spec, Right_Spec, label
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
        double *xi = euclidean_distance(l_x, r_x, regressor->numofN);

        if(lines<train_size){
            if(label==1){
                for(int k=0;k<11;k++)
                    regressor = fit_pair_logisticRegression(regressor, xi, label);
            }
            regressor = fit_pair_logisticRegression(regressor, xi, label);
        }
        else{
            double yi_pred = predict_pair_logisticRegression(regressor,xi);
            printf("Target %d Prediction %f\n",label,yi_pred);
            int pred = (int) round(yi_pred);
                if(label==1 && pred==label)
                    true_positive_1++;
                if(label==0 && pred==1)
                    false_positive_1++;
                if(label==1 && pred==0)
                    false_negative_1++;

                if(label==0 && pred==label)
                    true_positive_0++;
                if(label==1 && pred==0)
                    false_positive_0++;
                if(label==0 && pred==1)
                    false_negative_0++;
        }
        

        error+=loss_LogisticRegression(regressor,xi,label);
        if(lines%epoch==0) {
            printf("%d %f\n", lines / epoch, loss_LogisticRegression(regressor, xi, label));
            error=0.0;
        }


        free(l_x);
        free(r_x);
        free(xi);
    }


    precision_1 = (double)true_positive_1/((double)true_positive_1 + (double)false_positive_1);
    recall_1 = (double)true_positive_1/((double)true_positive_1 + (double)false_negative_1);
    f1_1 = 2*(precision_1*recall_1/(precision_1+recall_1));

    precision_0 = (double)true_positive_0/((double)true_positive_0 + (double)false_positive_0);
    recall_0 = (double)true_positive_0/((double)true_positive_0 + (double)false_negative_0);
    f1_0 = 2*(precision_0*recall_0/(precision_0+recall_0));

    printf("\n\nFor positive class:\n\nPrecision: %f\n\nRecall: %f\n\nf1 score: %f\n\n",precision_1,recall_1,f1_1);
    printf("\n\nFor negative class:\n\nPrecision: %f\n\nRecall: %f\n\nf1 score: %f\n\n",precision_0,recall_0,f1_0);

    free(line);
    fclose(fp);
    delete_logisticReg(&regressor);
}
