#include "LA5.h"



int main(int argc, char** argv){

    int rank,numranks;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numranks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Status stat;

    double startfull=MPI_Wtime();
   
    /*
        Master:
        Send a small range of values to each worker, the “chunk” size. 
        Wait to Recv the results from anyone.
        Add to the running total.
        IF there is no more work to do, send an invalid range to the worker. 
        Send a new range back to the worker. 
        IF all workers are finished, proceed to the end of the program. 
        Repeat.

        Worker: 
        Recva small range of work from master. 
        IF master gives an invalid range, continue to the end of the program.
        Compute the number of primes in this range.
        Sendthe number of primes back to master.
        Repeat.
    */

    int n=10000000;// there should be 664579 primes less than or equal to 10 million
    int numprimes = 0;
    int chunk_size = n/numranks; //arbitrary chunk size, can be tuned for performance
    int total_chunks = (n + chunk_size - 1) / chunk_size; // ceiling division to cover all numbers
    int chunk_start = 0;
    int chunk_end = chunk_size;
    int TERMINATE_SIGNAL = -1;
    
    if(rank == 0) //master
    {
        if(numranks == 1 || chunk_size >= n)
        {
            numprimes = count_primes_in_range(0,n);
        }
        else
        {
            int local_count;
            // Send initial chunks to all workers
            for (int w = 1; w < numranks; w++) {
                MPI_Send(&chunk_start, 1, MPI_INT, w, 0, MPI_COMM_WORLD);
                MPI_Send(&chunk_end,   1, MPI_INT, w, 0, MPI_COMM_WORLD);
                chunk_start = chunk_end + 1; // advance to next chunk
                chunk_end = chunk_start + chunk_size - 1;
                if (chunk_end > n) chunk_end = n; // cap at n
            }

            // Collect results and send more work dynamically
            for (int sent = 0; sent < total_chunks; sent++) {
                MPI_Recv(&local_count, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &stat);
                numprimes += local_count;

                if (chunk_start <= n ) { // still have more chunks to send
                    MPI_Send(&chunk_start, 1, MPI_INT, stat.MPI_SOURCE, 0, MPI_COMM_WORLD);
                    MPI_Send(&chunk_end,   1, MPI_INT, stat.MPI_SOURCE, 0, MPI_COMM_WORLD);
                    chunk_start = chunk_end + 1; // advance to next chunk
                    chunk_end = chunk_start + chunk_size - 1;
                    if (chunk_end > n) chunk_end = n; // cap at n
                } else {
                    // No more work, send termination signal (e.g., negative range)
                    MPI_Send(&TERMINATE_SIGNAL, 1, MPI_INT, stat.MPI_SOURCE, 0, MPI_COMM_WORLD);
                }
            }
        }
        double endfull=MPI_Wtime();
        printf("Number of Ranks: %d\n",numranks);
        printf("Chunk Size: %d\n",chunk_size);
        printf("Expected Number of Primes: 664579\n");
        printf("Number of Calculated Primes: %d\n",numprimes);
        printf("Full time taken: %f seconds\n", endfull - startfull);
    }
    else //worker
    {
        int local_count;
        while (1) {
            MPI_Recv(&chunk_start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &stat);
            if (chunk_start == TERMINATE_SIGNAL) break; // termination signal from master
            MPI_Recv(&chunk_end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &stat);
            local_count = count_primes_in_range(chunk_start, chunk_end);
            MPI_Send(&local_count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();

}

int count_primes_in_range(int start, int end)
{
    int count = 0;
    for (int i = start; i <= end; i++)
    {
        if (is_prime(i)) count++;
    }
    return count;
}

int is_prime(int n)
{
    /* handle special cases */
    if      (n == 0) return 0;
    else if (n == 1) return 0;
    else if (n == 2) return 1;
    else if (n == 3) return 1;
    else if (n % 2 == 0) return 0;
    else if (n % 3 == 0) return 0;

    //this could cause multiple problems: omp parallel for private(i) schedule(dynamic)
    for(int i=5;i<=(int)(sqrt((double) n));i+=6)
    {
        if (n%i==0) return 0;
        if (n%(i+2)==0) return 0;
    }
        
    return 1;
}
