#include <stdio.h>
#include <omp.h>

int main() {
    int shared_val = 100;    // Accessible by all
    int first_val = 10;      // To be inherited by each thread
    int last_val = 0;        // To capture the final loop state
    int private_val;         // Each thread's "scratchpad"
    int results[8] = {0};

    // Use 'default(none)' to force us to explicitly define every variable
    #pragma omp parallel for \
        default(none) \
        shared(results, shared_val) \
        firstprivate(first_val) \
        lastprivate(last_val) \
        private(private_val)
    for (int i = 0; i < 8; i++) {
        // 1. private_val starts as garbage; we must initialize it inside
        private_val = i * 2; 

        // 2. first_val started at 10 for every thread because of firstprivate
        // 3. shared_val is the same memory address for everyone
        results[i] = first_val + private_val + shared_val;

        // 4. last_val will be updated. Because of 'lastprivate', the value 
        // from the iteration i=7 will persist after the loop.
        last_val = results[i];
        
        printf("Thread %d processed i=%d | res=%d\n", 
                omp_get_thread_num(), i, results[i]);
    }

    printf("\n--- Results ---\n");
    printf("Final last_val (from last iteration): %d\n", last_val);
    printf("Original first_val (unchanged): %d\n", first_val);
    ///////////////////////////////////////////////////////////////////////////////////

    // Only go parallel if n is large enough to justify the overhead
    // Otherwise, a single thread will execute.
    n=4
    #pragma omp parallel if(n > 5)
    {
        printf("My id: %2d\n", omp_get_thread_num());
    }
    ///////////////////////////////////////////////////////////////////////////////////


    int constant = 10;
    int result = 0;
    int i;

    #pragma omp parallel default(shared) private(i)
    {
    // 'i' is private because it was explicitly listed.
    // 'constant' is shared because of the default(shared) rule.
    // 'result' is shared because of the default(shared) rule.
    
    i = omp_get_thread_num();
    result += (i * constant); // DANGER: 'result' is shared; this is a race condition!
    }





    return 0;
}
