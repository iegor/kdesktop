    /*

    Copyright (C) 2002 Stefan Westerfeld
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

#ifndef ARTS_MIDISYNCGROUP_IMPL_H
#define ARTS_MIDISYNCGROUP_IMPL_H

#include "artsmidi.h"

namespace Arts {

class MidiClient_impl;
class MidiManager_impl;
class AudioSync_impl;

class MidiSyncGroup_impl : virtual public MidiSyncGroup_skel {
protected:
	SystemMidiTimer masterTimer;

	MidiManager_impl *manager;
	std::list<MidiClient_impl *> clients;
	std::list<AudioSync_impl *> audioSyncs;

public:
	MidiSyncGroup_impl(MidiManager_impl *manager);
	~MidiSyncGroup_impl();

	// public interface
	void addClient(MidiClient client);
	void removeClient(MidiClient client);

	void addAudioSync(AudioSync audioSync);
	void removeAudioSync(AudioSync audioSync);

	// interface to MidiClient (AudioSync)
	void clientChanged(MidiClient_impl *client);
	void clientDied(MidiClient_impl *client);
	void audioSyncDied(AudioSync_impl *audioSync);

	TimeStamp time();
	TimeStamp playTime();

	// interface to MidiManager
	void adjustSync();
};

}

#endif /* ARTS_MIDISYNCGROUP_IMPL_H */
