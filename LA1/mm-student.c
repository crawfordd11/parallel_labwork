#include <stdio.h>
#include <stdlib.h>
#include "mm-student.h"
#include <mpi.h>

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

//Function to assign values to matrix
// Here we assign B[i][j] to identity regardless of dimensions
void assignI(double* mat, int n, int m){
    for(int j = 0; j < m; j++){
        for(int i = 0; i < n; i++){
            if(i==j){
                mat[i*m + j] = 1.0;
            }
            else{
                mat[i*m + j] = 0.0;
            }
        }
    }
}

void printMatrix(double* mat, int n, int m){
    int limit = 10;
    if(n>limit || m>limit){
        int smallerN = (n>limit) ? limit : n;
        int smallerM = (m>limit) ? limit : m;
        for(int i = 0; i < smallerN; i++){
            if(i==limit-1){
                for(int j = 0; j < smallerM+1; j++){printf("   ... ");}
                printf("\n");
            }
            for(int j = 0; j < smallerM; j++){
                if(j==smallerM-1 && i==smallerN-1){
                    printf("   ... ");
                    printf("%6.2f ", mat[(n*m)-1]);
                }
                else if(j==limit-1){
                    printf("   ... ");
                    printf("%6.2f ", mat[i*m + m-1]);
                }
                else{
                    printf("%6.2f ", mat[i*m + j]);
                }
            }
            printf("\n");
        }
    }
    else{
        for(int i = 0; i < n; i++){
            for(int j = 0; j < m; j++){
                printf("%6.2f ", mat[i*m + j]);
            }
            printf("\n");
        }
    }
    
}


//Main function
int main(int argc, char **argv){
    
    double startfull = MPI_Wtime();

    int n = 1024*16, m = 1024*16, p = 1; // Example dimensions to match assingment
    double* A = (double*)malloc(m * p * sizeof(double));
    double* B = (double*)malloc(n * m * sizeof(double));    
    assignA(A, m, p);
    assignI(B, n, m);
    double* C = mm(B, A, n, m, p);

    double endfull = MPI_Wtime();

    printf("Full time: %f\n",endfull-startfull);
    printf("Factor Matrix A ( %d x %d ):\n", m, p);
    printMatrix(A, m, p);

    printf("Factor Matrix B ( %d x %d ):\n", n, m);
    printMatrix(B, n, m);

    printf("Product Matrix C ( %d x %d ):\n", n, p);
    printMatrix(C, n, p);

    free(A);
    free(B);
    free(C);
    return 0;
}

