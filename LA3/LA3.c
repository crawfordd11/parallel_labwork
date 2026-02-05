#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "LA3.h"

void assignA(double* mat, int n, int m){
    //TODO: Fill the matrix as you see fit
}

void assignB(double* mat, int n, int m){
    //TODO: Fill the matrix as you see fit
}

double* mm(double* A, double* B, int n, int m, int p){
    double* C=malloc(n*p*sizeof(int));
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

int main(int argc, char** argv){

    int rank,numranks;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numranks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);




    MPI_Finalize();

    return 0;
}

