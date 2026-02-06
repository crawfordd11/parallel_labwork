#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int main(int argc, char **argv){
    int numranks;
    int rank;
    MPI_Status stat;

    clock_t start_time, end_time;
    double cpu_time_used;

    start_time = clock();

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numranks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int number=0;
    printf("Rank: %d, Number: %d\n",rank,number);
    if(rank==0){
        number=100;
        for(int i = 1; i<numranks; i++){
            MPI_Send(&number,1,MPI_INT,i,0,MPI_COMM_WORLD);
            MPI_Recv(&number,1,MPI_INT,i,0,MPI_COMM_WORLD,&stat);
            printf("Rank %d, Number: %d\n",rank,number);
        }
        
        end_time = clock();

        cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
        printf("Execution time: %f seconds\n", cpu_time_used);
    }
    else{
        MPI_Recv(&number,1,MPI_INT,0,0,MPI_COMM_WORLD,&stat);
        printf("Rank %d, Number: %d\n",rank,number);
        number++;
        MPI_Send(&number,1,MPI_INT,0,0,MPI_COMM_WORLD);
    }

    MPI_Finalize();


    return 0;    
}
