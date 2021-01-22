#ifndef PROJECT_SPARSEVECTOR_H
#define PROJECT_SPARSEVECTOR_H

typedef struct sparse_vector{
    int *index_array;
    double *concentrated_matrix;
    int num_elements;
}sparseVector;

//Function to initialize a sparse vector
sparseVector *init_sparseVector(double *X,int size,int sparse_size);

//Function to find if an index exists in the given vector
int find_index_sparseVector(sparseVector *vector,int index);

//Function to perform a binary search on an int array
int binary_search(int *arr, int l, int r, int x);

//Function to destroy a sparse vector
void destroy_sparseVector(sparseVector *vector);

#endif //PROJECT_SPARSEVECTOR_H
