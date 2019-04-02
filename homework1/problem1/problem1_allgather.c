/*
MPI_Allgather function usage for comparison
The gathered message is the 100*id of the processor.
Author: Gang Chen
*/

#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
	int i, id, p;
	double time;

	MPI_Init(NULL, NULL);
	MPI_Barrier(MPI_COMM_WORLD);
	time = -MPI_Wtime();

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	MPI_Status status;

	int msg = 100 * id;

	int *gathered_array = (int *)malloc(sizeof(int) * p);
	MPI_Allgather(&msg, 1, MPI_INT, gathered_array, 1, MPI_INT, MPI_COMM_WORLD);

	time += MPI_Wtime();
	printf("time = %lf\n", time);

	MPI_Finalize();
	return 0;
}