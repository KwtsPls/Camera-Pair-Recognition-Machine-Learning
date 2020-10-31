#include <stdio.h>
#include <stdlib.h>
#include "csvReader.h"
#include <string.h>


//Parser for finding pairs of spec_ids in the csv file
int csParser(char *filename)
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    //Open file
    fp = fopen(filename,"r");
    //Check if file Opened
    if(fp==NULL)
    {
        //PRINT Error
        printf("ERROR: CSV file was not found or could not be opened\n");
        return -1;
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
            printf("Left_Sp: %s, Right_Sp: %s\n",left_sp,right_sp);
        
        i++;    //New line Read
    }
    //Free space and close file
    free(line);
    fclose(fp);

    //Return num of files read
    return i;

}
