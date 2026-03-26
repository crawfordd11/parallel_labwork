#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int is_prime(int n);

int main(int argc, char** argv){

    int n=10000000;// there should be 664579 primes less than or equal to 10 million
    int numprimes = 2; //account for 2 and 3
    int i;
    #pragma omp parallel for private(i) reduction(+:numprimes)
    for (i = 5; i <= n; i+=6)
    {
        if (is_prime(i) == 1) numprimes ++;
        if (is_prime(i+2) == 1) numprimes ++;
    }

    printf("Number of Primes: %d\n",numprimes);

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

    //this would cause multiple problems: omp parallel for private(i) schedule(dynamic)
    for(int i=5;i<=(int)(sqrt((double) n));i+=6)
    {
        if (n%i==0) return 0;
        if (n%(i+2)==0) return 0;
    }
        
    return 1;
}
