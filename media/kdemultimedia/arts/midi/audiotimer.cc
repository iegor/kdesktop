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

#include "artsmidi.h"
#include "artsflow.h"
#include "stdsynthmodule.h"
#include "debug.h"
#include "miditimercommon.h"
#include "audiotimer.h"
#include "flowsystem.h"

using namespace std;
using namespace Arts;

static AudioTimer *AudioTimer_the = 0;

AudioTimer::AudioTimer()
{
	AudioTimer_the = this;
	samples = seconds = 0;
}

AudioTimer::~AudioTimer()
{
	AudioTimer_the = 0;
}

TimeStamp AudioTimer::time()
{
	return TimeStamp(seconds,
				(long)((float)samples / samplingRateFloat * 1000000.0));
}

void AudioTimer::notify(const Notification &)
{
	list<AudioTimerCallback *>::iterator i;
	for(i = callbacks.begin(); i != callbacks.end(); i++)
		(*i)->updateTime();
}

void AudioTimer::calculateBlock(unsigned long s)
{
	samples += s;
	while(samples > samplingRate)
	{
		samples -= samplingRate;
		seconds++;
	}
	Notification n;
	n.receiver = this;
	n.ID = 0;
	n.data = 0;
	n.internal = 0;
	NotificationManager::the()->send(n);
}

AudioTimer *AudioTimer::subscribe()
{
	if(!AudioTimer_the)
	{
		new AudioTimer();
		AudioTimer_the->_node()->start();
	}
	else
	{
		AudioTimer_the->_copy();
	}
	return AudioTimer_the;
}

void AudioTimer::addCallback(AudioTimerCallback *callback)
{
	callbacks.push_back(callback);
}

void AudioTimer::removeCallback(AudioTimerCallback *callback)
{
	callbacks.remove(callback);
}
