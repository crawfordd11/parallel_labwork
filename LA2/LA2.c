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

    

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numranks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    long long size = 1024*1024*1024/4;

    int *numbers = (int*)malloc(size*sizeof(int));

    //ping pong 

    printf("Number of ranks: %d", numranks);

    if(rank==0){
        numbers[0] = 0;
        
        start_time = MPI_Wtime();
        
        for(int i = 1; i<numranks; i++){
            MPI_Send(numbers,size,MPI_INT,i,0,MPI_COMM_WORLD);
            MPI_Recv(numbers,size,MPI_INT,i,0,MPI_COMM_WORLD,&stat);
        }
        
        end_time = MPI_Wtime();

        cpu_time_used = ((double) (end_time - start_time));
        printf("Execution ping pong time: %f seconds\n", cpu_time_used);
        printf("Execution ping pong time average: %f seconds\n", (cpu_time_used/(numranks*2)));
    }
    else{
        MPI_Recv(numbers,size,MPI_INT,0,0,MPI_COMM_WORLD,&stat);
        numbers[rank] = rank;
        MPI_Send(numbers,size,MPI_INT,0,0,MPI_COMM_WORLD);
    }

    //Ring time

    if(rank==0){
        numbers[0] = 0;
        
        start_time = MPI_Wtime();

        MPI_Send(numbers,size,MPI_INT,1,0,MPI_COMM_WORLD);
        MPI_Recv(numbers,size,MPI_INT,numranks-1,0,MPI_COMM_WORLD,&stat);
        
        
        end_time = MPI_Wtime();

        cpu_time_used = ((double) (end_time - start_time));
        printf("Execution ring time: %f seconds\n", cpu_time_used);
        printf("Execution ring time average: %f seconds\n", (cpu_time_used/(numranks)));
    }
    else{
        MPI_Recv(numbers,size,MPI_INT,rank-1,0,MPI_COMM_WORLD,&stat);
        numbers[rank] = rank;
        MPI_Send(numbers,size,MPI_INT,(rank+1)%numranks,0,MPI_COMM_WORLD);
    }

    free(numbers);
    
    MPI_Finalize();


    return 0;    
}
