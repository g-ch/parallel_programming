/*
3.2 Count words in a large file
Process 0 is a manager who reads the file line by line and allocates to the free workers.
Other Processes play the role of worker who reveive lines from manager and send counted results.
In the begining the workers send a initial signal.
When all the lines are processed, the manager sends a exit signal "sss" to all the workers and exits itself. 

Compile : mpic++ -o large_file large_file.cpp
run e.g.: mpirun -np 8 ./large_file
Author: Gang Chen
*/

#include <mpi.h>
#include <iostream>
#include <dirent.h>
#include <cstring>    
#include <vector>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

int main(int argc, char** argv) 
{
	int id, p, n;
	double time;
	const char *file = "/home/epcc-course/Example/Homework/Big_file/big_100.txt";

	MPI_Init(NULL, NULL);
	MPI_Barrier(MPI_COMM_WORLD);
	time = -MPI_Wtime();

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	if(p < 2)
	{
		cout << "Should use more than 2 processes!" << endl;
		MPI_Finalize();
		return 0;
	}

	if(id==0) //Reader, manager
	{
		MPI_Status status;
		long int words_num_total = 0;
		int src;

		// Read file
		fstream in;
		in.open(file, ios::in);
		
	    if(in.fail())
    	{
        	cout << "failed to open the file!" << endl;
        	MPI_Finalize();
        	return 0;
    	}

		string tmp;
		int void_request_num = 0;
		int lines_num_counter = 0;

        while(getline(in, tmp))
        {
            /* Send to a free worker */ 
        	int words_num_local;
            MPI_Recv(&words_num_local, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
            
            if(words_num_local >= 0) {
            	words_num_total += words_num_local;
            }else {
            	void_request_num ++;
            }

            src = status.MPI_SOURCE;

            const char* temp_char = tmp.data();

			MPI_Send(temp_char, tmp.size(), MPI_CHAR, src, 0, MPI_COMM_WORLD);
			lines_num_counter ++;
        }
    	in.close();
    	cout << "lines_num = " << lines_num_counter << endl;
    	// cout << "void_request_num=" << void_request_num << endl;

    	/* Wait for unfinished workers */
    	for(int i=0; i<void_request_num; i++)
    	{
    		int words_num_local;
            MPI_Recv(&words_num_local, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
            if(words_num_total >= 0) words_num_total += words_num_local;
    	}

    	cout << "* Found " << words_num_total << " words!" << endl;
    	/* Send finished signal */
    	const char *stop_signal = "sss";
    	for(int j=1; j<p; j++)
    	{
    		MPI_Send(stop_signal, 3, MPI_CHAR, j, 0, MPI_COMM_WORLD);
    	}
    	cout << "Process " << id << " exited." << endl;

	}
	else  //Counter, worker
	{
		MPI_Status status;
		int line_len;
		char *line;
		int words_num_local = 0;

		/* Send a initial request*/
		int initial_signal = -1;
		MPI_Send(&initial_signal, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);

		for(;;)
		{
			MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
			MPI_Get_count(&status, MPI_CHAR, &line_len);

			line = (char *)malloc(line_len * sizeof(char));
			MPI_Recv(line, line_len, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);

			/* check if stop signal */
			if(line_len == 3 && line[0] == 's' && line[1] == 's' && line[2] == 's')
			{
				cout << "Process " << id << " exited." << endl;
				break;
			}
			else
			{
				words_num_local = 0;
				for(int i=0; i<line_len; i++){
					if(line[i] == '\t' || line[i] == ' ' || line[i] == '\n'){
						words_num_local ++;
					}
				}
				// cout << "found " << words_num_local << " words in this line" << endl; 
				MPI_Send(&words_num_local, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
			}

			free(line);
			
		}		
	}

	MPI_Finalize();
	return 0;
}




