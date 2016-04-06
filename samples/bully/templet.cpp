#include <iostream>
#include <queue>
#include <map>
#include <math.h>
#include <mpi.h>
#include <windows.h>

using namespace std;

#define CLOCK_ID	100
#define ANSWER_ID	200
#define VOTING_ID	300
#define ERROR_ID	400
#define COORD_ID	500

typedef enum {
    IDLE = 1000,
    VOTING,
    COORDINATING
} State;

struct engine; 
struct proc; 
struct chan; 
 
struct proc{ 
	void(*recv)(chan*, proc*); 
 	int id; // process ref in MPI message
	State state;
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
	int rank=saved_rank;
	int size=saved_size;
	while (1) {
		if (rank == 0)
		{
			for (int i=1; i<saved_size; i++)
			{
				MPI_Send(buffer, buff_size, MPI_INT, i, CLOCK_ID, MPI_COMM_WORLD);
				cout << "[" << rank << "]: " << "Sending " << CLOCK_ID << " to [" << i << "]" << endl;
			}
		} else {
			MPI_Status status;
			MPI_Probe(0, MPI_ANY_TAG,MPI_COMM_WORLD,&status);
			MPI_Recv(buffer, buff_size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			cout << "[" << rank << "]: " << "Received: " << status.MPI_TAG  << " from [" << status.MPI_SOURCE << "]" << endl;
			switch (status.MPI_TAG) {
				case CLOCK_ID:
					if (rank==3)
					{
						MPI_Send(buffer, buff_size, MPI_INT, status.MPI_SOURCE, ERROR_ID, MPI_COMM_WORLD);
						cout << "[" << rank << "]: " << "Sending " << ERROR_ID << " to [" << status.MPI_SOURCE << "]" << endl;
					} else {
						MPI_Send(buffer, buff_size, MPI_INT, status.MPI_SOURCE, ANSWER_ID, MPI_COMM_WORLD);
						cout << "[" << rank << "]: " << "Sending " << ANSWER_ID << " to [" << status.MPI_SOURCE << "]" << endl;
					}
					break;
			}
		}
		Sleep(1000);
		if (rank == 0)
		{
			for (int i=1; i<saved_size; i++)
			{
				MPI_Status status;
				MPI_Recv(buffer, buff_size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				cout << "[" << rank << "]: " << "Received: " << status.MPI_TAG  << " from [" << status.MPI_SOURCE << "]" << endl;
				switch (status.MPI_TAG)
				{
					case ANSWER_ID:
						cout << "\t\tEverything is OK!" << endl;
						break;
					case ERROR_ID:
						cout << "\t\tError!!! Start voting!" << endl;
						break;
				}
			}
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
