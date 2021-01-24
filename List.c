#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "List.h"

//Function to initialize a list
LinkedList *init_LinkedList(){
    LinkedList *l = malloc(sizeof(LinkedList));
    l->head=NULL;
    l->tail=NULL;
    l->num_elements=0;
    return l;
}

//Function to insert a new entry into the list
LinkedList *insert_LinkedList(LinkedList *l,predictionPair *entry){

    ListNode *cur = malloc(sizeof(ListNode));
    cur->entry = entry;
    cur->next=NULL;

    //List is empty append its first node
    if(l->head==NULL){
        l->head = l->tail = cur;
    }
    else{
        //insert a new node at the end of the list
        l->tail->next = cur;
        l->tail = cur;
    }

    l->num_elements++;
    return l;
}

//Function to search if a given entry exists in the list
int search_LinkedList(LinkedList *l,predictionPair *pair){

    ListNode *cur = l->head;

    while(cur!=NULL){

        predictionPair *temp = cur->entry;
        if(strcmp(temp->left_sp,pair->left_sp)==0 && strcmp(temp->right_sp,pair->right_sp)==0 && temp->pred == pair->pred)
            return 1;

        cur = cur->next;
    }

    return 0;
}

//Function to print a given list
void print_LinkedList(LinkedList *l){
    ListNode *cur = l->head;

    while(cur!=NULL){
        predictionPair *temp = cur->entry;
        printf("%s,%s,%f\n",temp->left_sp,temp->right_sp,temp->pred);
        cur = cur->next;
    }
}

//Function to destroy a list
void destroy_LinkedList(LinkedList *l){

    if(l->head==NULL) {
        free(l);
        return;
    }

    //iteratively delete the first node of the list
    while(l->head!=NULL)
    {
        //list has only one node
        if(l->head==l->tail)
        {
            deletePredictionPair(l->head->entry);
            free(l->head);
            l->head=NULL;
            l->tail=NULL;
        }
        else{
            //Save the first node in cur
            ListNode* cur = l->head;
            //Set the list's head to point at the second node
            l->head = l->head->next;
            deletePredictionPair(cur->entry);
            free(cur);
        }
    }

    free(l);
}
