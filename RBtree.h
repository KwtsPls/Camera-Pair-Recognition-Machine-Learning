#ifndef HW2_RBTREE_H
#define HW2_RBTREE_H

#include <stdio.h>
#include "List.h"
#include "Transitivity.h"

typedef struct linked_list LinkedList;

//Enumeration representing red and black nodes for Red Black trees
typedef enum {RED,BLACK} NodeColour;

//Struct that represents a node of an Red/Black tree
typedef struct rb_node{

    double key;
    LinkedList *l;
    NodeColour colour;
    struct  rb_node *parent;
    struct  rb_node *left;
    struct rb_node *right;

}RBnode;

typedef struct rb_tree{
    RBnode *root;
    int num_elements;
}RBtree;

//Function to initialize a new Red/Black tree
RBtree *initRB();

//Function to create a new Red/Black tree node
RBnode *createTreeNode(predictionPair *,RBnode *,RBnode *,RBnode *);

//Function to insert a new record in a bst fashion
RBnode *insertBST(RBnode *,RBnode *,predictionPair*);

//Function to insert a new record in the Red/Black and maintain its properties
RBtree *insertRB(RBtree *,predictionPair*);

//Function that returns a node containing the given patient and set num as the number of
//records this node holds in its list
RBnode *findRBNode(RBnode*,predictionPair*);

//Function to search for a patient -> uses findRBNode
RBnode *searchRB(RBtree*,predictionPair*);

//Function to traverse a Red/Black tree in order -> uses inorderRBNode for the actual traversal
void inorderRB(RBtree*);

//Function to traverse the nodes of a Red/Black tree in order
void inorderRBNode(RBnode*);

//Function to do a right rotation
void right_rotation(RBtree *rbt,RBnode *node);

//Function to do a left rotation
void left_rotation(RBtree *rbt,RBnode *node);

//Function to delete all nodes of a Red/Black tree
void destroyRBNode(RBnode*);

//Function to delete a Red/Black tree
void destroyRB(RBtree *);

#endif //HW2_RBTREE_H
