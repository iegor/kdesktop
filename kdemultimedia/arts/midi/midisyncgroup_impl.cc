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

#include "midisyncgroup_impl.h"
#include "audiosync_impl.h"
#include "midiclient_impl.h"
#include "midimanager_impl.h"
#include "timestampmath.h"

using namespace Arts;
using namespace std;

MidiSyncGroup_impl::MidiSyncGroup_impl(MidiManager_impl *manager)
	: manager(manager)
{
}

MidiSyncGroup_impl::~MidiSyncGroup_impl()
{
	/* tell clients we're dead */
	list<MidiClient_impl *>::iterator i;
	for(i = clients.begin(); i != clients.end(); i++)
		(*i)->setSyncGroup(0);

	list<AudioSync_impl *>::iterator ai;
	for(ai = audioSyncs.begin(); ai != audioSyncs.end(); ai++)
		(*ai)->setSyncGroup(0);

	manager->removeSyncGroup(this);
}

void MidiSyncGroup_impl::addClient(MidiClient client)
{
	/* add client to list */
	MidiClient_impl *impl = manager->findClient(client.info().ID);
	impl->setSyncGroup(this);
	clients.push_back(impl);

	impl->synchronizeTo(masterTimer.time());
}

void MidiSyncGroup_impl::removeClient(MidiClient client)
{
	/* remove client from the list */
	MidiClient_impl *impl = manager->findClient(client.info().ID);
	impl->setSyncGroup(0);
	clients.remove(impl);
}

void MidiSyncGroup_impl::addAudioSync(AudioSync audioSync)
{
	AudioSync_impl *impl = AudioSync_impl::find(audioSync);
	impl->setSyncGroup(this);
	audioSyncs.push_back(impl);

	impl->synchronizeTo(masterTimer.time());
}

void MidiSyncGroup_impl::removeAudioSync(AudioSync audioSync)
{
	AudioSync_impl *impl = AudioSync_impl::find(audioSync);
	impl->setSyncGroup(0);
	audioSyncs.remove(impl);
}

void MidiSyncGroup_impl::clientChanged(MidiClient_impl *client)
{
	client->synchronizeTo(masterTimer.time());
}

void MidiSyncGroup_impl::clientDied(MidiClient_impl *client)
{
	clients.remove(client);
}

void MidiSyncGroup_impl::audioSyncDied(AudioSync_impl *audioSync)
{
	audioSyncs.remove(audioSync);
}

TimeStamp MidiSyncGroup_impl::time()
{
	TimeStamp result = masterTimer.time();

	list<MidiClient_impl *>::iterator i;
	for(i = clients.begin(); i != clients.end(); i++)
		result = timeStampMax(result, (*i)->clientTime());

	list<AudioSync_impl *>::iterator ai;
	for(ai = audioSyncs.begin(); ai != audioSyncs.end(); ai++)
		result = timeStampMax(result, (*ai)->clientTime());

	return result;
}

TimeStamp MidiSyncGroup_impl::playTime()
{
	return masterTimer.time();
}

void MidiSyncGroup_impl::adjustSync()
{
	list<AudioSync_impl *>::iterator ai;
	for(ai = audioSyncs.begin(); ai != audioSyncs.end(); ai++)
		(*ai)->synchronizeTo(masterTimer.time());
}
