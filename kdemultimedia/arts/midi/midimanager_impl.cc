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

#include "midimanager_impl.h"
#include "midiclient_impl.h"
#include "midisyncgroup_impl.h"
#include "debug.h"

using namespace Arts;
using namespace std;

static int cleanReference(const string& reference)
{
	Object test = Reference("global:"+reference);
	if(test.isNull())
	{
		Dispatcher::the()->globalComm().erase(reference);
		return 1;
	}
	else
		return 0;
}

MidiManager_impl::MidiManager_impl() : nextID(1)
{
	cleanReference("Arts_MidiManager");
	if(!ObjectManager::the()->addGlobalReference(Object::_from_base(_copy()),
					"Arts_MidiManager"))
	{
		arts_warning("can't register Arts::MidiManager");
	}
	else
	{
		arts_debug("Arts::MidiManager registered successfully.");
	}
	Dispatcher::the()->ioManager()->addTimer(1000, this);
}

MidiManager_impl::~MidiManager_impl()
{
	Dispatcher::the()->ioManager()->removeTimer(this);
}

vector<MidiClientInfo> *MidiManager_impl::clients()
{
	if(!alsaMidiGateway.isNull())
	{
		if(!alsaMidiGateway.rescan())
			alsaMidiGateway = AlsaMidiGateway::null();
	}

	vector<MidiClientInfo> *result = new vector<MidiClientInfo>;

	list<MidiClient_impl *>::iterator i;
	for(i = _clients.begin(); i != _clients.end(); i++)
		result->push_back((*i)->info());

	return result;
}

MidiClient MidiManager_impl::addClient(MidiClientDirection direction,
		MidiClientType type, const string& title, const string& autoRestoreID)
{
	MidiClientInfo info;

	info.ID = nextID++;
	info.direction = direction;
	info.type = type;
	info.title = title;
	info.autoRestoreID = autoRestoreID;

	MidiClient_impl *impl = new MidiClient_impl(info, this);
	_clients.push_back(impl);
	return MidiClient::_from_base(impl);
}

void MidiManager_impl::removeClient(MidiClient_impl *client)
{
	_clients.remove(client);
}

MidiClient_impl *MidiManager_impl::findClient(long clientID)
{
	list<MidiClient_impl *>::iterator i;

	for(i = _clients.begin(); i != _clients.end(); i++)
	{
		if((*i)->ID() == clientID)
			return (*i);
	}
	return 0;
}

void MidiManager_impl::connect(long clientID, long destinationID)
{
	MidiClient_impl *src = findClient(clientID);
	MidiClient_impl *dest = findClient(destinationID);

	arts_return_if_fail(src);
	arts_return_if_fail(dest);
	src->connect(dest);
}

void MidiManager_impl::disconnect(long clientID, long destinationID)
{
	MidiClient_impl *src = findClient(clientID);
	MidiClient_impl *dest = findClient(destinationID);

	arts_return_if_fail(src);
	arts_return_if_fail(dest);
	src->disconnect(dest);
}

MidiSyncGroup MidiManager_impl::addSyncGroup()
{
	MidiSyncGroup_impl *impl = new MidiSyncGroup_impl(this);
	syncGroups.push_back(impl);
	return MidiSyncGroup::_from_base(impl);
}

void MidiManager_impl::removeSyncGroup(MidiSyncGroup_impl *group)
{
	syncGroups.remove(group);
}

void MidiManager_impl::notifyTime()
{
	list<MidiClient_impl *>::iterator i;
	for(i = _clients.begin(); i != _clients.end(); i++)
		(*i)->adjustSync();

	list<MidiSyncGroup_impl *>::iterator gi;
	for(gi = syncGroups.begin(); gi != syncGroups.end(); gi++)
		(*gi)->adjustSync();
}

namespace Arts { REGISTER_IMPLEMENTATION(MidiManager_impl); }
