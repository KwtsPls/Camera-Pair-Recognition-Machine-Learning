#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sparseVector.h"

//Function to initialize a sparse vector
sparseVector *init_sparseVector(double *X,int size,int sparse_size){
    sparseVector *vector = malloc(sizeof(sparseVector));
    vector->num_elements = sparse_size;
    vector->index_array = malloc(sizeof(int)*sparse_size);
    vector->concentrated_matrix = malloc(sizeof(double)*sparse_size);

    int j=0;
    for(int i=0;i<size;i++){
        if(X[i]!=0.0){
            vector->index_array[j]=i;
            vector->concentrated_matrix[j]=X[i];
            j++;
        }
    }

    free(X);
    return vector;
}

//Function to find if an index exists in the given vector
int find_index_sparseVector(sparseVector *vector,int index){
    return binary_search(vector->index_array,0,vector->num_elements-1,index);
}

//Function to perform a binary search on an int array
int binary_search(int *arr, int l, int r, int x){
    if (r >= l) {
        int mid = l + (r - l) / 2;

        // If the element is present at the middle
        // itself
        if (arr[mid] == x)
            return mid;

        // If element is smaller than mid, then
        // it can only be present in left subarray
        if (arr[mid] > x)
            return binary_search(arr, l, mid - 1, x);

        // Else the element can only be present
        // in right subarray
        return binary_search(arr, mid + 1, r, x);
    }

    // We reach here when element is not
    // present in array
    return -1;
}

//Function to destroy a sparse vector
void destroy_sparseVector(sparseVector *vector){
    free(vector->index_array);
    free(vector->concentrated_matrix);
    free(vector);
    vector=NULL;
}