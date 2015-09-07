/*$TET$templet$!h-copyright!*/
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

#ifndef _TEMPLET_MODULE_templet
#define _TEMPLET_MODULE_templet

#include <string.h>
#include <assert.h>

#include "dbg/tet.h"

/*$TET$templet$!h-prologue!*/
#include <iostream>
using namespace std;
/*$TET$*/

class Link:public TEMPLET_DBG::Channel{
public:
	Link(TEMPLET_DBG::Assemble*a);
	~Link();
public:
	struct argSin2{//<Begin> 
/*$TET$Link$argSin2*/
	double x;
/*$TET$*/
	};
	struct argCos2{//<Begin> 
/*$TET$Link$argCos2*/
	double x;
/*$TET$*/
	};
	struct result{//<Calc> 
/*$TET$Link$result*/
	double y;
/*$TET$*/
	};
};

class Parent:public TEMPLET_DBG::Process{
public:
	Parent(TEMPLET_DBG::Assemble*a);
	~Parent();
private:
	//methods
	bool fork(/*out*/Link::argCos2*p1,/*out*/Link::argSin2*p2);
	bool join(/*in*/Link::result*p1,/*in*/Link::result*p2);

/*$TET$Parent$!userdata!*/
/*$TET$*/

public:
	Link* p_p1(){return 0; }
	Link* p_p2(){return 0; }
protected:
	virtual void _run(int _selector,TEMPLET_DBG::Channel*_channel);
};

class Child:public TEMPLET_DBG::Process{
public:
	Child(TEMPLET_DBG::Assemble*a);
	~Child();
private:
	//methods
	bool calcCos2(/*in*/Link::argCos2*p1,/*out*/Link::result*p2);
	bool calcSin2(/*in*/Link::argSin2*p1,/*out*/Link::result*p2);

/*$TET$Child$!userdata!*/
/*$TET$*/

public:
	void p_p(Link*p){}
protected:
	virtual void _run(int _selector,TEMPLET_DBG::Channel*_channel);
};

class templet:public TEMPLET_DBG::Assemble{
public:
	templet(int NT);
	~templet();

/*$TET$templet$!userdata!*/
/*$TET$*/
public:
	Parent*new_Parent(){return 0;}
	Child*new_Child(){return 0;}
};

/*$TET$templet$!h-epilogue!*/
/*$TET$*/
#endif
