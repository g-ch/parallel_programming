/*
Use MPI_Send and MPI_Recv to realize MPI_AllGather.
Process 0 gather all the information and send to all.
The gathered message is the 100*id of the processor.
Author: Gang Chen
*/

#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
	int i, id, p;
	double time;

	int* gathered_array;
	int send_msg;
	int recv_num, send_num;

	MPI_Init(NULL, NULL);
	MPI_Barrier(MPI_COMM_WORLD);
	time = -MPI_Wtime();

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	MPI_Status status;

	if(id == 0)
	{
		// Gather
		gathered_array = (int *)malloc(p*sizeof(int));
		gathered_array[0] = 0;
		for(recv_num = 1; recv_num < p; recv_num ++)
		{
			MPI_Recv(gathered_array+recv_num, 1, MPI_INT, recv_num, recv_num, MPI_COMM_WORLD, &status);
			// printf("Process 0 received msg %d from process %d. \n", gathered_array[recv_num], recv_num);
		}

		// printf("Process %d gathered array [", id);
		// for(int seq=0; seq<p; seq++)
		// 	printf("%d ", gathered_array[seq]);
		// printf("]\n");

		// Broadcast by send and receive
		for(send_num = 1; send_num < p; send_num ++)
		{
			MPI_Send(gathered_array, p, MPI_INT, send_num, 0, MPI_COMM_WORLD);
		}
	}
	else
	{
		send_msg = 100*id;
		MPI_Send(&send_msg, 1, MPI_INT, 0, id, MPI_COMM_WORLD);

		gathered_array = (int *)malloc(p*sizeof(int));
		MPI_Recv(gathered_array, p, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		// printf("Process %d received gathered array [", id);
		// for(int seq=0; seq<p; seq++)
		// 	printf("%d ", gathered_array[seq]);
		// printf("]\n");
	}
	time += MPI_Wtime();
	printf("time = %lf\n", time);

	MPI_Finalize();
	return 0;
}