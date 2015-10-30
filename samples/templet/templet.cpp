#include <iostream>
//#include <vector>
#include <queue>
#include <map>
#include <math.h>

using namespace std;

double x,sin2,cos2,one;

struct engine; 
struct proc; 
struct chan; 
 
struct engine{ 
	std::queue<chan*> ready;
 	std::map<int,chan*> chans;// channel id --> channel ref 
 	std::map<int,proc*> procs;// process id --> process ref 
 	std::map<int,int> pmap;   // process id --> rank in MPI_COMM_WORLD 
}; 
 
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
 
inline void run(engine*e, int n = 1) 
{ 
 	while (!e->ready.empty()){ 
 		chan*c = e->ready.front(); e->ready.pop(); 
 		c->sending = false; 
 		c->p->recv(c, c->p); 
 	} 
} 

engine e; 
proc  parent,child1,child2; 
chan  link0,link1,link2;

void  Parent(chan*c, proc*p)
{
	if(c==&link0){  
		// cin >> x;
		send(&e,&link1,&child1);
		send(&e,&link2,&child2);
	}
	if(access(&link1,p) && access(&link2,p)) one=sin2+cos2;
}

void inParent(proc*, std::istream&s)
{
	s >> x >> one;
}

void outParent(proc*, std::ostream&s)
{
	s << x << one;
}

void Child(chan*c, proc*)
{
	if(c==&link1) sin2=sin(x)*sin(x);  
	else if(c==&link2)cos2=cos(x)*cos(x);
	else cout << "something wrong";
	send(&e,c,&parent);
}

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

void main()
{
	//cin >> x;
	//sin2=sin(x)*sin(x); /*//*/ cos2=cos(x)*cos(x);
	//cout << endl << "sin2(x)+cos2(x)=" << one=sin2+cos2;
	
	//assigning message handling procedures
	parent.recv=&Parent;
	child1.recv=child2.recv=&Child;

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

	//assigning ids
	parent.id=0;
	child1.id=1;
	child2.id=2;
	//---------
	link0.id=0;
	link1.id=1;
	link2.id=2;

	//putting ids to engine database
	e.procs[0]=&parent;
	e.procs[1]=&child1;
	e.procs[2]=&child2;
	//---------
	e.chans[0]=&link0;
	e.chans[1]=&link1;
	e.chans[2]=&link2;

	//setting (proc) --> (MPI rank) mapping
	e.pmap[0]=0;
	e.pmap[1]=1;
	e.pmap[2]=2;

	cin >> x;

	send(&e,&link0,&parent);
	run(&e);

	cout << endl << "sin2(x)+cos2(x)=" << one;
}
