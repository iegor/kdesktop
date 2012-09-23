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

#ifndef ARTS_MIDIMANAGER_IMPL_H
#define ARTS_MIDIMANAGER_IMPL_H

#include "artsmidi.h"

namespace Arts {

class MidiClient_impl;
class MidiSyncGroup_impl;
class MidiManager_impl : virtual public MidiManager_skel,
						 virtual public TimeNotify
{
protected:
	std::list<MidiClient_impl *> _clients;
	std::list<MidiSyncGroup_impl *> syncGroups;

	long nextID;
	AlsaMidiGateway alsaMidiGateway;

	void notifyTime();

public:
	MidiManager_impl();
	~MidiManager_impl();

	// public interface
	std::vector<MidiClientInfo> *clients();

	MidiClient addClient(MidiClientDirection direction, MidiClientType type,
					const std::string& title, const std::string& autoRestoreID);

	void connect(long clientID, long destinationID);
	void disconnect(long clientID, long destinationID);
	MidiSyncGroup addSyncGroup();

	// interface to MidiClient_impl
	void removeClient(MidiClient_impl *client);
	MidiClient_impl *findClient(long clientID);

	// interface to MidiSyncGroup_impl
	void removeSyncGroup(MidiSyncGroup_impl *group);
};                                                                              

}
#endif /* ARTS_MIDIMANAGER_IMPL_H */
