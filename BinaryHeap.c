#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Queue.h"

//Function to initialize a new binary heap
BHTree *initBH()
{
    BHTree *bht = (BHTree*)malloc(sizeof(BHTree));
    bht->root = NULL;
    bht->num_elements=0;
    return bht;
}

//Function to create a new binary heap node
BHNode *createBHNode(BHNode *p,BHNode *l,BHNode *r,predictionPair *entry)
{
    BHNode *node = (BHNode*)malloc(sizeof(BHNode));
    node->entry = entry;
    node->key = fabs(0.5 - entry->pred);
    node->parent = p;
    node->right = r;
    node->left = l;

    return node;
}

//Function to insert a new entry in the binary heap
void insertBHNode(BHTree **bht,BHNode *p,BHNode *l,BHNode *r,predictionPair* entry)
{

    //Binary heap is empty - create the first node of the tree
    (*bht)->num_elements++;

    if((*bht)->root==NULL)
    {
        (*bht)->root = createBHNode(p,l,r,entry);
        return;
    }

    //Initialize a queue to perform a BFS-like operation
    //to get the right place to insert the new node
    Queue *q = initQueue();
    pushQueue(q,(*bht)->root);
    BHNode *newNode = NULL;

    //iterate while the queue is not empty - in every iteration pop from the queue
    //a node and produce its children. With the queue a level order traversal is achieved
    //so if any of the node's children are NULL it means that we can add the node
    //in the rightmost free space of the tree
    while(emptyQueue(q)==0)
    {
        BHNode *temp = popQueue(&q->head);

        //if this node's left child is NULL the node will be inserted here
        if(temp->left==NULL)
        {
            temp->left = createBHNode(temp,NULL,NULL,entry);
            newNode = temp->left;
            break;
        }
        else
            pushQueue(q,temp->left);


        //if this node's right child is NULL the node will be inserted here
        if(temp->right==NULL)
        {
            temp->right = createBHNode(temp,NULL,NULL,entry);
            newNode = temp->right;
            break;
        }
        else
            pushQueue(q,temp->right);
    }


    //Iterate from the newly inserted node up to the root of the tree
    //to check if the heap property is maintained
    while(newNode!=(*bht)->root && newNode->key > newNode->parent->key)
    {
        swapNodes(&newNode->key,&newNode->parent->key,&newNode->entry,&newNode->parent->entry);
        newNode = newNode->parent;
    }

    //free the memory used for the queue
    destroyQueue(q);
}

//Function to get the rightmost node of the binary heap
//on the last level of the the heap
BHTree *getRightMostNode(BHTree *bht,float *right_key,predictionPair **entryName)
{
    //heap is empty
    if(bht->root==NULL)
        return NULL;


    //Binary heap has only one node left
    //get its value and delete it
    if(bht->root->left==NULL && bht->root->right==NULL)
    {
        *right_key = bht->root->key;
        *entryName = bht->root->entry;

        free(bht->root);
        bht->root=NULL;

        return bht;
    }

    //Initialize queue used for the BFS
    Queue *q = initQueue();
    pushQueue(q,bht->root);
    BHNode  *node=NULL;

    //Traverse through the binary heap in a level order fashion
    //and keep the last node remaining in the queue
    while (emptyQueue(q)==0) {

        if (q->head->node->left != NULL)
            pushQueue(q,q->head->node->left);

        if (q->head->node->right != NULL)
            pushQueue(q,q->head->node->right);

        node = popQueue(&q->head);

    }

    //free the queue
    destroyQueue(q);

    //keep the values of the node
    *right_key = node->key;
    *entryName = node->entry;


    //remember the node's parent
    BHNode *p = node->parent;

    int flag=0;
    if(p->right == node)
        flag=1;

    //delete the rightmost node of the last level
    free(node);
    node=NULL;

    //set this node as NULL from the parent node
    if(flag==0)
        p->left=NULL;
    else
        p->right=NULL;

    return bht;
}

//Function to delete the root of the tree and return it's value
float deleteMaxBH(BHTree *bht,predictionPair **entryName)
{
    if(bht->root==NULL)
        return -1;

    float max_key = bht->root->key;
    *entryName = bht->root->entry;
    bht->num_elements--;


    //get the key of the right-most node and save
    //it in the head of the tree
    float key;
    predictionPair *tempKey;
    bht = getRightMostNode(bht,&key,&tempKey);

    //Tree is empty
    if(bht->root==NULL)
        return max_key;

    bht->root->key = key;
    bht->root->entry = tempKey;

    BHNode *node = bht->root;

    //if this node is in the second to last level
    //and has only a left child - check if the heap
    //property is maintained
    if(node->left!=NULL && node->right==NULL)
    {
        if(node->left->key > node->key)
            swapNodes(&node->key,&node->left->key,&node->entry,&node->left->entry);
    }

    while(node->left!=NULL && node->right!=NULL)
    {


        //if this node is in the second to last level
        //and has only a left child - check if the heap
        //property is maintained
        if(node->left!=NULL && node->right==NULL)
        {
            if(node->left->key > node->key)
                swapNodes(&node->key,&node->left->key,&node->entry,&node->left->entry);

            break;
        }

        //if this node is in the second to last level
        //and has only a left child - check if the heap
        //property is maintained
        if(node->right!=NULL && node->left==NULL)
        {
            if(node->right->key > node->key)
                swapNodes(&node->key,&node->right->key,&node->entry,&node->right->entry);

            break;
        }


        //Node has two children - check if the heap property is maintained
        if(node->right != NULL && node->left != NULL)
        {

            //heap property is in check - the function can quit
            if(node->key > node->left->key && node->key > node->right->key)
                break;
            //right child's key is greater than left child's key
            //swap this node's key with the right child's key
            else if(node->right->key > node->left->key)
            {
                swapNodes(&node->key, &node->right->key, &node->entry, &node->right->entry);
                node = node->right;
            }
            //symmetrical case for left child
            else
            {
                swapNodes(&node->key, &node->left->key, &node->entry, &node->left->entry);
                node = node->left;
            }

        }


    }

    //return the max value
    return max_key;
}

//Function to swap the values of two binary heap nodes
void swapNodes(float *v1,float *v2,predictionPair **pair1,predictionPair **pair2)
{
    predictionPair *tempPair = *pair1;
    *pair1 = *pair2;
    *pair2 = tempPair;

    float tempValue = *v1;
    *v1 = *v2;
    *v2 = tempValue;
}

//Secondary function to traverse the tree inorder
void inorderPrint(BHNode *node)
{
    if(node==NULL)
        return;

    inorderPrint(node->left);
    printf("%f %s,%s,%f\n",node->key,node->entry->left_sp,node->entry->right_sp,node->entry->pred);
    inorderPrint(node->right);

}

//Function to delete all the nodes of the tree
void deleteBHNodes(BHNode *node)
{
    if(node==NULL)
        return;

    deleteBHNodes(node->left);
    deleteBHNodes(node->right);

    free(node);
}

//Function to delete the binary heap
void deleteBHT(BHTree *bht)
{
    deleteBHNodes(bht->root);
    free(bht);
}

//Function to get the number of nodes in a binary heap
void numNodeBHT(BHNode* node,int *count)
{
    if(node==NULL)
        return;

    numNodeBHT(node->left,count);
    (*count)++;
    numNodeBHT(node->right,count);
}
