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
#include "math.h"

#undef DEBUG_JITTER

using namespace std;
using namespace Arts;

MidiTimerCommon::MidiTimerCommon() :refCount(0)
{
	refCount = 0;
}

MidiTimerCommon::~MidiTimerCommon()
{
	arts_assert(refCount == 0);
}

void MidiTimerCommon::processQueue()
{
	TimeStamp now = time();

	list<TSNote>::iterator n = noteQueue.begin();
	while(n != noteQueue.end())
	{
		TSNote& note = *n;
		TimeStamp& noteTime = note.event.time;

		if( now.sec > noteTime.sec 
		|| ((now.sec == noteTime.sec) && (now.usec > noteTime.usec)))
		{
#ifdef DEBUG_JITTER
			float jitter = (now.sec-noteTime.sec) * 1000.0;
			jitter += (float)(now.usec-noteTime.usec) / 1000.0;
			arts_debug("midi jitter: %f",jitter);
#endif

			note.port.processCommand(note.event.command);
			n = noteQueue.erase(n);
		}
		else n++;
	}
}

void MidiTimerCommon::queueEvent(MidiPort port,const MidiEvent& event)
{
	noteQueue.push_back(TSNote(port, event));
}
