#ifndef PROJECT_DICTIONARY_H
#define PROJECT_DICTIONARY_H

typedef struct dict{
    char *dict_name;
    Dictionary_node *list;
}Dictionary;

typedef struct dict_node{
    char *key;
    char **value;
    int values_num;
    struct dict_node *next;
}Dictionary_node;

//Function to initialize a dictionary
Dictionary initDictionary();

//Function for inserting in dictionary
Dictionary *insertDictionary(Dictionary *dic, char *key, char **val, int num_val);

#endif //PROJECT_DICTIONARY_H
