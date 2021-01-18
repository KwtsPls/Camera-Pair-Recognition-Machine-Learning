#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "RBtree.h"


//Function to initialize a new Red/Black tree
RBtree *initRB(){
    RBtree *rbt = (RBtree*)malloc(sizeof(RBtree));
    rbt->root=NULL;
    rbt->num_elements=0;
    return  rbt;
}

//Function to create a new AVL tree node
RBnode *createTreeNode(predictionPair *entry,RBnode *p,RBnode *l,RBnode *r){
    RBnode *node = (RBnode*)malloc(sizeof(RBnode));

    //initialize the list of pairs and insert its first entry
    node->l = init_LinkedList();
    node->l = insert_LinkedList(node->l,entry);

    //Set the nodes key
    node->key = fabs(0.5-entry->pred);

    node->parent=p;
    node->left=l;
    node->right=r;

    return node;
}

//Function that returns a node containing the given pair
RBnode *findRBNode(RBnode *node,predictionPair *pair){
    if (node == NULL) return NULL;

    if (fabs(0.5-pair->pred) == node->key) return node;
    else if (fabs(0.5-pair->pred) < node->key ) return findRBNode(node->left,pair);
    else return findRBNode(node->right,pair);
}

//Function to search for a patient -> uses findRBNode
RBnode *searchRB(RBtree *rbt,predictionPair *pair)
{
    return findRBNode(rbt->root,pair);
}


//Function to insert a new record in a bst fashion
RBnode *insertBST(RBnode *node,RBnode *parent,predictionPair *entry){

    if(node==NULL) return createTreeNode(entry,parent,NULL,NULL);

    /* Function is called recursively with the parent node being the current one*/

    //Key already exists in this tree add the entry
    //in this node's list
    if(fabs(0.5- entry->pred) == node->key){
        node->l = insert_LinkedList(node->l,entry);
    }
    else if(fabs(0.5- entry->pred) < node->key) node->left = insertBST(node->left,node,entry);
    else node->right = insertBST(node->right,node,entry);

    return node;

}

//Function to insert a new record in the Red/Black and maintain its properties
RBtree *insertRB(RBtree *rbt,predictionPair *pair)
{
    RBnode *cur,*uncle;

    rbt->root = insertBST(rbt->root,rbt->root,pair);
    rbt->num_elements++;

    //with searchRB get a pointer to the newly added node
    cur = searchRB(rbt,pair);

    //Entry was inserted in a node with the same key
    //the tree's balanced is not changed
    if(cur->l->num_elements>1)
        return rbt;

    //paint the new node red
    cur->colour=RED;

    //An iteration that traverses upward the tree till it reaches the root of the tree or the parent of the current node is black
    while((cur!=rbt->root) && (cur->parent->colour==RED))
    {

        //if the parent of current node is the left child of its parent then its sibling is the right child
        if(cur->parent == cur->parent->parent->left){
            uncle = cur->parent->parent->right;
            //case 2 : preform recolouring and make set the current node as its grandparent to continue the iteration*
            if(uncle!=NULL && uncle->colour==RED){
                cur->parent->colour=BLACK;
                uncle->colour=BLACK;
                cur->parent->parent->colour=RED;
                cur=cur->parent->parent;
            }
            //case 1 if parent of node is left child of it's parent : preform trinode reconstruction
            else if(uncle==NULL || uncle->colour==BLACK){
                //Double Left Right case
                if(cur==cur->parent->right){
                    cur=cur->parent;
                    left_rotation(rbt,cur);
                }
                /* Single Right case*/
                cur->parent->colour=BLACK;
                cur->parent->parent->colour=RED;
                right_rotation(rbt,cur->parent->parent);
            }
        }
        else
        {
            //A similar operation is carried out if parent of current node is the right child of its parent
            uncle= cur->parent->parent->left;
            if(uncle!=NULL && uncle->colour==RED){
                cur->parent->colour=BLACK;
                uncle->colour=BLACK;
                cur->parent->parent->colour=RED;
                cur=cur->parent->parent;
            }
            else if(uncle==NULL || uncle->colour==BLACK){
                if(cur==cur->parent->left){
                    cur=cur->parent;
                    right_rotation(rbt,cur);
                }
                cur->parent->colour=BLACK;
                cur->parent->parent->colour=RED;
                left_rotation(rbt,cur->parent->parent);
            }
        }
    }

    //Always maintain a black root
    rbt->root->colour=BLACK;
    return rbt;
}

//Function to traverse a Red/Black tree in order -> uses inorderRBNode for the actual traversal
void inorderRB(RBtree *rbt)
{
    inorderRBNode(rbt->root);
}

//Function to traverse an AVL tree in order
void inorderRBNode(RBnode *node)
{
    if(node==NULL)
        return;

    inorderRBNode(node->left);
    print_LinkedList(node->l);
    inorderRBNode(node->right);
}

/* These are the rotation functions, adjusted to update the parent node as well*/

//Function to do a right rotation
void right_rotation(RBtree *rbt,RBnode *node)
{
    RBnode *x;
    x=node->left;
    node->left = x->right;

    if(x->right!=NULL)	x->right->parent=node;
    x->parent=node->parent;
    if (node->parent==NULL) rbt->root=x;
    else{
        if(node == (node->parent)->right) node->parent->right=x;
        else node->parent->left=x;
    }
    x->right=node;
    node->parent=x;
}

//Function to do a left rotation
void left_rotation(RBtree *rbt,RBnode *node)
{
    RBnode *x;
    x=node->right;
    node->right=x->left;

    if(x->left!=NULL) x->left->parent=node;
    x->parent=node->parent;
    if(node->parent==NULL) rbt->root=x;
    else{
        if (node==(node->parent)->left)	node->parent->left = x;
        else node->parent->right = x;
    }
    x->left=node;
    node->parent=x;
}

//Function to delete a Red/Black tree
void destroyRB(RBtree *rbt)
{
    destroyRBNode(rbt->root);
    free(rbt);
}

//Function to delete an AVL tree
void destroyRBNode(RBnode *node)
{
    if (node == NULL)
        return;

    /* first delete both subtrees */
    destroyRBNode(node->left);
    destroyRBNode(node->right);
    destroy_LinkedList(node->l);
    free(node);
}