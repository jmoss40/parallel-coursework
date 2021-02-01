/*
 * CCalPi_openmp.c
 * 
 * To Compile:
 *   gcc -g -Wall -o CCalPi_openmp CCalPi_openmp -fopenmp
 * 
 * To Run: 
 *   ./CCalPi_openmp {thread_count}
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

long n = 100000;
double global_sum = 0.0;
long thread_count;
void computingPi();

int main(int argc, char* argv[]) {
    thread_count = strtol(argv[1], NULL, 10);

    #pragma omp parallel num_threads(thread_count)
    computingPi();

    printf("Pi is equal to %f\n", global_sum);

    return EXIT_SUCCESS;
}

void computingPi(){
    long my_rank = omp_get_thread_num(); 
    double factor;
    double local_sum = 0.0;
    long my_n = n/thread_count;
    long my_first_i = my_n * my_rank;
    long my_last_i = my_first_i + my_n;
    
    for (long i = my_first_i; i < my_last_i; i++){
        factor = (i % 2 == 0) ? 1.0 : -1.0;
        local_sum += 4 * factor / (2*i+1);
    }
    #pragma omp critical
    global_sum += local_sum;
}
