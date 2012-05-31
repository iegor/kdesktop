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
#include "debug.h"
#include "miditimercommon.h"

using namespace std;
using namespace Arts;

namespace Arts {

class SystemMidiTimerCommon : public MidiTimerCommon,
							  public TimeNotify {
protected:
	SystemMidiTimerCommon();
	virtual ~SystemMidiTimerCommon();

public:
	// allocation: share one SystemMidiTimerCommon for everbody who needs one
	static SystemMidiTimerCommon *subscribe();

	void notifyTime();
	TimeStamp time();
};

}

static SystemMidiTimerCommon *SystemMidiTimerCommon_the = 0;

SystemMidiTimerCommon::SystemMidiTimerCommon()
{
	SystemMidiTimerCommon_the = this;
	Dispatcher::the()->ioManager()->addTimer(10, this);
}

SystemMidiTimerCommon::~SystemMidiTimerCommon()
{
	Dispatcher::the()->ioManager()->removeTimer(this);
	SystemMidiTimerCommon_the = 0;
}

TimeStamp SystemMidiTimerCommon::time()
{
	timeval tv;
	gettimeofday(&tv,0);
	return TimeStamp(tv.tv_sec, tv.tv_usec);
}

void SystemMidiTimerCommon::notifyTime()
{
	processQueue();
}

SystemMidiTimerCommon *SystemMidiTimerCommon::subscribe()
{
	if(!SystemMidiTimerCommon_the) new SystemMidiTimerCommon();
	SystemMidiTimerCommon_the->refCount++;
	return SystemMidiTimerCommon_the;
}

class SystemMidiTimer_impl : public SystemMidiTimer_skel {
protected:
	SystemMidiTimerCommon *timer;
public:
	SystemMidiTimer_impl()
	{
		timer = SystemMidiTimerCommon::subscribe();
	}
	~SystemMidiTimer_impl()
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

namespace Arts {
	REGISTER_IMPLEMENTATION(SystemMidiTimer_impl);
}
