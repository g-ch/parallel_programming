/*
2.1 Implement the matrix multiplication function.
Generate random matrix and do matrix multiplication AxB=C
based on the Rowwise Block-Striped Parallel method.
Compile : mpicc -o problem2-1 problem2-1.c
run e.g.: mpirun -np 8 ./problem2-1
Author: Gang Chen
*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Creates an array of random numbers. Each number has a value from 0 - 1
float *create_rand_nums(int num_elements) {
	float *rand_nums = (float *)malloc(sizeof(float) * num_elements);
	assert(rand_nums != NULL);
	int i;
	for (i = 0; i < num_elements; i++) {
		rand_nums[i] = (rand() / (float)RAND_MAX);
	}
	return rand_nums;
}

int main(int argc, char** argv) {
	int id, p;
	double time;

  	int N = 1024;
  	int circle = 0;
  	float *A, *B, *C;

	MPI_Init(NULL, NULL);
	MPI_Barrier(MPI_COMM_WORLD);
	time = -MPI_Wtime();

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	if(id==0)
	{
	   	C = (float*)malloc(sizeof(float)*N*N);
	   	B = create_rand_nums(N*N);
    	A = create_rand_nums(N*N);
	}

	int len = N / p;
  	float *localA = (int*)malloc(sizeof(int)*len*N);
  	float *localB = (int*)malloc(sizeof(int)*len*N);
  	float *localC = (int*)malloc(sizeof(int)*len*N);
  	float *temp   = (int*)malloc(sizeof(int)*len*N);

  	MPI_Scatter(A, len*N, MPI_INT, localA, len*N, MPI_INT, 0, MPI_COMM_WORLD);
  	MPI_Scatter(B, len*N, MPI_INT, localB, len*N, MPI_INT, 0, MPI_COMM_WORLD);

	int index = id;
	int dest = id==0 ? p-1 : id-1;
	int source = (id+1) % p;
	int cnt = 0;

	while(circle < p)
	{
		for(int i = 0; i < len; i++){
      		for(int j = 0; j < len; j++){
        		cnt = j*N + (index*len+i);
        		localC[cnt] = 0;
        		for(int k = 0; k <N; k++)
          			localC[cnt] += localA[N*i+k] * localB[N*j+k];
      		}
		}

		if(id == 0)
		{
			MPI_Ssend(localA, len*N, MPI_INT, dest, 0, MPI_COMM_WORLD);
	      	MPI_Recv(localA, len*N, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	  	}
		else
		{
			memcpy(temp, localA, sizeof(int)*len*N);
			MPI_Recv(localA, len*N, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Ssend(temp, len*N, MPI_INT, dest, 0, MPI_COMM_WORLD);
		}
	    circle++;
    	index = (index+1) % p;
	}

	MPI_Gather(localC, len*N, MPI_INT, C, len*N, MPI_INT, 0, MPI_COMM_WORLD);

	time += MPI_Wtime();
	printf("time = %lf\n", time);

	if(id == 0)
	{
    	printf("\n the Matrix of A:\n");
    	int i;
    	for(i = 0; i < N*N; i++)
      		printf("%f\t", A[i]);

      	printf("\n the Matrix of B:\n");
    	for(i = 0; i < N*N; i++)
      		printf("%f\t", B[i]);

      	printf("\n the Matrix of C:\n");
    	for(i = 0; i < N*N; i++)
      		printf("%f\t", C[i]);	
  	}


	MPI_Finalize();
	return 0;
}