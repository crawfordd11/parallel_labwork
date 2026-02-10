#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "LA3.h"

//Function to assign values to matrix A
// Here we assign A[i][j] to be sequential numbers starting from [1,1], row-wise
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
// Here we assign B[i][j] to be sequential numbers starting from [1,1], but column-wise
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
//n is the number of rows, m is the number of columns
void printMatrix(double* mat, int n, int m){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            printf("%6.2f ", mat[i*m + j]);
        }
        printf("\n");
    }
}

//Function to compute the dot product of two vectors A and B of size n
//Uses the sum of products method
//A and B are input vectors, C is the output (single value) and n is the length of the vectors
void dotProduct(double* A, double* B, double* C, int n){
    double sum = 0;
    for(int i=0;i<n;i++){
        sum+=A[i]*B[i];
    }    
    *C = sum;
}

//Function to perform matrix multiplication of A and B, resulting in C
//A is an n by m matrix, B is an m by p matrix, C is an n by p matrix
double* mm(double* A, double* B, int n, int m, int p){
    double* C=malloc(n*p*sizeof(double));
    for(int i=0;i<n;i++){                //pick a row in A
        for(int j=0;j<p;j++){            //pick a col in B
            int sum=0;
            for(int k=0;k<m;k++){        //do the dot product
                sum+=A[i*m+k]*B[k*p+j];  //A[i][k]*B[k][j]
            }
            C[i*p+j]=sum;
        }
    } 
    return C;
}

//Function to perform parallel matrix multiplication of A and B, resulting in C
//A is an n by m matrix, B is an m by p matrix, C is an n by p matrix
//rank is the rank of the current process, numranks is the total number of processes
//Requires dotProduct function to compute the dot product of a row of A and a column of B
void MPI_mm(double* A, double* B, double* C, int n, int m, int p, int rank, int numranks, MPI_Status stat){
    int numDotProducts = n * p;
    int dotProductsPerRank = numDotProducts / numranks;
    // int remainder = numDotProducts % numranks;

    // if(remainder > 0){
    //     dotProductsPerRank++;
    //     for(int i = 0; i < numranks; i++){
        
    //     }
    // }
    if(rank == 0){
        for(int i = 0; i < numranks; i++){
            int start = i * dotProductsPerRank;
            int end = (i == numranks - 1) ? numDotProducts : start + dotProductsPerRank;
//TODO: fix size of buffer to hold the rows of A and columns of B that this rank will compute the dot products for
            double* A_buffer = malloc(dotProductsPerRank * m * sizeof(double));
            double* B_buffer = malloc(dotProductsPerRank * m * sizeof(double));
//TODO: fill the buffers with the appropriate rows of A and columns of B 
            for(int j = 0; j < dotProductsPerRank; j++){
                int dotProductIndex = start + j;
                int row = dotProductIndex / p;
                int col = dotProductIndex % p;
                for(int k = 0; k < m; k++){
                    A_buffer[j*m + k] = A[row*m + k];
                    B_buffer[j*m + k] = B[k*p + col];
                }
            }   
            //send the buffers to the appropriate ranks
            MPI_Send(A_buffer, dotProductsPerRank * m, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
            MPI_Send(B_buffer, dotProductsPerRank * m, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
            MPI_Send(C, numDotProducts, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
            //receive the results from rank i
            MPI_Recv(A_buffer, dotProductsPerRank * m, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &stat);
            MPI_Recv(B_buffer, dotProductsPerRank * m, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &stat);
            MPI_Recv(C, numDotProducts, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &stat);
        }
    }
    
//TODO: check if blocking calls are an issue here for rank 0 to do work. If so, bring back else statement or consider non blocking. 

    //receive the results from rank 0
    MPI_Recv(A_buffer, dotProductsPerRank * m, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &stat);
    MPI_Recv(B_buffer, dotProductsPerRank * m, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &stat);
    MPI_Recv(C, numDotProducts, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &stat);
    
//TODO: compute the dot products assigned to this rank review this
    for(int j = 0; j < dotProductsPerRank; j++){
        double result;
        dotProduct(&A_buffer[j*m], &B_buffer[j*m], &result, m);
        int dotProductIndex = rank * dotProductsPerRank + j;
        int row = dotProductIndex / p;
        int col = dotProductIndex % p;
        C[row*p + col] = result;
    }
    
    //send the buffers back to rank 0
    MPI_Send(A_buffer, dotProductsPerRank * m, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    MPI_Send(B_buffer, dotProductsPerRank * m, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    MPI_Send(C, numDotProducts, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);   
    

    return C;
}

int main(int argc, char** argv){

    int rank,numranks;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numranks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Status stat;

    double* C=malloc(n*p*sizeof(double));


    MPI_Finalize();
    free(C);
    return 0;
}

