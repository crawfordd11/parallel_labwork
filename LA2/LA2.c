#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char **argv){
    int numranks;
    int rank;
    MPI_Status stat;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numranks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int number=0;
    printf("Rank: %d, Number: %d\n",rank,number);
    if(rank==0){
        number=100;
        MPI_Send(&number,1,MPI_INT,1,0,MPI_COMM_WORLD);
        MPI_Recv(&number,1,MPI_INT,1,0,MPI_COMM_WORLD,&stat);
        printf("Rank %d, Number: %d\n",rank,number);
    }
    if(rank==1){
        MPI_Recv(&number,1,MPI_INT,0,0,MPI_COMM_WORLD,&stat);
        printf("Rank %d, Number: %d\n",rank,number);
        number++;
        MPI_Send(&number,1,MPI_INT,0,0,MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;    
}
