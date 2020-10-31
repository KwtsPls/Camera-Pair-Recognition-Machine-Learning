#ifndef PROJECT_DICTIONARY_H
#define PROJECT_DICTIONARY_H



typedef struct dict_node{
    char *key;
    char **value;
    int values_num;
    struct dict_node *next;
}Dictionary_node;


typedef struct dict{
    char *dict_name;
    Dictionary_node *list;
}Dictionary;



//Function to initialize a dictionary
Dictionary *initDictionary(char *dict_name);

//Function for inserting in dictionary
Dictionary *insertDictionary(Dictionary *dic, char *key, char **val, int num_val);

//Function for printing dictionary
void printDictionary(Dictionary *dic);


//Give Key in the Dictionary to get the values
char **lookUpDictionary(Dictionary *dic, char *key, int *num_val);


//Function for Deleting Dictionaries
void deleteDictionary(Dictionary **dic);

#endif //PROJECT_DICTIONARY_H
