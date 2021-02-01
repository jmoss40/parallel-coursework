
/*
 * histogram_openmp.c
 * 
 * To Compile:
 *   gcc -g -Wall -fopenmp -o histogram_openmp histogram_openmp.c my_rand.c
 * To Run:
 *   ./histogram_openmp {thread_count}
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "my_rand.h"
#include <math.h>
#include <time.h>

void calculateHistogram();

unsigned seed, x;
double bin_range, min, max;
long thread_count, num_bins, num_values;
long* histogram;

int main(int argc, char* argv[]){
    //get thread count from command line
    thread_count = strtol(argv[1], NULL, 10);
    
    //collect data values from user
    puts("Please enter the number of data values to randomly generate. ");
    scanf("%ld", &num_values);
    puts("Please enter the number of bins to use. ");
    scanf("%ld", &num_bins);

    //create a seed to use with my_drand function
    seed = time(0);
    x = my_rand(&seed);
    x = my_rand(&x);

    //calculate min, max, and bin_range
    min = 0.0;
    max = my_drand(&x) * 10.0;
    bin_range = (max - min) / num_bins;
    
    //create the histogram array and initialize to 0
    histogram = malloc(num_bins * sizeof(long));
    for(long i = 0; i < num_bins; i++)
	*(histogram + i) = 0;

    #pragma omp parallel num_threads(thread_count)
    calculateHistogram();

    //print the histogram
    for(long i = 0; i < num_bins; i++)
	printf("The frequency in bin %ld is %ld\n", i, *(histogram + i));

    free(histogram);
}

void calculateHistogram(){
    //determine how many data values this thread should generate
    long my_rank = omp_get_thread_num();
    long my_amount = num_values / thread_count;
    if(my_rank == 0){my_amount += num_values % thread_count;}
    
    for(long i = 0; i < my_amount; i++){
        float value = my_drand(&x) * max + min; //generate a value
        for(long j = 0; j < num_bins; j++){
            if((value >= (min + (bin_range * j))) && value < (min + (bin_range * (j+1)))){
		#pragma omp critical
                (*(histogram + j))++; //increment the bin that the value is in
                
            }
        }
    } 
}
