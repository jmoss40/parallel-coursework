/*
 * histogram_pthreads.c
 * 
 * How to compile:
 *   gcc -g -Wall -o histogram_pthreads histogram_pthreads.c my_rand.c
 * How to run:
 *   ./histogram_pthreads {numBins} {numValues} {thread_count}
 *   Ex: ./histogram_pthreads 10 100 5
 * 
 * Jordan Moss
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "my_rand.h"
#include <math.h>

long *histogram;
long *thread_range; 
float min, max, binRange;
long numBins, numValues, thread_count;
unsigned seed, x;
pthread_mutex_t mutex;

void* parallel_histogram(void* thread_id);

int main(int argc, char* argv[]) {
    //if an incorrect number of arguments given, print correct
    //usage and end the program
    if (argc <= 1){
        printf("Usage: histogram_pthreads numBins numValues thread_count\n");
        return 1;
    }
    
    //numBins, numValues, and thread_count taken in as arguments
    numBins = strtod(argv[1], NULL);
    numValues = strtod(argv[2], NULL);
    thread_count = strtod(argv[3], NULL);
    
    //create a seed for the my_rand function
    seed = time(0);
    x = my_rand(&seed);

    //calculate the min value and max value
    min = 0.0;
    max = (my_rand(&x) % 100 / 10.0) + min;
    
    pthread_mutex_init(&mutex, NULL);
    
    //create the histgramPtr and initialize all values to 0
    histogram = malloc(numBins * sizeof(long));
    for (int i = 0; i < numBins; i++){
        *(histogram + i) = 0;
    }

    pthread_t* threadsPtr = malloc(thread_count * sizeof(pthread_t));
    
    //calculate how many data values each thread should process
    thread_range = malloc(thread_count * sizeof(long));
    int val = numValues / thread_count;
    int diff = ceil((val * thread_count) - numValues);
    for(int i = 0; i < (thread_count-1); i++){
	*(thread_range + i) = val;
    }
    *(thread_range + (thread_count-1)) = val - diff;

    //start the threads
    for(long i = 0; i < thread_count; i++){
        pthread_create(&threadsPtr[i], NULL, parallel_histogram, (void*)i);
    }

    //join the threads
    for(long i = 0; i < thread_count; i++){
        pthread_join(threadsPtr[i], NULL);
    }
    
    //print the contents of the frequency array
    for (int i = 0; i < numBins; i++){
        printf("The frequency in bin %d is %li\n", i, histogram[i]);
    }
   
    free(thread_range);
    free(histogram);
    pthread_mutex_destroy(&mutex);
    return 0;
}

void* parallel_histogram(void* thread_id){
    binRange = (max - min + 1) / numBins;
     
    for (int i = 0; i < thread_range[(long)thread_id]; i++){
        float value = my_drand(&x) * max + min; //generate a value
        for(int j = 0; j < numBins; j++){
            if((value > (min + (binRange * j))) && value < (min + (binRange * (j+1)))){
                pthread_mutex_lock(&mutex);
                (*(histogram + j))++; //increment the bin that the value is in
                pthread_mutex_unlock(&mutex); 
            }
        }
    }
    return NULL;
}
