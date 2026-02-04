#include <stdio.h>
#include <stdlib.h>
#include "mm-student.h"

//Matrix multiplication function
// C = A * B where A is n x m, B is m x p, C is n x p
// A, B, and C are stored in row-major order
// Returns pointer to the resulting matrix C
// Caller is responsible for freeing the allocated memory for C
double* mm(double* A, double* B, int n, int m, int p){
    double* C = (double*)malloc(n * p * sizeof(double));
    for(int i = 0; i < n; i++){
        for(int j = 0; j < p; j++){
            C[i*p + j] = 0;
            for(int k = 0; k < m; k++){
                C[i*p + j] += A[i*m + k] * B[k*p + j];
            }
        }
    }
    return C;
}

//Function to assign values to matrix A
// Here we assign A[i][j] to be sequential numbers starting from 1, row-wise
void assignA(double* mat, int n, int m){
    int counter = 1;
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            mat[i*m + j] = (double)counter;
            counter++;
        }
    }
}

//Function to assign values to matrix B
// Here we assign B[i][j] to be sequential numbers starting from 1, but column-wise
void assignB(double* mat, int n, int m){
    int counter = 1;
    for(int j = 0; j < m; j++){
        for(int i = 0; i < n; i++){
            mat[i*m + j] = (double)counter;
            counter++;
        }
    }
}

//Function to print matrix, row major order
void printMatrix(double* mat, int n, int m){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            printf("%6.2f ", mat[i*m + j]);
        }
        printf("\n");
    }
}

//Main function
int main(int argc, char **argv){
    
    int n = 3, m = 3, p = 3; // Example dimensions to match assingment
    double* A = (double*)malloc(n * m * sizeof(double));
    double* B = (double*)malloc(m * p * sizeof(double));    
    assignA(A, n, m);
    assignB(B, m, p);
    double* C = mm(A, B, n, m, p);

    printf("Factor Matrix A ( %d x %d ):\n", n, p);
    printMatrix(A, n, m);

    printf("Factor Matrix B ( %d x %d ):\n", m, p);
    printMatrix(B, m, p);

    printf("Product Matrix C ( %d x %d ):\n", n, p);
    printMatrix(C, n, p);

    free(A);
    free(B);
    free(C);
    return 0;
}

