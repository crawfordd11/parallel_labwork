#include <stdio.h>
#include <omp.h>

int main(int argc, char** argv) {
  int x=0;
  #pragma omp parallel
  {
        //int id = omp_get_thread_num();
        //int total = omp_get_num_threads();
	//printf("%2d    %d\n", id, total);
    	x=x+1;
  }
  printf("X: %d\n",x);

  x=0;

  #pragma omp parallel
  {
        //int id = omp_get_thread_num();
        //int total = omp_get_num_threads();
	//printf("%2d    %d\n", id, total);
  	#pragma omp critical 
	{
    		x=x+1;
	}
  }
  printf("X: %d\n",x);

  x=0;

  #pragma omp parallel
  {
	#pragma omp atomic 
	  x=x+1;
  }
  printf("X: %d\n",x);


  #pragma omp parallel
  {
    #pragma omp sections
    {
        #pragma omp section
        {
            printf("Thread %d is doing Task A\n", omp_get_thread_num());
            //do_task_a();
        }

        #pragma omp section
        {
            printf("Thread %d is doing Task B\n", omp_get_thread_num());
            //do_task_b();
        }
    }
  }

  #pragma omp parallel for
	for (int i = 0; i < 10; i++) {
    		//results[i] = heavy_calculation(data[i]);
        	int id = omp_get_thread_num();
		printf("%2d   %d\n", id, i);
	}

    int n = 1000;
    int data[1000];
    int total_sum = 0;

    // Initialize the array
    for (int i = 0; i < n; i++) {
        data[i] = 1;
    }

    // Using the reduction clause
    // syntax: reduction(operator:variable)
    #pragma omp parallel for reduction(+:total_sum)
    for (int i = 0; i < n; i++) {
        total_sum += data[i];
    }

    printf("The total sum is: %d\n", total_sum);
    return 0;
}
