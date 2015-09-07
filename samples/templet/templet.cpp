/*$TET$templet$!cpp-copyright!*/
/*--------------------------------------------------------------------------*/
/*  Copyright 2010-2015 Sergey Vostokin                                     */
/*                                                                          */
/*  Licensed under the Apache License, Version 2.0 (the "License");         */
/*  you may not use this file except in compliance with the License.        */
/*  You may obtain a copy of the License at                                 */
/*                                                                          */
/*  http://www.apache.org/licenses/LICENSE-2.0                              */
/*                                                                          */
/*  Unless required by applicable law or agreed to in writing, software     */
/*  distributed under the License is distributed on an "AS IS" BASIS,       */
/*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*/
/*  See the License for the specific language governing permissions and     */
/*  limitations under the License.                                          */
/*--------------------------------------------------------------------------*/
/*$TET$*/

/*$TET$templet$!templet!*/
/*~Link=
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
	calcSin2(p?argSin2,p!result).*/
/*$TET$*/

#include "templet.h"

/*$TET$templet$!cpp-prologue!*/
/*$TET$*/
//////////////////////class Link////////////////////
Link::Link(TEMPLET_DBG::Assemble*a):TEMPLET_DBG::Channel(a)
{
/*$TET$Link$!constructor!*/
/*$TET$*/
}

Link::~Link()
{
/*$TET$Link$!destructor!*/
/*$TET$*/
}

//////////////////////class Parent////////////////////
Parent::Parent(TEMPLET_DBG::Assemble*a):TEMPLET_DBG::Process(a)
{
/*$TET$Parent$!constructor!*/
/*$TET$*/
}

Parent::~Parent()
{
/*$TET$Parent$!destructor!*/
/*$TET$*/
}

/*$TET$Parent$!usercode!*/
/*$TET$*/

bool Parent::fork(/*out*/Link::argCos2*p1,/*out*/Link::argSin2*p2)
{
/*$TET$Parent$fork*/
	p1->x=p2->x=0.333;
	return true;
/*$TET$*/
}

bool Parent::join(/*in*/Link::result*p1,/*in*/Link::result*p2)
{
/*$TET$Parent$join*/
	cout<<"sin2x+cos2x="<<p1->y+p2->y;
	return true;
/*$TET$*/
}

void Parent::_run(int _selector,TEMPLET_DBG::Channel*_channel)
{
	bool res;
/*$TET$Parent$!run!*/
/*$TET$*/

}

//////////////////////class Child////////////////////
Child::Child(TEMPLET_DBG::Assemble*a):TEMPLET_DBG::Process(a)
{
/*$TET$Child$!constructor!*/
/*$TET$*/
}

Child::~Child()
{
/*$TET$Child$!destructor!*/
/*$TET$*/
}

/*$TET$Child$!usercode!*/
/*$TET$*/

bool Child::calcCos2(/*in*/Link::argCos2*p1,/*out*/Link::result*p2)
{
/*$TET$Child$calcCos2*/
	p2->y=cos(p1->x)*cos(p1->x);
	return true;
/*$TET$*/
}

bool Child::calcSin2(/*in*/Link::argSin2*p1,/*out*/Link::result*p2)
{
/*$TET$Child$calcSin2*/
	p2->y=sin(p1->x)*sin(p1->x);
	return true;
/*$TET$*/
}

void Child::_run(int _selector,TEMPLET_DBG::Channel*_channel)
{
	bool res;
/*$TET$Child$!run!*/
/*$TET$*/

}

//////////////////////class templet////////////////////
templet::templet(int NT): TEMPLET_DBG::Assemble(NT)
{
/*$TET$templet$!constructor!*/
/*$TET$*/
}

templet::~templet()
{
/*$TET$templet$!destructor!*/
/*$TET$*/
}

/*$TET$templet$!cpp-epilogue!*/
int main(int argc, char*argv[])
{
	templet tet(2);
	
	Parent* p=tet.new_Parent();
	Child*  c1=tet.new_Child();
	Child*  c2=tet.new_Child();

	c1->p_p(p->p_p1());
	c2->p_p(p->p_p2());

	tet.run();
	
	return 0;
}
/*$TET$*/

