
/*
 * CCalPi_mpi.c
 * 
 * Jordan Moss
 * 
 * How to compile:
 *   mpicc -g -Wall -o CCalPi_mpi CCalPi_mpi.c
 * How to run:
 *   mpiexec -n {process_count} CCalPi_mpi
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char* argv[]) {

    int process_count, my_rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    long n = 100000, tag = 0;
    double local_sum = 0, global_sum = 0;
    double factor, rec_sum;
    long my_n = n / process_count;
    long my_first_i = my_n * my_rank;
    long my_last_i = my_first_i + my_n;
    
    for (long i = my_first_i; i < my_last_i; i++){
        factor = (i % 2 == 0) ? 1.0 : -1.0;
        local_sum += 4 * factor / (2*i+1);
    }

    global_sum = local_sum;
    for(int stride = 1; stride < process_count; stride *= 2){
	if(my_rank % stride == 0){
	    int partner_rank = my_rank % (2 * stride) == 0 ? my_rank + stride : my_rank - stride;
	    if(my_rank % (2 * stride) == 0){
		MPI_Recv(&rec_sum, 1, MPI_LONG, partner_rank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		global_sum += rec_sum;
	    }else{
		MPI_Send(&global_sum, 1, MPI_LONG, partner_rank, tag, MPI_COMM_WORLD);
	    }
	    tag++;
	}
    }

    MPI_Finalize();

    if(my_rank == 0){
	printf("Pi is equal to %f\n", global_sum);
    }

    return EXIT_SUCCESS;
}



