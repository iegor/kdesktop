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

#ifndef ARTS_MIDI_MIDITIMERCOMMON_H
#define ARTS_MIDI_MIDITIMERCOMMON_H

#include "artsmidi.h"

namespace Arts {

class MidiTimerCommon {
protected:
	struct TSNote {
		MidiPort port;
		MidiEvent event;

		TSNote() { /* for some stl impls */ }

		TSNote(MidiPort port, const MidiEvent& event) :
			port(port), event(event)
		{
		}
	};
	std::list<TSNote> noteQueue;

	int refCount;
	void processQueue();
	
	MidiTimerCommon();
	virtual ~MidiTimerCommon();

public:
	void unsubscribe() { if(--refCount == 0) delete this; }

	void queueEvent(MidiPort port, const MidiEvent& event);
	virtual TimeStamp time() = 0;
};

}
#endif /* ARTS_MIDI_MIDITIMERCOMMON_H */
