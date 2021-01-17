#include <stdio.h>
#include <stdlib.h>
#include "Queue.h"

//Function to initialize the queue
Queue *initQueue()
{
    Queue *q = (Queue*)malloc(sizeof(Queue));
    q->head=NULL;
    return q;
}

//Function to insert a new node at the end of the list
//The insertion is like a linked list's
void pushQueue(Queue *q,BHNode *node)
{
    //if the list is empty
    if(q->head==NULL)
    {
        q->head = (QueueNode*)malloc(sizeof(QueueNode));
        q->head->node = node;
        q->head->next = NULL;
    }

    QueueNode *cur = q->head;

    //get to the last node of the list
    while(cur->next!=NULL)
    {
        cur = cur->next;
    }

    cur->next = (QueueNode*)malloc(sizeof(QueueNode));
    cur->next->node = node;
    cur->next->next = NULL;
}

//Function to pop the first node from the queue
BHNode *popQueue(QueueNode **head)
{
    BHNode *node = NULL;
    QueueNode *cur = NULL;

    //list is empty - no item to pop
    if (*head == NULL) {
        return NULL;
    }

    cur = (*head)->next;
    node = (*head)->node;
    free(*head);
    *head = cur;

    return node;
}

//Function that returns if the list is empty or not
int emptyQueue(Queue *q)
{
    return (q->head==NULL);
}

//Function to delete a queue
void destroyQueue(Queue *q)
{
    while(q->head!=NULL)
        deleteLastQueueNode(q);

    free(q);
}

//Function to delete the last node of the list
void deleteLastQueueNode(Queue *q)
{
    if(q->head->next==NULL)
    {
        free(q->head);
        q->head=NULL;
        return;
    }

    QueueNode *cur = q->head;

    while(cur->next->next!=NULL)
    {
        cur=cur->next;
    }

    free(cur->next);
    cur->next=NULL;
}
