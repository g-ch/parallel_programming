/*
3.1 Count words in many small files
Compile : mpic++ -o small_files small_files.cpp
run e.g.: mpirun -np 8 ./small_files
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

#define BLOCK_LOW(id,p,n) ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1)
#define BLOCK_SIZE(id,p,n) (BLOCK_LOW((id)+1,p,n) - BLOCK_LOW(id,p,n))

using namespace std;
 
void count(fstream &in, string &file_name, long int &cnt) 
{ 
	const char *file = file_name.c_str();
	in.open(file);

	char ch;
	while(in.get(ch))  //word count
	{ 
	  	if(ch == '\t' || ch == ' ' || ch == '\n')
		  	cnt ++;            
  	}
  	in.close();
}


int main(int argc, char** argv) {
	int id, p, n;
	double time;
	const char *folder = "/home/clarence/Desktop/parallel_programming/Small_file/tmp";
	vector<string> files; //To store file names 

	MPI_Init(NULL, NULL);
	MPI_Barrier(MPI_COMM_WORLD);
	time = -MPI_Wtime();

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	MPI_Status status;

	if(p < 2)
	{
		cout << "Should use more than 2 processes!" << endl;
		MPI_Finalize();
		return 0;
	}

	if(id==0)
	{
		DIR* dir = opendir(folder);
	    dirent* pp = NULL;
	    while((pp = readdir(dir)) != NULL)
	    {
	        if(pp->d_name[0] != '.')
	        {
	            string name = string(folder) + "/" + string(pp->d_name);
	            files.push_back(name);
	            n++;
	            // cout<<name<<endl;
	        }
	    }
	    closedir(dir);

	    // Split file names to all processes
	    for(int i=1; i<p; i++)
	    {
	    	int start_num = BLOCK_LOW (i,p,n);
	    	int end_num = BLOCK_HIGH (i,p,n);
	    	int num_size = BLOCK_SIZE (id,p,n);

	    	MPI_Send(&num_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD); //send num

	    	for(int j=0; j<num_size; j++)
	    	{
	    		int buf_size = files[start_num+j].size() - 4;
	    		const char *file_send = files[start_num+j].c_str();

	    		MPI_Send(&buf_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD); //send num
		    	MPI_Send(file_send, buf_size, MPI_CHAR, i, 0, MPI_COMM_WORLD);
	    	}

	    }

	    // Count task for process 0
	    int end_num_0 = BLOCK_HIGH(0,p,n);
	    fstream in;
	    for(int i=0; i<end_num_0; i++)
	    {
	    	long int cnt = 0;
			count(in, files[i], cnt);
			cout <<"File " << files[i] <<" has " << cnt << " words." <<endl;
	    }
	}
	else
	{
		// Receive allocated file names
		int num_size;
		MPI_Recv(&num_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		cout << num_size << " files for process " << id << endl;
		vector<string> file_names_local;

		for(int i=0; i<num_size; i++)
		{
			int buf_size = 0;
			MPI_Recv(&buf_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

			// cout << "size recv = " << buf_size << "\t";

			char *file_name_temp = (char *)malloc(buf_size*sizeof(char)); 
			MPI_Recv(file_name_temp, buf_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);

			file_names_local.push_back(string(file_name_temp));
		}

		// Word count
		fstream in;
		for(int j=0; j<num_size; j++)
		{
			long int cnt = 0;
			string name = file_names_local[j] + ".txt";
			count(in, name, cnt);
			cout <<"File " << name <<" has " << cnt << " words." <<endl;
		}
	}

	MPI_Finalize();
	return 0;
}




