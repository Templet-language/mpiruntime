#include <iostream>
#include <queue>
#include <map>
#include <math.h>
#include <mpi.h>

using namespace std;

struct engine; 
struct proc; 
struct chan; 
 
struct proc{ 
	void(*recv)(chan*, proc*); 
 	int id; // process ref in MPI message 
 	void(*in) (proc*, std::istream&); 
 	void(*out)(proc*, std::ostream&); 
}; 
 
struct chan{ 
	proc*p; 
 	bool sending; 
	int id; // channel ref in MPI message 
	void(*in) (chan*, std::istream&); 
	void(*out)(chan*, std::ostream&); 
}; 

proc  parent,child1,child2; 
chan  link0,link1,link2;

struct engine{ 
	std::queue<chan*> ready;
 	std::map<int,chan*> chans;// channel id --> channel ref 
 	std::map<int,proc*> procs;// process id --> process ref 
 	std::map<int,int> pmap;   // process id --> rank in MPI_COMM_WORLD 

	inline void map(){
		/*
		//assigning serialization procedures
		parent.in=&inParent;
		parent.out=&outParent;
		child1.in=&inChild1;
		child2.in=&inChild2;
		child1.out=&outChild1;
		child2.out=&outChild2;
		//------------------------------
		link0.in=&inLink0;
		link0.out=&outLink0;
		link1.in=&inLink1;
		link1.out=&outLink1;
		link2.in=&inLink2;
		link2.out=&outLink2;
		*/
		//assigning ids
		parent.id=0;
		child1.id=1;
		child2.id=2;
		//---------
		link0.id=0;
		link1.id=1;
		link2.id=2;

		//putting ids to engine database
		procs[0]=&parent;
		procs[1]=&child1;
		procs[2]=&child2;
		//---------
		chans[0]=&link0;
		chans[1]=&link1;
		chans[2]=&link2;

		//setting (proc) --> (MPI rank) mapping
		pmap[0]=0;
		pmap[1]=1;
		pmap[2]=2;
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

void  Parent(chan*c, proc*p)
{
	if(c==&link0){  
		send(&e,&link1,&child1);
		send(&e,&link2,&child2);
	}
	if(access(&link1,p) && access(&link2,p)) {
		int rank;
		MPI_Comm_rank(MPI_COMM_WORLD,&rank);
		if (rank==0)
		{
			MPI_Status status;
			double sin2, cos2;
			MPI_Probe(1,11,MPI_COMM_WORLD,&status);
			MPI_Recv(&sin2,1,MPI_DOUBLE,1,11,MPI_COMM_WORLD,&status);
			MPI_Probe(2,22,MPI_COMM_WORLD,&status);
			MPI_Recv(&cos2,1,MPI_DOUBLE,2,22,MPI_COMM_WORLD,&status);
			double one=sin2+cos2;
			cout << endl << "sin2(x)+cos2(x)=" << one;
		}
	}
}
/*
void inParent(proc*, std::istream&s)
{
	s >> x >> one;
}

void outParent(proc*, std::ostream&s)
{
	s << x << one;
}
*/
void Child(chan*c, proc*)
{
	double x;
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	if ((c==&link1) && (rank==1))
	{
		MPI_Status status;
		MPI_Probe(0,1,MPI_COMM_WORLD,&status);
		MPI_Recv(&x,1,MPI_DOUBLE,0,1,MPI_COMM_WORLD,&status);
		double sin2=sin(x)*sin(x);
		MPI_Send(&sin2,1,MPI_DOUBLE,0,11,MPI_COMM_WORLD);
	} 
	else 
	{
		if((c==&link2) && (rank==2))
		{
			MPI_Status status;
			MPI_Probe(0,2,MPI_COMM_WORLD,&status);
			MPI_Recv(&x,1,MPI_DOUBLE,0,2,MPI_COMM_WORLD,&status);
			double cos2=cos(x)*cos(x);
			MPI_Send(&cos2,1,MPI_DOUBLE,0,22,MPI_COMM_WORLD);
		}
	}
	send(&e,c,&parent);
}
/*
void inChild1(proc*, std::istream&s){s >> sin2;}
void outChild1(proc*, std::ostream&s){s << sin2;}

void inChild2(proc*, std::istream&s){s >> cos2;}
void outChild2(proc*, std::ostream&s){s << cos2;}

void inLink0(chan*, std::istream&){}//passing no data
void outLink0(chan*, std::ostream&){}

void inLink1(chan*c, std::istream&s)
{
	if(c->p==&child1 && c->sending){s >> x;	}
	if(c->p==&parent && c->sending){s >> sin2;}
}
void outLink1(chan*c, std::ostream&s)
{
	if(c->p==&child1 && c->sending){s << x;}
	if(c->p==&parent && c->sending){s << sin2;}
}

void inLink2(chan*c, std::istream&s)
{
	if(c->p==&child2&& c->sending){s >> x;}
	if(c->p==&parent && c->sending){s >> cos2;}
}
void outLink2(chan*c, std::ostream&s)
{
	if(c->p==&child2 && c->sending){s << x;}
	if(c->p==&parent && c->sending){s << sin2;}
}
*/
void main(int argc, char **argv)
{
	MPI_Init(&argc,&argv); // MPI initialization

	//sin2=sin(x)*sin(x); /*//*/ cos2=cos(x)*cos(x);
	//cout << endl << "sin2(x)+cos2(x)=" << one=sin2+cos2;
	
	//assigning message handling procedures
	parent.recv=&Parent;
	child1.recv=child2.recv=&Child;

	e.map();

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	if (rank==0)
	{
		cout << "Enter x: ";
		double x;
		cin >> x;
		MPI_Send(&x,1,MPI_DOUBLE,1,1,MPI_COMM_WORLD);
		MPI_Send(&x,1,MPI_DOUBLE,2,2,MPI_COMM_WORLD);
	}

	send(&e,&link0,&parent);
	e.run();
	MPI_Finalize();
}
