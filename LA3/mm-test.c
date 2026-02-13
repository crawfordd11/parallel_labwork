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
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            printf("%6.2f ", mat[i*m + j]);
        }
        printf("\n");
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

//Function to perform parallel matrix multiplication of A and B, resulting in C
//A is an n by m matrix, B is an m by p matrix, C is an n by p matrix
//rank is the rank of the current process, numranks is the total number of processes
//Requires dotProduct function to compute the dot product of a row of A and a column of B
//Requires a Transpose function to transpose B
void MPI_mm(double* A, double* B, double* C, int n, int m, int p, int rank, int numranks){
    int numDotProducts = n * p;
    int dotProductsPerRank = numDotProducts / numranks;
    
    //In our tests, numranks << numDotProducts, allowing for the assumption that each rank will compute at least one dot product, 
    //The case where the number of ranks is greater than the number of dot products is not handled by this code, 
    //and would require additional logic to ensure optimal load ballance.

    //Create buffers for the number of dot products assigned to each rank, and the displacements for those dot products.
    int* C_sendcounts = (int*) malloc(numranks * sizeof(int));
    int* C_displs = (int*) malloc(numranks * sizeof(int));
    
    //This loop describes the exact responsibility for each rank
    //It does not add padding to account for the fact that the number of dot products may not be perfectly divisible 
    //by the number of ranks, but it does ensure that all dot products are assigned to a rank.
    if(rank == 0){    
        for(int i = 0; i < numranks; i++){
            int sendcount = (i == numranks - 1) ? numDotProducts - (dotProductsPerRank * i) : dotProductsPerRank;
            int displs = i * dotProductsPerRank;
            C_sendcounts[i] = sendcount;
            C_displs[i] = displs;
        }
    }
    
    // //TODO: Optimize sending of A and B with MPI_Scatterv.
    // MPI_Scatterv(A, A_sendcounts, A_displs, MPI_DOUBLE, C_buf, A_displs[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);
    transposeMatrix(B, m, p);

    MPI_Bcast(A, n*m, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(B, m*p, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    //allocate info for the receive buffer on each rank. The finished work for a given rank will be stored here.
    double* C_buf = (double*) malloc(C_sendcounts[rank] * sizeof(double));

    //Each rank computes its assigned dot products, storing the results in C_buf. 
    //The index of the dot product corresponds to the row of A and column of B that are used for that dot product.
    for(int j = C_displs[rank]; j < C_displs[rank] + C_sendcounts[rank]; j++){
        double result = 0;
        int dotProductIndex = j;
        int col = dotProductIndex % p;
        int row = (dotProductIndex -col) / n;
        
        //make proper vectors to pass to the dot product function, which expects two vectors of size m.
        //because B has been transposed, we can treat the column of B as a row vector.
        dotProduct(&A[row*m], &B[col*m], &result, m);

        C_buf[j - C_displs[rank]] = result;
    }
    
    //send the buffers back to rank 0, which will compose the final matrix C from the results.
    MPI_Gatherv(C_buf, C_sendcounts[rank], MPI_DOUBLE, C, C_sendcounts, C_displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    free(C_sendcounts);
    free(C_displs);
    free(C_buf);
}

int main(int argc, char** argv){

    int rank,numranks;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numranks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int n = 3, m = 3, p = 3; // Example dimensions to match assingment
    
    double *A, *B, *C;

    //generate data
    if(rank == 0){
        //long SIZE = 1024*1024*1024/sizeof(double); //1GB worth of doubles

        *A = (double*) malloc(n * m * sizeof(double));
        *B = (double*) malloc(m * p * sizeof(double));    
        *C = (double*) malloc(n * p * sizeof(double)); 

        assignA(A, n, m);
        assignB(B, m, p);
    }
    
    //multiplication

    //==========================================
    int numDotProducts = n * p;
    int dotProductsPerRank = numDotProducts / numranks;
    
    //Create buffers for the number of dot products assigned to each rank, and the displacements for those dot products, and the send storage.
    int* C_sendcounts = (int*) malloc(numranks * sizeof(int));
    int* C_displs = (int*) malloc(numranks * sizeof(int));
    
    for(int i = 0; i < numranks; i++){
        int sendcount = (i == numranks - 1) ? numDotProducts - (dotProductsPerRank * i) : dotProductsPerRank;
        int displs = i * dotProductsPerRank;
        C_sendcounts[i] = sendcount;
        C_displs[i] = displs;
    }
    transposeMatrix(B, m, p);

    // //TODO: Optimize sending of A and B with MPI_Scatterv.
    int* A_sendcounts = (int*) malloc(numranks * sizeof(int));
    int* A_displs = (int*) malloc(numranks * sizeof(int));
    
    for(int i = 0; i < numranks; i++){
        int sendcount = (C_sendcounts[i]%p > 0) ? m * ((C_sendcounts[i])/p + 2) : m * ((C_sendcounts[i])/p);
        A_sendcounts[i] = sendcount;
    }
    A_displs[0] = 0;
    for(int i = 1; i < numranks; i++){
        int displs = ((C_sendcounts[i])%p > 0) ? A_displs[i-1] + A_sendcounts[i] - m : A_displs[i-1] + A_sendcounts[i] ;
        A_displs[i] = displs;
    }


    MPI_Scatterv(A,sendcounts,disp,MPI_INT,mya,sendcounts[rank],MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(B, m*p, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    

    //Each rank computes its assigned dot products, storing the results in C_buf. 
    double* C_buf = (double*) malloc(C_sendcounts[rank] * sizeof(double));
    //The index of the dot product corresponds to the row of A and column of B that are used for that dot product.
    for(int j = C_displs[rank]; j < C_displs[rank] + C_sendcounts[rank]; j++){
        double result = 0;
        int dotProductIndex = j;
        int col = dotProductIndex % p;
        int row = (dotProductIndex -col) / n;
        
        //make proper vectors to pass to the dot product function, which expects two vectors of size m.
        //because B has been transposed, we can treat the column of B as a row vector.
        dotProduct(&A[row*m], &B[col*m], &result, m);

        C_buf[j - C_displs[rank]] = result;
    }
    
    //send the buffers back to rank 0, which will compose the final matrix C from the results.
    MPI_Gatherv(C_buf, C_sendcounts[rank], MPI_DOUBLE, C, C_sendcounts, C_displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    free(C_sendcounts);
    free(C_displs);
    free(C_buf);

    //==========================================

    if(rank == 0){
        printf("Factor Matrix A ( %d x %d ):\n", n, m);
        printMatrix(A, n, m);

        printf("Factor Matrix B ( %d x %d ):\n", m, p);
        printMatrix(B, m, p);

        printf("Transpose Matrix B ( %d x %d ):\n", p, m);
        transposeMatrix(B, m, p);
        printMatrix(B, p, m);

        printf("Product Matrix C ( %d x %d ):\n", n, p);
        printMatrix(C, n, p);

        free(A);
        free(B);
        free(C);
    }

    MPI_Finalize();
    
    return 0;
}

