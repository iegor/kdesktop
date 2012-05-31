    /*

    Copyright (C) 2001-2002 Stefan Westerfeld
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

#ifndef AUDIOSYNC_IMPL_H
#define AUDIOSYNC_IMPL_H

#include "artsmidi.h"
#include "audiotimer.h"

namespace Arts {
	
class MidiSyncGroup_impl;
class AudioSync_impl : virtual public AudioSync_skel,
                       virtual public AudioTimerCallback
{
	AudioTimer *timer;

	struct AudioSyncEvent
	{
		TimeStamp time;
		std::list<SynthModule> startModules;
		std::list<SynthModule> stopModules;

		void execute();
	};
	std::list<AudioSyncEvent *> events;
	AudioSyncEvent *newEvent;
	MidiSyncGroup_impl *syncGroup;
	TimeStamp syncOffset;

	TimeStamp audioTime();
	TimeStamp audioPlayTime();

public:
	AudioSync_impl();
	~AudioSync_impl();

	// public interface
    TimeStamp time();
	TimeStamp playTime();

    void queueStart(SynthModule synthModule);
    void queueStop(SynthModule synthModule);
    void execute();
    void executeAt(const TimeStamp& timeStamp);

	// interface to AudioTimer
	void updateTime();

	// interface to MidiSyncGroup
	static AudioSync_impl *find(AudioSync audioSync);

	void synchronizeTo(const TimeStamp& time);
	void setSyncGroup(MidiSyncGroup_impl *syncGroup);
	TimeStamp clientTime();
};

}

#endif /* AUDIOSYNC_IMPL_H */
