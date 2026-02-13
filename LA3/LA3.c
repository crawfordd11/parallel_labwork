#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "LA3.h"

//TODO: review MPI_mm function for correctness
//TODO: implement main function to test MPI_mm

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
                if(j==limit-1 && i==limit-1){
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

//Function to transpose a matrix A of size m by n, resulting in a matrix of size n by m
//Replaces the original matrix values A with its transpose.
//Requires that the input matrix A is stored in row major order, and that the output matrix is also stored in row major order.
void transposeMatrix(double* A, int m, int n){
    // Create a new matrix 'Transpose' of size n x m
    double* Transpose = (double*) malloc(n * m * sizeof(double));

    // Iterate through each element of the original matrix A
    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            Transpose[j*m + i] = A[i*n + j];
        }
    }
    
    for(int i = 0; i < m*n; i++){
        A[i] = Transpose[i];
    }

    free(Transpose);
}
    

//Function to compute the dot product of two vectors A and B of size n
//Uses the sum of products method
//A and B are input vectors, C is the output (single value) and n is the length of the vectors
void dotProduct(double* A, double* B, double* C, int n){
    *C = 0;
    for(int i=0;i<n;i++){
        *C+=A[i]*B[i];
    }    
}

//Function to perform matrix multiplication of A and B, resulting in C
//A is a vector that represents n by m matrix, B is a vector that represents m by p matrix, C is an n by p matrix
double* mm(double* A, double* B, int n, int m, int p){
    double* C=malloc(n*p*sizeof(double));
    for(int i=0;i<n;i++){                //pick a row in A
        for(int j=0;j<p;j++){            //pick a col in B
            double sum=0;
            for(int k=0;k<m;k++){        //do the dot product
                sum+=A[i*m+k]*B[k*p+j];  //A[i][k]*B[k][j]
            }
            C[i*p+j]=sum;
        }
    } 
    return C;
}


int main(int argc, char** argv){


    //==========================================
    int rank,numranks;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numranks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Status stat;

    double startfull = MPI_Wtime();

    int N, M;
    if(rank==0){
        N=1000;
        M=1000;
    }

    MPI_Bcast(&N,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&M,1,MPI_INT,0,MPI_COMM_WORLD);

    double *a;
    double *b;
    double *c;
    double startalloc=MPI_Wtime();
    if(rank==0){
        a=(double*) malloc(M*sizeof(double));
        b=(double*) malloc(N*M*sizeof(double));
        c=(double*) malloc(N*sizeof(double));
        for(int i = 0; i<M; i++){
            for(int j = 0; j<N; j++){
                b[i*M+j]=1.0;
            }
        }
        assignA(a, M, 1);
    }
    double endalloc=MPI_Wtime();

    int myN=N/numranks;
    
    int *sendcounts=(int*)malloc(numranks*sizeof(int));
    int *recvcounts=(int*)malloc(numranks*sizeof(int));
    int *disp=(int*)malloc(numranks*sizeof(int));
    int *recvdisp=(int*)malloc(numranks*sizeof(int));

    for(int i = 0; i<numranks; i++){
        sendcounts[i]=myN*M;
        recvcounts[i]=myN;
    }
    sendcounts[numranks-1]+=(N-myN*numranks)*M;
    recvcounts[numranks-1]+=(N-myN*numranks);

    //printf("Rank %d: Number of Elements: %d Number of Rows of B: %d\n",rank,sendcounts[rank],sendcounts[rank]/M);

    disp[0]=0;
    recvdisp[0]=0;
    for(int i = 1; i<numranks; i++){
        disp[i]=disp[i-1]+sendcounts[i-1];
        recvdisp[i]=recvdisp[i-1]+recvcounts[i-1];
    }

    //int *mya=(int*) malloc(M*sizeof(int));
    double *myb=(double*) malloc(sendcounts[rank]*sizeof(double));

    double startscatter=MPI_Wtime();
    //MPI_Scatter(info to send, how many per rank, what kind, where do I recieve info, how many, what kind, from who, what world am I in)
    MPI_Bcast(a,M,MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Scatterv(b,sendcounts,disp,MPI_DOUBLE,myb,sendcounts[rank],MPI_DOUBLE,0,MPI_COMM_WORLD);
    double endscatter=MPI_Wtime();

    double startcomp=MPI_Wtime();

    int myRows=sendcounts[rank]/M; 
    double* C_buf = (double*) malloc(myRows * sizeof(double));

    double result=0;
    for(int i=0; i<myRows;i++){
        dotProduct(a, &myb[i*M], &result, M);
        C_buf[i]=result;
    }
    double endcomp=MPI_Wtime();

    double startgather=MPI_Wtime();
    //MPI_Reduce has a collector, but Allreduce sends to all
    MPI_Gatherv(C_buf, myRows, MPI_DOUBLE, c, recvcounts, recvdisp, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    double endgather=MPI_Wtime();
    double endfull=MPI_Wtime();

    //==========================================

    if(rank==0){    
        printf("Full time: %f\n",endfull-startfull);
        printf("Calc time: %f\n", endcomp-startcomp);
        printf("Alloc+Initalize time: %f\n", endalloc-startalloc);
        printf("Scatter time: %f\n",endscatter-startscatter);
        printf("Gather time: %f\n",endgather-startgather);
    
        printf("Factor Vector A ( %d x %d ):\n", M, 1);
        printMatrix(a, M, 1);

        printf("Factor Matrix B ( %d x %d ):\n", N, M);
        printMatrix(b, N, M);

        printf("Product Matrix C ( %d x %d ):\n", N, 1);
        printMatrix(c, N, 1);

        free(a);
        free(b);
        free(c);
    }

    free(sendcounts);
    free(recvcounts);
    free(disp);
    free(recvdisp);
    free(myb);
    free(C_buf);

    MPI_Finalize();
    
    return 0;
}
