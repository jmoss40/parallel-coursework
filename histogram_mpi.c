/*
 * histogram_mpi.c
 * 
 * How to compile:
 *   mpicc -g -Wall -o histogram_mpi histogram_mpi.c my_rand.c
 * How to run:
 *   mpiexec -n {number of processes} ./histogram_mpi
 *   Ex: mpiexec -n 2 ./histogram_mpi
 * 
 * Jordan Moss
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "my_rand.h"
#include <math.h>
#include <time.h>

int main(void) { 
    int my_rank, process_count;
    int numBins, numValues;
    double min, max, binRange;
    int *histogram, *my_histogram;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if(my_rank == 0){
	//get the number of bins and number of values to generate
	puts("Please enter the number of bins to use: ");
	scanf("%d", &numBins);
	puts("\nPlease enter the number of values to generate: ");
	scanf("%d", &numValues);
	
	//create a global histogram and initialize the contents to 0
	histogram = malloc(numBins * sizeof(int));
	for (int i = 0; i < numBins; i++)
	    *(histogram + i) = 0;

	//calculate the min value and max value
	min = 0.0;
	max = drand(&x) * 10.0;
	binRange = (max - min) / numBins;
    }

    MPI_Bcast(&numBins,   1, MPI_INT,    0, MPI_COMM_WORLD);
    MPI_Bcast(&numValues, 1, MPI_INT,    0, MPI_COMM_WORLD);
    MPI_Bcast(&max,       1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&min,       1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&binRange,  1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  
    //create a seed for the my_drand function
    unsigned seed = time(0);
    unsigned x = my_rand(&seed);
    x = my_rand(&x);

    //Create a local histogram for each process and initialize the contents to 0     
    my_histogram = malloc(numBins * sizeof(int));
    for(int i = 0; i < numBins; i++)
	*(my_histogram + i) = 0;
  
    //Calculate the number of values for this thread to generate
    int my_amount = numValues / process_count;
    if(my_rank == 0){my_amount += numValues % process_count;}
    
    //Each process generates its number of random values, and calculates its local histogram
    for(int i = 0; i < my_amount; i++) {
	double value = my_drand(&x) * max + min; //generate a value
	for(int j = 0; j < numBins; j++) {
	    if(value >= min + (binRange * j) && value < min + (binRange * (j+1))) {
		++*(my_histogram + j); //increment the bin that the value falls into
	    }
	}    
    }

    //Add the local histogram to the global histogram using MPI_Reduce
    MPI_Reduce(my_histogram, histogram, numBins, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); 

    //Print the contents of the frequency array
    if(my_rank == 0){
	for (int i = 0; i < numBins; i++)
	    printf("The frequency in bin %d is %d\n", i, *(histogram + i));
    }

    free(histogram);
    free(my_histogram);
    MPI_Finalize();
    return 0;
}

