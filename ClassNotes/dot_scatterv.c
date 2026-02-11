#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


int main(int argc, char** argv){

    int rank,numranks;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numranks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Status stat;

    double startfull = MPI_Wtime();

    int N;
    if(rank==0){
        N=1000;
    }

    MPI_Bcast(&N,1,MPI_INT,0,MPI_COMM_WORLD);

    // if(N%numranks !=0){
    //     if(rank==0){
    //         printf("Size not divisible by ranks: %d %d\n",N,numranks);
    //     }
    //     MPI_Finalize();
    //     return 0;
    // }

    int *a;
    int *b;
    double startalloc=MPI_Wtime();
    if(rank==0){
        a=(int*) malloc(N*sizeof(int));
        b=(int*) malloc(N*sizeof(int));
        for(int i = 0; i<N; i++){
            a[i]=b[i]=2;
        }
    }
    double endalloc=MPI_Wtime();

    int myN=N/numranks;
    //previoius version
    //int sendcounts=myN;
    int *sendcounts=(int*)malloc(numranks*sizeof(int));
    int *disp=(int*)malloc(numranks*sizeof(int));

    for(int i = 0; i<numranks; i++){
        sendcounts[i]=myN;
    }
    sendcounts[numranks-1]+=N-myN*numranks;

    printf("Rank %d: Numbr of Elements: %d\n",rank,sendcounts[rank]);

    disp[0]=0;
    for(int i = 1; i<numranks; i++){
        disp[i]=disp[i-1]+sendcounts[i-1];
    }

    int *mya=(int*) malloc(N*sizeof(int));
    int *myb=(int*) malloc(N*sizeof(int));

    double startscatter=MPI_Wtime();
    //MPI_Scatter(info to send, how many per rank, what kind, where do I recieve info, how many, what kind, from who, what world am I in)
    MPI_Scatterv(a,sendcounts,disp,MPI_INT,mya,sendcounts[rank],MPI_INT,0,MPI_COMM_WORLD);
    MPI_Scatterv(b,sendcounts,disp,MPI_INT,myb,sendcounts[rank],MPI_INT,0,MPI_COMM_WORLD);
    double endscatter=MPI_Wtime();

    double startcomp=MPI_Wtime();
    int result=0;
    for(int i=0; i<sendcounts[rank];i++){
        result+=mya[i]*myb[i];
    }
    double endcomp=MPI_Wtime();

    double startreduce=MPI_Wtime();
    //MPI_Reduce has a collector, but Allreduce sends to all
    MPI_Allreduce(MPI_IN_PLACE,&result,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
    double endreduce=MPI_Wtime();
    double endfull=MPI_Wtime();

    if(rank==0){
        printf("Rank: %d, Result: %d\n",rank,result);
        printf("Full time: %f\n",endfull-startfull);
        printf("Calc time: %f\n", endcomp-startcomp);
        printf("Alloc+Initalize time: %f\n", endalloc-startalloc);
        printf("Scatter time: %f\n",endscatter-startscatter);
        printf("Reduce time: %f\n",endreduce-startreduce);
    }

    MPI_Finalize();
    return 0;
}