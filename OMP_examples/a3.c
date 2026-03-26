#include <stdio.h>
#include <omp.h>

int main() {

		// FIRSTPRIVATE
    int threshold = 100;

    #pragma omp parallel firstprivate(threshold)
    {
        // Each thread gets its own 'threshold' initialized to 100
        int id = omp_get_thread_num();
        threshold += id; 
        printf("Thread %d has threshold value: %d\n", id, threshold);
    }

    // The original 'threshold' remains unchanged
    printf("Final value in main: %d\n", threshold);



    		// LASTPRIVATE

    int result = 0;

    // lastprivate ensures the value from the LAST iteration (i=4) is kept
    #pragma omp parallel for lastprivate(result)
    for (int i = 0; i < 5; i++) {
        int id = omp_get_thread_num();
        result = i * 10;
        printf("Thread %d processed i=%d, local result: %d\n", id, i, result);
    }

    // After the loop, result will be 40 (from the last iteration i=4)
    printf("Final value of result after loop: %d\n", result);


    		// SHARED	
    int shared_count = 0; // This exists in the main memory

    #pragma omp parallel shared(shared_count)
    {
        int id = omp_get_thread_num();

        // Every thread increments the SAME memory location
        // Using 'atomic' ensures they don't trip over each other
        #pragma omp atomic
        shared_count += 1;

        printf("Thread %d incremented the shared counter.\n", id);
    }

    // The final value reflects the work of ALL threads combined
    printf("Final shared counter value: %d\n", shared_count);

    		// DEFAULT
		// In OpenMP, the default clause allows you to define the data-sharing 
		// attributes of all variables in a parallel region at once, 
		// rather than listing every single one. This is a best practice 
		// for writing safe, bug-free code.
		//
		// In C/C++, the most common use is default(none), which **forces** you 
		// to explicitly declare whether every variable is 
		// shared, private, firstprivate, or reduction.
    int constant_val = 10;  // Should be shared (read-only)
    int offset = 5;         // Should be firstprivate (initial value needed)
    int total = 0;          // Should be shared (but needs protection)

    // default(none) forces us to categorize everything
    #pragma omp parallel default(none) shared(constant_val, total) firstprivate(offset)
    {
        int id = omp_get_thread_num();
        int local_calc = constant_val + offset + id;

        #pragma omp atomic
        total += local_calc;

        printf("Thread %d calculated: %d\n", id, local_calc);
    }

    printf("Final total: %d\n", total);


    		// NUMBER OF THREADS
		// the one we have seen in class is to export a variable in the shell:
		//     export OMP_NUM_THREADS=8
		//     If you use multiple methods, OpenMP follows this priority:
			// - Highest: The num_threads clause in the #pragma.
			// - Middle: The omp_set_num_threads() function call.
			// - Lowest: The OMP_NUM_THREADS environment variable.

    // This specific block will use exactly 4 threads
    #pragma omp parallel num_threads(4)
    {
        printf("Thread %d is working\n", omp_get_thread_num());
    }


    omp_set_num_threads(2); // Sets global default to 2
    #pragma omp parallel
    {
        printf("Thread %d is working\n", omp_get_thread_num());
    }

    		// BARRIER
    int n = 10;
    int data[10];
    int myresult[10];

    omp_set_num_threads(4); 
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();

        // STAGE 1: Each thread initializes its own part of the array  !!!! Hmmmm
        if (tid < n) {
            data[tid] = tid * 10;
            printf("\tThread %d initialized data[%d]=%d\n", tid, tid, data[tid]);
        }

        // --- THE BARRIER ---
        // We MUST wait here. If Thread 5 moves to Stage 2 before
        // Thread 4 or 6 finishes Stage 1, the average will be wrong.
        #pragma omp barrier

        // STAGE 2: Each thread uses data from neighbors
        if (tid > 0 && tid < n - 1) {
            myresult[tid] = (data[tid-1] + data[tid] + data[tid+1]) / 3;
            printf("\tThread %d computed average: %d\n", tid, myresult[tid]);
        }
    }

    		// NOWAIT
    n = 1000;
    double A[1000], B[1000], C[1000], D[1000];

    #pragma omp parallel
    {
        // Task 1: Process Array A into C
        #pragma omp for nowait
        for (int i = 0; i < n; i++) {
            C[i] = A[i] * 2.0;
        }
        // Without nowait, threads would wait here for everyone to finish C[i].
        // With nowait, a fast thread jumps immediately to the loop below.

        // Task 2: Process Array B into D (Independent of Task 1)
        #pragma omp for nowait
        for (int i = 0; i < n; i++) {
            D[i] = B[i] + 10.0;
        }

        // Final implicit barrier happens here at the end of the parallel region.
    }


    return 0;
}
