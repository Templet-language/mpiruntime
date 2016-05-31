#include <iostream>
#include <queue>
#include <map>
#include <math.h>
#include <mpi.h>
#include <windows.h>
#include <cstdlib>

using namespace std;

#define CLOCK_ID		100
#define ANSWER_ID		200
#define VOTING_ID		300
#define VOTING_OK_ID	350
#define ERROR_ID		400
#define COORD_ID		500

struct engine; 
struct proc; 
struct chan; 
 
struct proc{ 
	void(*recv)(chan*, proc*); 
 	int id; // process ref in MPI message
}; 
 
struct chan{ 
	proc*p; 
 	bool sending; 
	int id; // channel ref in MPI message
}; 

proc procs[8]; 
chan links[8];
int buffer[1];
int buff_size = 1;
int saved_rank, saved_size;

struct engine{ 
	std::queue<chan*> ready;
	inline void map(){
		
		//assigning ids
		for (int i=0;i<saved_size;i++)
		{
			procs[i].id=i;
			links[i].id=i;
		}
	}

	inline void run() 
	{ 
 		while (!ready.empty()){ 
 			chan*c = ready.front(); ready.pop(); 
 			c->sending = false; 
 			c->p->recv(c, c->p); 
 		} 
	} 
}; 
 
engine e; 

inline void send(engine*e, chan*c, proc*p) 
{ 
	if (c->sending) return; 
 	c->sending = true;	c->p = p; 
 	e->ready.push(c); 
} 
 
inline bool access(chan*c, proc*p) 
{ 
	return c->p == p && !c->sending; 
} 

void Process(chan*c, proc*p)
{
	bool coordinating = false;
	bool voting = true;

	int rank=saved_rank;
	int size=saved_size;

	if (rank==size-1){
		coordinating = true;
	}

	if (rank==0)
	{
		for (int i=1; i<size; i++)
		{
			MPI_Send(buffer, buff_size, MPI_INT, i, CLOCK_ID, MPI_COMM_WORLD);
			cout << "[" << rank << "]: " << "Sending [CLOCK] to [" << i << "]" << endl;
			Sleep(400);
		}
	} else {
		Sleep(400);
		MPI_Status status;
		MPI_Probe(0, MPI_ANY_TAG,MPI_COMM_WORLD,&status);
		MPI_Recv(buffer, buff_size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		cout << "[" << rank << "]: " << "Received: " << ((status.MPI_TAG==CLOCK_ID) ? "[CLOCK]" : "[???]")  << " from [" << status.MPI_SOURCE << "]" << endl;
		if (coordinating) {
			coordinating = false;
			MPI_Send(buffer, buff_size, MPI_INT, status.MPI_SOURCE, ERROR_ID, MPI_COMM_WORLD);
			cout << "[" << rank << "]: " << "Sending [ERROR] to [" << status.MPI_SOURCE << "]" << endl;
		} else {
			MPI_Send(buffer, buff_size, MPI_INT, status.MPI_SOURCE, ANSWER_ID, MPI_COMM_WORLD);
			cout << "[" << rank << "]: " << "Sending [ANSWER] to [" << status.MPI_SOURCE << "]" << endl;
		}
		Sleep(400);
	}
	if (rank==0)
	{
		Sleep(400);
		for (int i=1; i<size; i++)
		{
			MPI_Status status;
			MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG,MPI_COMM_WORLD,&status);
			MPI_Recv(buffer, buff_size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			cout << "[" << rank << "]: " << "Received: " << ((status.MPI_TAG==ANSWER_ID)?"[ANSWER]":"[ERROR]")  << " from [" << status.MPI_SOURCE << "]" << endl;
			if (status.MPI_TAG==ANSWER_ID)
			{
				cout << ">" << status.MPI_SOURCE << "<: OK" << endl;
			} else {
				cout << ">" << status.MPI_SOURCE << "<: Failed! Start voting!" << endl;
				voting = true;
			}
		}
	}

	for (int chosenRank=0; chosenRank<size-1; chosenRank++)
	{
		if (rank==chosenRank) {
			if (voting)
			{
				for (int i=chosenRank+1; i<size; i++)
				{
					MPI_Send(buffer, buff_size, MPI_INT, i, VOTING_ID, MPI_COMM_WORLD);
					cout << "[" << rank << "]: " << "Sending [VOTING] to [" << i << "]" << endl;
				}
			}
			Sleep(400);
		} else {
			if (rank > chosenRank)
			{
				Sleep(400);
				MPI_Status status;
				MPI_Probe(chosenRank, MPI_ANY_TAG,MPI_COMM_WORLD,&status);
				MPI_Recv(buffer, buff_size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				cout << "[" << rank << "]: " << "Received: " << ((status.MPI_TAG==VOTING_ID)?"[VOTING]":"[???]")  << " from [" << status.MPI_SOURCE << "]" << endl;
				Sleep(400);
				if (rank==size-1)
				{
					MPI_Send(buffer, buff_size, MPI_INT, status.MPI_SOURCE, ERROR_ID, MPI_COMM_WORLD);
					cout << "[" << rank << "]: " << "Sending [ERROR] to [" << status.MPI_SOURCE << "]" << endl;
				} else {
					MPI_Send(buffer, buff_size, MPI_INT, status.MPI_SOURCE, VOTING_OK_ID, MPI_COMM_WORLD);
					cout << "[" << rank << "]: " << "Sending [VOTING_OK] to [" << status.MPI_SOURCE << "]" << endl;
				}
				Sleep(400);
			}
		}
	}
	Sleep(1000);
	for (int chosenRank=0; chosenRank<size-1; chosenRank++)
	{
		if (rank==chosenRank) {
			if (voting)
			{
				int votes=0;
				for (int i=chosenRank+1; i<size; i++)
				{
					MPI_Status status;
					MPI_Probe(i, MPI_ANY_TAG,MPI_COMM_WORLD,&status);
					MPI_Recv(buffer, buff_size, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
					cout << "[" << rank << "]: " << "Received: " << ((status.MPI_TAG==VOTING_OK_ID)?"[VOTING_OK]":"[ERROR]")  << " from [" << status.MPI_SOURCE << "]" << endl;
					if (status.MPI_TAG==VOTING_OK_ID)
					{
						votes++;
					}
				}
				if (votes>0)
				{
					cout << "[" << rank << "]: I'm not coordinator!" << endl;
				} else {
					coordinating = true;
					cout << "[" << rank << "]: I'm new coordinator!" << endl << "[End of voting]" << endl;
				}
			}
			Sleep(400);
		}
	}	
}

void main(int argc, char **argv)
{
	MPI_Init(&argc,&argv); // MPI initialization
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	int size;
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	
	for (int i=0;i<size;i++)
	{
		procs[i].recv=&Process;
	}

	e.map();
	

	saved_rank=rank;
	saved_size=size;

	send(&e,&links[rank],&procs[rank]);

	e.run();
	MPI_Finalize();
}
