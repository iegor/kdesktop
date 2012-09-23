    /*

    Copyright (C) 2000-2002 Stefan Westerfeld
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

#include "midiclient_impl.h"
#include "midimanager_impl.h"
#include "midimanagerport_impl.h"
#include "midisyncgroup_impl.h"
#include "timestampmath.h"

#undef DEBUG_SYNC_DRIFT

using namespace Arts;
using namespace std;

MidiClient_impl::MidiClient_impl(const MidiClientInfo& info,
	MidiManager_impl *manager) :_info(info), manager(manager), syncGroup(0)
{
}

MidiClient_impl::~MidiClient_impl()
{
	while(!_info.connections.empty())
		disconnect(manager->findClient(_info.connections[0]));

	if(syncGroup)
	{
		syncGroup->clientDied(this);
		syncGroup = 0;
	}
	manager->removeClient(this);
}

MidiClientInfo MidiClient_impl::info()
{
	return _info;
}

void MidiClient_impl::title(const string &newvalue)
{
	_info.title = newvalue;
}

string MidiClient_impl::title()
{
	return _info.title;
}

void MidiClient_impl::addInputPort(MidiPort port)
{
	assert(_info.direction == mcdRecord);

	ports.push_back(port);

	// FIXME: should we synchronize inputPorts at all
	rebuildConnections();
}

MidiPort MidiClient_impl::addOutputPort()
{
	assert(_info.direction == mcdPlay);

	MidiPort port = MidiPort::_from_base(new MidiManagerPort_impl(this));
	ports.push_back(port);

	rebuildConnections();
	return port;
}

void MidiClient_impl::removePort(MidiPort port)
{
	list<MidiPort>::iterator i = ports.begin();
	while(i != ports.end())
	{
		if (i->_isEqual(port))
			i = ports.erase(i);
		else
			i++;
	}

	rebuildConnections();
}

void MidiClient_impl::rebuildConnections()
{
	_connections.clear();

	vector<long>::iterator li;
	for(li = _info.connections.begin(); li != _info.connections.end(); li++)
	{
		MidiClient_impl *other = manager->findClient(*li);
		assert(other);

		list<MidiPort>::iterator pi;
		for(pi = other->ports.begin(); pi != other->ports.end(); pi++)
		{
			MidiClientConnection mcc;
			mcc.offset = TimeStamp(0,0);
			mcc.port = *pi;
			_connections.push_back(mcc);
		}
	}
	adjustSync();
}

list<MidiClientConnection> *MidiClient_impl::connections()
{
	return &_connections;
}

static void removeElement(vector<long>& vec, long el)
{
	vector<long> tmp;
	vec.swap(tmp);
	vector<long>::iterator i;
	for(i = tmp.begin(); i != tmp.end(); i++)
		if(*i != el) vec.push_back(*i);
}

void MidiClient_impl::connect(MidiClient_impl *dest)
{
	assert(_info.direction != dest->_info.direction);

	disconnect(dest);

	_info.connections.push_back(dest->ID());
	dest->_info.connections.push_back(ID());

	list<MidiPort>::iterator pi;

	/* add the other clients ports to our connection list */
	for(pi = dest->ports.begin(); pi != dest->ports.end(); pi++)
	{
		MidiClientConnection mcc;
		mcc.offset = TimeStamp(0,0);
		mcc.port = *pi;
		_connections.push_back(mcc);
	}

	/* add our ports to the other clients connection list */
	for(pi = ports.begin(); pi != ports.end(); pi++)
	{
		MidiClientConnection mcc;
		mcc.offset = TimeStamp(0,0);
		mcc.port = *pi;
		dest->_connections.push_back(mcc);
	}
	adjustSync();
}

void MidiClient_impl::disconnect(MidiClient_impl *dest)
{
	assert(_info.direction != dest->_info.direction);

	removeElement(_info.connections,dest->ID());
	removeElement(dest->_info.connections,ID());

	list<MidiPort>::iterator pi;

	/* remove the other clients ports from our connection list */
	for(pi = dest->ports.begin(); pi != dest->ports.end(); pi++)
	{
		list<MidiClientConnection>::iterator ci = _connections.begin();

		while(ci != _connections.end())
		{
			if(ci->port._isEqual(*pi))
				ci = _connections.erase(ci);
			else
				ci++;
		}
	}

	/* remove our ports from the other clients connection list */
	for(pi = ports.begin(); pi != ports.end(); pi++)
	{
		list<MidiClientConnection>::iterator ci = dest->_connections.begin();

		while(ci != dest->_connections.end())
		{
			if(ci->port._isEqual(*pi))
				ci = dest->_connections.erase(ci);
			else
				ci++;
		}
	}
	adjustSync();
}

void MidiClient_impl::synchronizeTo(const TimeStamp& time)
{
	list<MidiClientConnection>::iterator i;

	for(i = _connections.begin(); i != _connections.end(); i++)
	{
		MidiClientConnection& mcc = *i;

#ifdef DEBUG_SYNC_DRIFT
		TimeStamp drift = mcc.offset;		// debug drift
#endif

		mcc.offset = mcc.port.playTime();
		timeStampDec(mcc.offset, time);

#ifdef DEBUG_SYNC_DRIFT
		timeStampDec(drift,mcc.offset);		// debug drift
		printf("SYNC DRIFT %30s %30s: %f\n",
				mcc.port._interfaceName().c_str(), _info.title.c_str(),
				timeStampToDouble(drift));
#endif
	}
}

void MidiClient_impl::setSyncGroup(MidiSyncGroup_impl *newSyncGroup)
{
	syncGroup = newSyncGroup;
}

void MidiClient_impl::adjustSync()
{
	if(syncGroup)
		syncGroup->clientChanged(this);
	else
		synchronizeTo(systemMidiTimer.time());
}

TimeStamp MidiClient_impl::time()
{
	if(syncGroup)
		return syncGroup->time();
	else
		return clientTime();
}

TimeStamp MidiClient_impl::playTime()
{
	if(syncGroup)
		return syncGroup->playTime();
	else
		return systemMidiTimer.time();
}

TimeStamp MidiClient_impl::clientTime()
{
	TimeStamp result = playTime();

	list<MidiClientConnection>::iterator i;
	for(i = _connections.begin(); i != _connections.end(); i++)
	{
		TimeStamp time = i->port.time();
		timeStampDec(time, i->offset);
		result = timeStampMax(result, time);
	}

	return result;
}
