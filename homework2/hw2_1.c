/*
2.1 Using OpenMP to implement the Monte Carlo algorithm
E.g. calculating PI using the Monte Carlo algorithm
Compile : gcc -std=c99 -fopenmp -o hw2_1 hw2_1.c
run e.g.: ./hw2_1 8 1000000
*/

#include<omp.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>

int main(int argc,char** argv){

    long long int num_in_cycle=0;
    long long int num_point;
    int thread_count;

    thread_count=strtol(argv[1],NULL,10);
    num_point=strtol(argv[2],NULL,10);

	printf("thread_count=%d \n", thread_count);
    printf("num_point=%lld \n", num_point);

    srand(time(NULL));

    double x,y,distance_point;
    long long int i;
    #pragma omp parallel for num_threads(thread_count) default(none) \
        reduction(+:num_in_cycle) shared(num_point) private(i,x,y,distance_point)
    
    for( i=0;i<num_point;i++){
        x=(double)rand()/(double)RAND_MAX;
        y=(double)rand()/(double)RAND_MAX;
        distance_point=x*x+y*y;
        if(distance_point<=1){
            num_in_cycle++;
        }
    }
    double estimate_pi=(double)num_in_cycle/num_point*4;
    printf("Pi is %lf\n", estimate_pi);
    return 0;
}



