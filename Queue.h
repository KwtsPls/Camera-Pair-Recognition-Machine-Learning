#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include "JobScheduler.h"
typedef struct job Job;

//A struct that represents a queue node used to simulate a BFS
//needed to create the Binary Heap ( used for level order traversal)
typedef struct qnode{

    Job *node;
    struct qnode *next;

}QueueNode;

//Struct to implement the queue - the queue will be a linked list
typedef struct queue{

    QueueNode *head;

}Queue;

//Function to initialize the queue
Queue *initQueue();

//Function to insert a new node at the end of the list
void pushQueue(Queue *,Job *);

//Function to pop the first node from the queue
Job *popQueue(QueueNode **);

//Function that returns if the list is empty or not
int emptyQueue(Queue*);

//Function to print the given queue
void printQueue(Queue *q);

//Function to delete a queue
void destroyQueue(Queue*);

//Function to delete the last node of the list
void deleteLastQueueNode(Queue *);

#endif //QUEUE_H