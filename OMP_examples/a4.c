#include <omp.h>
#include <stdio.h>
#include <unistd.h>  // for usleep()

void work(int n){
    int num=n*100000;
    int a=0;
    for(int i=0;i<num;i++){
        a=a*2+1;
    }
}
int main(int argc, char *argv[]){
		// ONE THREAD CREATES TASKS THAT ARE EXECUTED BY THE OTHER THREADS
    int count=10;
    #pragma omp parallel
    {
        int tid=omp_get_thread_num();
        #pragma omp single
        {
            for(int i=0;i<count;i++){
                #pragma omp task
                {
                    printf("Thread %d doing work...\n",omp_get_thread_num());
                    work(i);
                }
            }
        } // End of single
    } // End of parallel region


    		// SCHEDULE (static) - this is the default
		//
		// It divides the total number of iterations into equal-sized 
		// "chunks" and assigns them to threads in a round-robin 
		// fashion **before** the loop even starts.
		//
		// In this example, we specify a chunk_size of 2. This means 
		// Thread 0 gets iterations 0-1, Thread 1 gets 2-3, and so on.
    int n = 12;
    int chunk_size = 2;

    #pragma omp parallel for schedule(static, chunk_size) num_threads(3)
    for (int i = 0; i < n; i++) {
        int tid = omp_get_thread_num();
        printf("Thread %d is processing iteration %d\n", tid, i);
    }


    

    		// SCHEDULE (dynamic)
		// perfect for irregular workloads, where you don't 
		// know how long each iteration will take until you're actually doing it
     n = 10;
    // We use a chunk size of 1 so threads grab one iteration at a time
    #pragma omp parallel for schedule(dynamic, 1) num_threads(3)
    for (int i = 0; i < n; i++) {
        int tid = omp_get_thread_num();
        
        // Simulate unpredictable work: even numbers are fast, odd are slow
        if (i % 2 == 0) {
            usleep(100); // Fast
        } else {
            usleep(500000*10); // Slow (0.5 seconds)
        }

        printf("Thread %d finished iteration %d\n", tid, i);
    }


    		// SCHEDULE (guided)
		// It starts by handing out large chunks of work to minimize overhead, but as the 
		// remaining work decreases, it dynamically shrinks the chunk size to ensure no 
		// thread gets stuck with a massive task at the very end.
		// The formula for the chunk size is roughly:  remaining_iterations/number_of_threads
    n = 100;
    int min_chunk = 1;

    // 'guided' starts large and shrinks down to 'min_chunk'
    #pragma omp parallel for schedule(guided, min_chunk) num_threads(4)
    for (int i = 0; i < n; i++) {
        // Simulating work
        if (i == 0) {
            int tid = omp_get_thread_num();
            printf("Thread %d started with a large chunk near iteration %d\n", tid, i);
        }

        if (i == n-1) {
            int tid = omp_get_thread_num();
            printf("Thread %d finished the final small chunks at iteration %d\n", tid, i);
        }
    }



    return(0);
}

