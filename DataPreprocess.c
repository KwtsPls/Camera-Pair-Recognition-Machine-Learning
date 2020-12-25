#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "SecTable.h"
#include "DataPreprocess.h"
#include "BagOfWords.h"

//Array with stopwords - temporary
char *stopwords_array[]={"www","http","com","a","able","about","across","after","all","almost","also","am","among","an","and","any","are","as","at","be","because","been","but","by","can","cannot","could","dear","did","do","does","either","else","ever","every","for","from","get","got","had","has","have","he","her","hers","him","his","how","however","i","if","in","into","is","it","its","just","least","let","like","likely","may","me","might","most","must","my","neither","no","nor","not","of","off","often","on","only","or","other","our","own","rather","said","say","says","she","should","since","so","some","than","that","the","their","them","then","there","these","they","this","tis","to","too","twas","us","wants","was","we","were","what","when","where","which","while","who","whom","why","will","with","would","yet","you","your"};


//Function to create a hash table containing stopwords
secTable *init_stopwords(){
    secTable *st = create_secTable(ST_INIT_SIZE,SB_SIZE,HashString,CompareString,DeleteString,String);
    int size = sizeof(stopwords_array)/ sizeof(stopwords_array[0]);
    //Insert every stopword from the array into the table
    for(int i=0;i<size;i++)
        st = insert_secTable(st,stopwords_array[i]);

    return st;
}


//Function to perform some basic cleaning on a string
char *text_cleaning(char *text){

    char *src = text, *dst = text;

    while (*src){

        //Replace change line characters that were not parsed correctly with a blank space
        if(*src=='\\' && *(src+1)=='n'){
            src++;
            *dst++ = (unsigned char)32;
            src++;
        }
        else if (ispunct((unsigned char)*src)){
            /* Convert the punctuation to a blank space */
            *dst++ = (unsigned char)32;
            src++;
        }
        else if (isupper((unsigned char)*src)){
            /* Make it lowercase */
            *dst++ = tolower((unsigned char)*src);
            src++;
        }
        else if (src == dst){
            /* Increment both pointers without copying */
            src++;
            dst++;
        }
        else{
            /* Copy character */
            *dst++ = *src++;
        }
    }

    *dst = 0;

    return text;
}

//Check if word is in utf-16 encoding
int check_utf16(char *word){
    if(strlen(word)>2)
        return (word[0]=='u' && isdigit(word[1]));
    else
        return 0;
}

//Function to check if a word is a single non numeric character
int single_character(char *word) {
    if (strlen(word) > 1)
        return 0;
    else{
        if(isdigit(word[0]))
            return 0;
        else
            return 1;
    }
}

//Function to remove stopwords from a given string
char *remove_stopwords(char *text,secTable *stopwords,secTable **vocabulary,secTable **unique_words,int *len){

    //Allocate space for the new text
    char *new_text = malloc(strlen(text)+1);
    memset(new_text,0,strlen(text)+1);

    char *cur;
    char empty[1];
    empty[0]=0;

    //For every word in the text check if it is a stopword and if it is ignore it
    while((cur = strsep(&text, " "))!= NULL){

        //If current word does is not a stopword store it
        if(find_secTable(stopwords,cur)==0 && check_utf16(cur)==0 && strlen(cur)<15 && strcmp(cur,empty)!=0){

            if (strlen(new_text) == 0)
                strcpy(new_text, cur);
            else {
                strcat(new_text, " ");
                strcat(new_text, cur);
            }

            //If the current word does not exist in the vocabulary add it
            indexedWord *iw = createIndexedWord(cur,(*vocabulary)->num_elements);
            if (find_secTable(*vocabulary, iw) == 0) {
                *vocabulary = insert_secTable(*vocabulary, iw);
                char *new_word = malloc(strlen(cur)+1);
                strcpy(new_word,cur);
                *unique_words = insert_secTable(*unique_words, new_word);
            }
            else{
                if(find_secTable(*unique_words,cur)==0) {
                    char *new_word = malloc(strlen(cur)+1);
                    strcpy(new_word,cur);
                    *unique_words = insert_secTable(*unique_words, new_word);
                }
                *vocabulary = updateTF_secTable(*vocabulary,iw);
                deleteIndexedWord(iw);
            }


            (*len)++;
        }
    }

    //Delete the old string and return the new one
    if(text!=NULL)
        free(text);

    return new_text;
}

//Function to preprocess a given string
char *preprocess(char *text,secTable *stopwords,secTable **vocabulary,secTable **unique_words,int *len){

   //printf("Before : %s\n",text);

    //Text cleaning for the given string
    text = text_cleaning(text);

    //Remove stopwords from given text
    text = remove_stopwords(text,stopwords,vocabulary,unique_words,len);

    //printf("AFTER : %s\n",text);

    return text;
}

//Function to swap integers
void swap_int(int *a,int *b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

//Function to swap doubles
void swap_vectors(double **a,double **b){
    double *temp = *a;
    *a = *b;
    *b = temp;
}

//Function to swap strings
void swap_string(char **a,char **b){
    char *temp = *a;
    *a = *b;
    *b = temp;
}

//Function to return an integer in range
int random_int(int n){
    int e =  n/RAND_MAX;
    int mod = n % RAND_MAX;
    int r = (rand()%RAND_MAX)*e + (rand()%mod);
    return r;
}

//Function to shuffle the given data
void shuffle_data(double **X,int *y,char **pairs,int n,int random_state){

    //For the given number of random_state shuffle the array
    for(int k=0;k<random_state;k++){
        for(int i=0;i<n;i++){
            int random_index = random_int(n);
            swap_int(&y[i],&y[random_index]);
            swap_vectors(&X[i],&X[random_index]);
            swap_string(&pairs[i],&pairs[random_index]);
        }
    }

}