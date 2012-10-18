    /*

    Copyright (C) 2001 Stefan Westerfeld
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

#include "artsmidi.h"
#include "artsflow.h"
#include "stdsynthmodule.h"
#include "debug.h"
#include "miditimercommon.h"
#include "audiotimer.h"
#include "flowsystem.h"

using namespace std;

namespace Arts {

class AudioMidiTimerCommon : public MidiTimerCommon, public AudioTimerCallback
{
protected:
	AudioMidiTimerCommon();
	virtual ~AudioMidiTimerCommon();

	AudioTimer *audioTimer;

public:
	// allocation: share one AudioMidiTimerCommon for everbody who needs one
	static AudioMidiTimerCommon *subscribe();

	TimeStamp time();
	void updateTime();
};

}

using namespace Arts;

static AudioMidiTimerCommon *AudioMidiTimerCommon_the = 0;

AudioMidiTimerCommon::AudioMidiTimerCommon()
{
	AudioMidiTimerCommon_the = this;

	audioTimer = AudioTimer::subscribe();
	audioTimer->addCallback(this);
}

AudioMidiTimerCommon::~AudioMidiTimerCommon()
{
	audioTimer->removeCallback(this);
	audioTimer->unsubscribe();

	AudioMidiTimerCommon_the = 0;
}

TimeStamp AudioMidiTimerCommon::time()
{
	return audioTimer->time();
}

void AudioMidiTimerCommon::updateTime()
{
	processQueue();
}

AudioMidiTimerCommon *AudioMidiTimerCommon::subscribe()
{
	if(!AudioMidiTimerCommon_the)
		AudioMidiTimerCommon_the = new AudioMidiTimerCommon();
	AudioMidiTimerCommon_the->refCount++;
	return AudioMidiTimerCommon_the;
}

namespace Arts {

class AudioMidiTimer_impl : public AudioMidiTimer_skel {
protected:
	AudioMidiTimerCommon *timer;
public:
	AudioMidiTimer_impl()
	{
		timer = AudioMidiTimerCommon::subscribe();
	}
	~AudioMidiTimer_impl()
	{
		timer->unsubscribe();
	}
	TimeStamp time()
	{
		return timer->time();
	}
	void queueEvent(MidiPort port, const MidiEvent& event)
	{
		timer->queueEvent(port, event);
	}
};

REGISTER_IMPLEMENTATION(AudioMidiTimer_impl);
}
