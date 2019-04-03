/*
2.2 Implement the matrix max pool operation for Matrix A.
The result is in Matrix B
Compile : mpicc -o problem2-2 problem2-2.c
run: mpirun -np 8 ./problem2-2
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

int main(int argc, char** argv) 
{
    int id, p;
    double time;

    int N = 1024; // Matrix size
    int K_S = 4;  // Kernel size
    int circle = 0;
    float *A, *A_copied, *B, *B_copied;
    int len, len_exp;
    float *localA, *localB;

    MPI_Init(NULL, NULL);
    MPI_Barrier(MPI_COMM_WORLD);
    time = -MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    len = N / p;
    len_exp = len+K_S-1;

    if(id==0)
    {
        A = create_rand_nums(N*N);
        A_copied = (float*)malloc(sizeof(float)*(N + p*(K_S-1)) * N);

        // Allocate A_copied by adding 3(kernel_size=4) more lines for pooling or convolution
        for(int i=0; i<p; i++)
        {
            for(int j=0; j<len_exp; j++)
            {
                for(int k=0; k<N; k++)
                {
                    if((i*len+j) * N + k < N*N)
                        A_copied[(i*len_exp+j) * N + k] = A[(i*len+j) * N + k];
                    else
                        A_copied[(i*len_exp+j) * N + k] = 0;
                }
            }
        }

        B_copied = (float*)malloc(sizeof(float)* N *(N-K_S+1));
        B = (float*)malloc(sizeof(float)*(N-K_S+1)*(N-K_S+1));   
    }

    localA = (int*)malloc(sizeof(int) * len_exp * N);
    localB = (int*)malloc(sizeof(int) * len * (N-K_S+1));

    MPI_Scatter(A_copied, len_exp*N, MPI_FLOAT, localA, len_exp*N, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Max Pooling
    for(int l=0; l<len; l++)
    {
        for(int n=0; n<N-K_S+1; n++)
        {
            float temp_max = 0.f;
            for(int i=0; i<K_S; i++)
            {
                for(int j=0; j<K_S; j++)
                {
                    if(localA[l*N + n + N*i + j] > temp_max)
                        temp_max = localA[l*N + n + N*i + j];
                }
            }
            localB[l*(N-K_S+1) + n] = temp_max;
        }
    }

    MPI_Gather(localB, len*N, MPI_INT, B_copied, len*N, MPI_INT, 0, MPI_COMM_WORLD);
    time += MPI_Wtime();
    printf("time = %lf\n", time);

    if(id==0)
    {
        printf("\n the Matrix of A:\n");
        int i;
        for(i = 0; i < N*N; i++){
            printf("%f\t", A[i]);
        }

        printf("\n the Matrix of B:\n");
        for(int i=0; i<(N-K_S+1)*(N-K_S+1); i++){
            B[i] = B_copied[i];
            printf("%f\t", B[i]);
        }
    }

    MPI_Finalize();
    return 0;
}
