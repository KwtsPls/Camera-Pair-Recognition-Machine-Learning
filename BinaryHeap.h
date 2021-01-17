#ifndef BINARY_HEAP_H
#define BINARY_HEAP_H

#include <stdio.h>
#include "Transitivity.h"

//struct to represent a node of the binary heap
//The implementation of the binary heap is that of
//a binary tree that uses level order insertion
typedef struct pred_pair predictionPair;

typedef struct bh_node{

    float key;
    predictionPair *entry;
    struct bh_node *left;
    struct bh_node *right;
    struct bh_node *parent;

}BHNode;

//Struct that represents the binary heap
typedef struct bh_tree{
    BHNode *root;
    int num_elements;
}BHTree;

//Function to initialize a new binary heap
BHTree *initBH();

//Function to create a new binary heap node
BHNode *createBHNode(BHNode *,BHNode *,BHNode *,predictionPair*);

//Function to insert a new entry in the binary heap
void insertBHNode(BHTree **,BHNode *,BHNode *,BHNode *,predictionPair*);

//Function to get the rightmost node of the binary heap
//on the last level of the the heap
BHTree *getRightMostNode(BHTree*,float*,predictionPair**);

//Function to delete the root of the tree and return it's value
float deleteMaxBH(BHTree*,predictionPair **);

//Function to swap the values of two binary heap nodes
void swapNodes(float*,float*,predictionPair**,predictionPair**);

//Secondary function to traverse the tree inorder
void inorderPrint(BHNode*);

//Function to delete all the nodes of the tree
void deleteBHNodes(BHNode*);

//Function to delete the binary heap
void deleteBHT(BHTree*);

//Function to get the number of nodes in a binary heap
void numNodeBHT(BHNode*,int *);

#endif //BINARY_HEAP_H