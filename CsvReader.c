#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HashTable.h"
#include "CsvReader.h"


//Parser for finding pairs of spec_ids in the csv file
HashTable *csvParser(char *filename,HashTable **ht)
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    size_t read;

    //Open file
    fp = fopen(filename,"r");
    //Check if file Opened
    if(fp==NULL)
    {
        //PRINT Error
        printf("ERROR: CSV file was not found or could not be opened\n");
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
        
        char *left_sp,*right_sp,*lbl_str;
        //Take left_spec_id
        left_sp = strtok(line,",");
        //Take right_spec_id
        right_sp = strtok(NULL,",");
        //Take label
        lbl_str = strtok(NULL,",");
        //Label to integer
        int label = atoi(lbl_str);
        if(label == 1) //They re the same 
        {
            //printf("\nleft spec: %s , right spec: %s \n",left_sp,right_sp);
            *ht = createCliqueHashTable(ht, left_sp,right_sp);
        }
        i++;    //New line Read
    }
    //Free space and close file
    free(line);
    fclose(fp);

    //Return num of files read
    return *ht;
}


//Function to create a new csv to write the cliques into
void csvWriteCliques(HashTable **ht){
    FILE *fp;
    fp = fopen("cliques.csv","w+");

    //Write first line into file of the line
    int err = fprintf(fp,"left_spec_id, right_spec_id\n");
    //Something went wrong while writing to the file....
    if(err == EOF)
        exit(10);

    //Iterate through the Hash Table
    for(int i=0;i<(*ht)->buckets_num;i++)
    {
        //If the table is not Null iterate through it
        if((*ht)->table[i]!=NULL)
        {
            for(int j=0;j<(*ht)->table[i]->num_entries;j++)
            {
                //So dirty bit is 1 we have to write it in the file as a clique
                if((*ht)->table[i]->array[j]->set->dirty_bit==1)
                {
                    //Make the bit of the list to 0 so we dont write it again in the file
                    (*ht)->table[i]->array[j]->set->dirty_bit=0;

                    bucketListWriteCliques((*ht)->table[i]->array[j]->set,fp);

                }
            }
        }
    }

    fclose(fp);
}   