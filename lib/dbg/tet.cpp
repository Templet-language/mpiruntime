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

#include "tet.h"
#include <assert.h>
#include <cstddef>
#include <cstdlib>

namespace TEMPLET_DBG {

void Channel::_send()
{ 
	TEMPLET_DBG::send(_assemble, this, _active == RTL_SRV ? _srvPort : _cliPort);
}

void Activator::_send(int port)
{ 
	_cli_selector = port; _active = RTL_CLI;
	TEMPLET_DBG::send(_assemble, this, _cliPort);
}

void Channel::resend()
{
	assert(_active==CLI||_active==SRV);
	_active=(_active==CLI)?RTL_CLI:RTL_SRV;
	TEMPLET_DBG::send(_assemble, this, _active == RTL_SRV ? _srvPort : _cliPort);
}

Assemble::~Assemble()
{
	std::vector<Channel*>::iterator itc;
	for(itc=channels.begin();itc!=channels.end();itc++)delete(*itc);
	std::vector<Process*>::iterator itp;
	for(itp=processes.begin();itp!=processes.end();itp++)delete(*itp);
}

bool Assemble::run(){
	TEMPLET_DBG::run(this);
	return true;
}

void recv(chan*chn, proc*prc)
{
	Process*p = (Process*)prc;
	Channel*c = (Channel*)chn;

	int _selector;
	assert(c->_active == Channel::RTL_CLI || c->_active == Channel::RTL_SRV);

	if (c->_active == Channel::RTL_CLI){ c->_active = Channel::CLI; _selector = c->_cli_selector; }
	if (c->_active == Channel::RTL_SRV){ c->_active = Channel::SRV; _selector = c->_srv_selector; }

	p->_run(_selector, c);
}

}