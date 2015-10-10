#include <iostream>
#include <vector>
#include <math.h>
using namespace std;

double x0,x1,x2,s2,c2;

/////////////////////////////
struct engine; 
struct proc; 
struct chan; 
 
struct engine{ 
	std::vector<chan*> ready; 
}; 
 
struct proc{ 
	void(*recv)(chan*, proc*); 
}; 
 
struct chan{ 
	proc*p; 
 	bool sending; 
}; 
 
inline void send(engine*e, chan*c, proc*p) 
{ 
	if (c->sending) return; 
 	c->sending = true; 
 	c->p = p; 
 	e->ready.push_back(c); 
} 
 
inline bool access(chan*c, proc*p) 
{ 
	return c->p == p && !c->sending; 
} 
 
inline void run(engine*e, int n = 1) 
{ 
	size_t rsize; 
	while (rsize = e->ready.size()){ 
 		int n = rand() % rsize;	
		std::vector<chan*>::const_iterator it = e->ready.begin() + n; 
 		chan*c = *it;	e->ready.erase(it); c->sending = false; 
 		c->p->recv(c, c->p); 
	} 
} 

/////////////////////////////
/*
~Link= 
 	+Begin ? argSin2 -> Calc | argCos2 -> Calc; 
 	 Calc  ! result  -> End; 
 	 End. 
*Parent= 
 	 p1 : Link ! result -> join; 
 	 p2 : Link ! result -> join; 
 	+fork(p1!argCos2,p2!argSin2); 
 	 join(p1?result,p2?result). 
*Child= 
 	p : Link ? argCos2 -> calcCos2 | argSin2 -> calcSin2; 
 	calcCos2(p?argCos2,p!result); 
 	calcSin2(p?argSin2,p!result).
*/ 
/////////////////////////////

engine e; 
proc  parent,child1,child2; 
chan  link0,link1,link2;

void  Parent(chan*c, proc*p)
{
	if(c==&link0){  
		cin >> x0; x1=x2=x0;
		send(&e,&link1,&child1);
		send(&e,&link2,&child2);
	}
	if(access(&link1,p) && access(&link2,p))
		cout << endl << "sin2(x)+cos2(x)=" << s2+c2;
}

void Child(chan*c, proc*)
{
	if(c==&link1) s2=sin(x1)*sin(x1);  
	else if(c==&link2)c2=cos(x2)*cos(x2);
	else cout << "something wrong";
	send(&e,c,&parent);
}

void main()
{
	//cin >> x;
	//s2=sin(x)*sin(x); /*//*/ c2=cos(x)*cos(x);
	//cout << endl << "sin2(x)+cos2(x)=" << s2+c2;
	
	parent.recv=&Parent;
	child1.recv=child2.recv=&Child;
	
	send(&e,&link0,&parent);
	run(&e);
}
