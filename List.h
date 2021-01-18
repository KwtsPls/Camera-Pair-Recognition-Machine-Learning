#ifndef PROJECT_LIST_H
#define PROJECT_LIST_H

#include "Transitivity.h"

typedef struct pred_pair predictionPair;

typedef struct list_node{

    predictionPair *entry;
    struct list_node *next;

}ListNode;

typedef struct linked_list{

    int num_elements;
    ListNode *head;
    ListNode *tail;

}LinkedList;

//Function to initialize a list
LinkedList *init_LinkedList();

//Function to insert a new entry into the list
LinkedList *insert_LinkedList(LinkedList *l,predictionPair *entry);

//Function to search if a given entry exists in the list
int search_LinkedList(LinkedList *l,predictionPair *pair);

//Function to print a given list
void print_LinkedList(LinkedList *l);

//Function to destroy a list
void destroy_LinkedList(LinkedList *l);

#endif //PROJECT_LIST_H
