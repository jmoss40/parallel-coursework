
/*
 * CCalPi_pthreads.c
 * 
 * Jordan Moss
 * 
 * How to compile:
 *   gcc -g -Wall -o CCalPi_pthreads CCalPi_pthreads.c -lm -lpthread
 * How to run:
 *   ./CCalPi_pthreads {thread_count}
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>

void* computingPi(void* rank);

double* sums = NULL;
long n = 100000;
long thread_count = 0;
long num_levels = 0;
long x, active_threads;
long* counters;
sem_t* count_sems;
sem_t* barrier_sems;

int main(int argc, char* argv[]) {
    thread_count = strtol(argv[1], NULL, 10);    
    pthread_t* thread_manager = malloc(thread_count * sizeof(pthread_t));
    num_levels = ceil(log(thread_count) / log(2)) + 1;

    counters = malloc(num_levels * sizeof(long));
    for(long x = 0; x < num_levels; x++){
	counters[x] = 0;
    }

    sums = malloc(thread_count * sizeof(double));
    for(long x = 0; x < thread_count; x++){
	sums[x] = 0;
    }

    count_sems = malloc(num_levels * sizeof(sem_t));
    barrier_sems = malloc(num_levels * sizeof(sem_t));
    
    for(long i = 0; i < num_levels; i++){
	sem_init(&count_sems[i], 0, 1);
	sem_init(&barrier_sems[i], 0, 0);
    }

    for(long i = 0; i < thread_count; i++){
	pthread_create(&thread_manager[i], NULL, computingPi, (void*)i);
    }

    for(long i = 0; i < thread_count; i++){
	pthread_join(thread_manager[i], NULL);
    }    

    printf("Pi is equal to %f\n", sums[0]);
    
    free(thread_manager);
    free(counters);
    free(sums);
 
    for(long i = 0; i <= num_levels; i++){
	sem_destroy(&count_sems[i]);
	sem_destroy(&barrier_sems[i]);
    }

    return EXIT_SUCCESS;
}

void* computingPi(void* rank){
    long my_rank = (long) rank;
    double factor;
    long level = 0;
    long my_n = n/thread_count;
    long my_first_i = my_n * my_rank;
    long my_last_i = my_first_i + my_n;
    
    for (long i = my_first_i; i < my_last_i; i++){
	factor = (i % 2 == 0) ? 1.0 : -1.0;
	sums[my_rank] += 4 * factor / (2*i+1);
    }
    
    for(long i = 1; i < thread_count; i*=2){
	x = (level==0) ? 1 : (2*level);
	if(my_rank % x == 0){
	    active_threads = (level==0) ? thread_count : floor(thread_count / (2*level));
	    sem_wait(&count_sems[level]);
	    if (counters[level] == active_threads-1){
		counters[level] = 0;
		sem_post(&count_sems[0]);
		for(long j = 0; j < active_threads-1; j++){
		    sem_post(&barrier_sems[level]);
		}
	    } else {
		counters[level]++;
		sem_post(&count_sems[level]);
		sem_wait(&barrier_sems[level]);
		printf("In level %ld, %li is released from waiting...\n", level, my_rank);
	        fflush(stdout);
	    }
	    
	    if(my_rank == 0){
		printf("%ld threads completed barrier %li \n", active_threads, level);
		fflush(stdout);
	    }

	    if(my_rank % (2*i) == 0){
		sums[my_rank] += sums[my_rank + i];
	    }
	}
	level++;
    }
    return NULL;
}

