    /*

    Copyright (C) 2000 Stefan Westerfeld
                       stefan@space.twc.de

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
  
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
   
    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

    */

#include "midimanagerport_impl.h"
#include "midimanager_impl.h"
#include "midiclient_impl.h"
#include "timestampmath.h"

#include <stdio.h>

using namespace Arts;
using namespace std;

MidiManagerPort_impl::MidiManagerPort_impl(MidiClient_impl *client)
	: client(client)
{
}

TimeStamp MidiManagerPort_impl::time()
{
	return client->time();
}

TimeStamp MidiManagerPort_impl::playTime()
{
	return client->playTime();
}


void MidiManagerPort_impl::processCommand(const MidiCommand& command)
{
	list<MidiClientConnection> *connections = client->connections();
	list<MidiClientConnection>::iterator i;

	for(i = connections->begin(); i != connections->end(); i++)
		i->port.processCommand(command);
}

void MidiManagerPort_impl::processEvent(const MidiEvent& event)
{
	list<MidiClientConnection> *connections = client->connections();
	list<MidiClientConnection>::iterator i;

	for(i = connections->begin(); i != connections->end(); i++)
	{
		/* relocate the event to the synchronized time */
		TimeStamp time = event.time;
		timeStampInc(time, i->offset);

		i->port.processEvent(MidiEvent(time, event.command));
	}
}
