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

#ifndef ARTS_MIDICLIENT_IMPL_H
#define ARTS_MIDICLIENT_IMPL_H

#include "artsmidi.h"
#include "midimanager_impl.h"
#include "midimanagerport_impl.h"

namespace Arts {

struct MidiClientConnection
{
	TimeStamp offset;
	MidiPort port;
};

class MidiManager_impl;
class MidiSyncGroup_impl;
class MidiClient_impl : virtual public MidiClient_skel
{
protected:
	SystemMidiTimer systemMidiTimer;
	MidiClientInfo _info;
	MidiManager_impl *manager;
	MidiSyncGroup_impl *syncGroup;
	std::list<MidiPort> ports;
	std::list<MidiClientConnection> _connections;

public:
	MidiClient_impl(const MidiClientInfo& info, MidiManager_impl *manager);
	~MidiClient_impl();

	// MCOP interface
	MidiClientInfo info();
	void title(const std::string &newvalue);
	std::string title();
	void addInputPort(MidiPort port);
	MidiPort addOutputPort();
	void removePort(MidiPort port);

	// interface to MidiManager/Port
	inline long ID() { return _info.ID; }
	std::list<MidiClientConnection> *connections();
	void connect(MidiClient_impl *dest);
	void disconnect(MidiClient_impl *dest);
	void rebuildConnections();
	void adjustSync();

	TimeStamp time();
	TimeStamp playTime();

	// interface to MidiSyncGroup
	void synchronizeTo(const TimeStamp& time);
	void setSyncGroup(MidiSyncGroup_impl *syncGroup);
	TimeStamp clientTime();
};

}
#endif /* ARTS_MIDICLIENT_IMPL_H */
