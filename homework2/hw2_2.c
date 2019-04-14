/*
2.2 Using OpenMP to implement a quick sorting algorithm
Initialize a random int array with the size of 1000000 and quick sort with OpenMP
Compile : gcc -std=c99 -fopenmp -o hw2_2 hw2_2.c
run e.g.: ./hw2_2 8
*/

#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>

#define SIZE 1000000

int arr[SIZE];

void swap(int* a, int* b)
{
	int t = *a;
	*a = *b;
	*b = t;
}

int partition (int arr[], int low, int high)
{
	int pivot = arr[high]; 
	int i = (low - 1); // Index of smaller element
	for (int j = low; j <= high- 1; j++)
	{
		if (arr[j] <= pivot)
		{
			i++; // increment index of smaller element
			swap(&arr[i], &arr[j]);
		}
	}
	swap(&arr[i + 1], &arr[high]);
	return (i + 1);
}


void quickSort(int arr[], int low, int high)
{
	if (low < high)
	{
		
		int pi = partition(arr, low, high);

		#pragma omp task firstprivate(arr,low,pi)
		{
			quickSort(arr,low, pi - 1);
		}

		#pragma omp task firstprivate(arr, high,pi)
		{
			quickSort(arr, pi + 1, high);
		}
	}
}

void print_array(int a[], int n)
{
    int i;
    for(i =0 ; i<n ;i++)
    {
        printf("%d  ", a[i]);
    }
}


int main(int argc,char** argv)
{
	int thread_count;
    thread_count=strtol(argv[1],NULL,10);
	printf("thread_count=%d \n", thread_count);

	double start_time, run_time;
	for( int i = 0; i < SIZE-1; i++ )
    {
       arr[i] = rand() % SIZE +1;
    }
	int n = sizeof(arr)/sizeof(arr[0]);

	omp_set_num_threads(thread_count);
	start_time = omp_get_wtime();

	#pragma omp parallel
 	{
	 	int id = omp_get_thread_num();
	    int nthrds = omp_get_num_threads();

		#pragma omp single nowait
		quickSort(arr, 0, n-1);
	}

	run_time = omp_get_wtime() - start_time;
	printf("Time = %lf seconds\n ",run_time);

  	// print_array(arr, SIZE);

	return 0;
}