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

#include "audiosync_impl.h"
#include "midisyncgroup_impl.h"
#include "audiotimer.h"
#include "audiosubsys.h"
#include "timestampmath.h"

#undef AUDIO_DEBUG_DRIFT

using namespace std;
using namespace Arts;

namespace Arts {
	static list<AudioSync_impl *> audioSyncImplList;
}

void AudioSync_impl::AudioSyncEvent::execute()
{
	list<SynthModule>::iterator i;

	for(i = startModules.begin(); i != startModules.end(); i++)
		i->start();

	for(i = stopModules.begin(); i != stopModules.end(); i++)
		i->stop();
}

AudioSync_impl::AudioSync_impl()
	: newEvent(new AudioSyncEvent), syncGroup(0)
{
	syncOffset = TimeStamp(0,0);

	timer = AudioTimer::subscribe();
	timer->addCallback(this);

	audioSyncImplList.push_back(this);
}

AudioSync_impl::~AudioSync_impl()
{
	delete newEvent;

	while(!events.empty())
	{
		delete events.front();
		events.pop_front();
	}

	if(syncGroup)
	{
		syncGroup->audioSyncDied(this);
		syncGroup = 0;
	}
	audioSyncImplList.remove(this);
	timer->removeCallback(this);
	timer->unsubscribe();
}

TimeStamp AudioSync_impl::time()
{
	if(syncGroup)
		return syncGroup->time();
	else
		return audioTime();
}

TimeStamp AudioSync_impl::playTime()
{
	if(syncGroup)
		return syncGroup->playTime();
	else
		return audioPlayTime();
}

TimeStamp AudioSync_impl::audioTime()
{
	return timer->time();
}

TimeStamp AudioSync_impl::audioPlayTime()
{
	double delay = AudioSubSystem::the()->outputDelay();

	TimeStamp time = audioTime();
	timeStampDec(time,timeStampFromDouble(delay));
	return time;
}

TimeStamp AudioSync_impl::clientTime()
{
	TimeStamp time = audioTime();
	timeStampDec(time, syncOffset);
	return time;
}

void AudioSync_impl::queueStart(SynthModule synthModule)
{
	newEvent->startModules.push_back(synthModule);
}

void AudioSync_impl::queueStop(SynthModule synthModule)
{
	newEvent->stopModules.push_back(synthModule);
}

void AudioSync_impl::execute()
{
	newEvent->execute();
	newEvent->startModules.clear();
	newEvent->stopModules.clear();
}

void AudioSync_impl::executeAt(const TimeStamp& timeStamp)
{
	newEvent->time = timeStamp;
	if(syncGroup)
		timeStampInc(newEvent->time, syncOffset);

	events.push_back(newEvent);

	newEvent = new AudioSyncEvent;
}

void AudioSync_impl::updateTime()
{
	TimeStamp now = audioTime();
	list<AudioSyncEvent*>::iterator i;

	i = events.begin();
	while(i != events.end())
	{	
		AudioSyncEvent *event = *i;
		TimeStamp& eventTime = event->time;

		if( now.sec > eventTime.sec
				|| ((now.sec == eventTime.sec) && (now.usec > eventTime.usec)))
		{
			event->execute();
			delete event;
			i = events.erase(i);
		}
		else
		{
			i++;
		}
	}
}

void AudioSync_impl::setSyncGroup(MidiSyncGroup_impl *newSyncGroup)
{
	syncGroup = newSyncGroup;
}

void AudioSync_impl::synchronizeTo(const TimeStamp& time)
{
#ifdef AUDIO_DEBUG_DRIFT
	TimeStamp drift = syncOffset;			// debug drift
#endif

	syncOffset = audioPlayTime();
	timeStampDec(syncOffset, time);

#ifdef AUDIO_DEBUG_DRIFT
	timeStampDec(drift, syncOffset);		// debug drift
	printf("SYNC DRIFT %30s %30s: %f\n",
				"AudioSync", "AudioSync", timeStampToDouble(drift));
#endif
}

AudioSync_impl *AudioSync_impl::find(AudioSync audioSync)
{
	list<AudioSync_impl *>::iterator i;

	for(i = audioSyncImplList.begin(); i != audioSyncImplList.end(); i++)
	{
		if((*i)->_isEqual(audioSync._base()))
			return (*i);
	}
	return 0;
}

namespace Arts { REGISTER_IMPLEMENTATION(AudioSync_impl); }
