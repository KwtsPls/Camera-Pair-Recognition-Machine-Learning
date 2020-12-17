#include <stdlib.h>
#include <string.h>
#include "BagOfWords.h"

//Function to create an indexed word
indexedWord *createIndexedWord(char *word,int index){

    indexedWord *w = malloc(sizeof(indexedWord));
    w->word = malloc(strlen(word)+1);
    strcpy(w->word,word);
    w->index = index;
    w->idf=0.0;
    w->tf=1.0;
    w->tf_idf_mean=0.0;

    return w;
}

//Function to delete an indexed word
void deleteIndexedWord(indexedWord *iw){
    free(iw->word);
    free(iw);
}

//Function to return an array representing a bow with tf-idf values for a given spec id
double *getBagOfWords(HashTable *ht,secTable *vocabulary,char *spec_id,char *mode){


    if(strcmp(mode,"tf-idf")==0){
        //Get the length of the vocabulary
        int len = vocabulary->num_elements;

        //Initialize the array representing the bow
        double *bow = malloc(sizeof(double)*(len+1));
        double *idf_array = malloc(sizeof(double)*(len+1));

        //Initialize every value at zero
        for(int i=1;i<len+1;i++) {
            bow[i] = 0.0;
            idf_array[i]=0.0;
        }

        //Get the dictionary holding the information of the current spec
        Dictionary *dict = findSpecHashTable(ht,spec_id);

        int text_len=0;
        char *text = dict->list->value[0];
        char *cur=NULL;
        indexedWord *iw;

        //Iterate through the text to find it's length and calculate how many times each word from our vocabulary exists
        while((cur = strsep(&text, " "))!= NULL){
            if((iw=getIndexedWord_secTable(vocabulary,cur))!=NULL){
                text_len++;
                bow[iw->index+1]+=1.0;
                idf_array[iw->index+1] = iw->idf;
            }
        }

        for(int i=1;i<len+1;i++){
            //calculate tf-idf value for the word in this text
            if(bow[i]!=0.0)
                bow[i] = (bow[i]/(double)text_len)*idf_array[i];
        }

        //Add an additional column for bias
        bow[0]=1.0;

        free(idf_array);
        return bow;
    }
    else{
        //Get the length of the vocabulary
        int len = vocabulary->num_elements;

        //Initialize the array representing the bow
        double *bow = malloc(sizeof(double)*len+1);

        //Initialize every value at zero
        for(int i=1;i<len+1;i++) {
            bow[i] = 0.0;
        }

        //Get the dictionary holding the information of the current spec
        Dictionary *dict = findSpecHashTable(ht,spec_id);

        char *text = dict->list->value[0];
        char *cur=NULL;
        indexedWord *iw;

        //Iterate through the text to find it's length and calculate how many times each word from our vocabulary exists
        while((cur = strsep(&text, " "))!= NULL){
            if((iw=getIndexedWord_secTable(vocabulary,cur))!=NULL){
                bow[iw->index+1]+=1.0;
            }
        }

        //Add an additional column for bias
        bow[0]=1.0;

        return bow;
    }
}

//Helper function to test bow
void test_bow_implementation(HashTable *ht,secTable *vocabulary,char *filename){

    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    size_t read;

    //Open file
    fp = fopen(filename,"r");
    //Check if file Opened
    if(fp==NULL)
    {
        errorCode = OPENING_FILE;
        fclose(fp);
        print_error();
        return ;
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

        char *left_sp,*right_sp;
        //Take left_spec_id
        left_sp = strtok(line,",");
        //Take right_spec_id
        right_sp = strtok(NULL,",");

        double *bow_left = getBagOfWords(ht,vocabulary,left_sp,"tf-idf");
        double *bow_right = getBagOfWords(ht,vocabulary,right_sp,"tf-idf");

        free(bow_left);
        free(bow_right);
        i++;    //New line Read
    }

    free(line);
    fclose(fp);
}