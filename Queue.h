#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include "BinaryHeap.h"

//A struct that represents a queue node used to simulate a BFS
//needed to create the Binary Heap ( used for level order traversal)
typedef struct qnode{

    BHNode *node;
    struct qnode *next;

}QueueNode;

//Struct to implement the queue - the queue will be a linked list
typedef struct queue{

    QueueNode *head;

}Queue;

//Function to initialize the queue
Queue *initQueue();

//Function to insert a new node at the end of the list
void pushQueue(Queue *,BHNode *);

//Function to pop the first node from the queue
BHNode *popQueue(QueueNode **);

//Function that returns if the list is empty or not
int emptyQueue(Queue*);

//Function to delete a queue
void destroyQueue(Queue*);

//Function to delete the last node of the list
void deleteLastQueueNode(Queue *);

#endif //QUEUE_H