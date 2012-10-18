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

#include "midimanager_impl.h"
#include <stdio.h>

namespace Arts {

class MidiTest_impl : virtual public MidiTest_skel {
public:
	Arts::TimeStamp time()
	{
		return TimeStamp(0,0);
	}
	Arts::TimeStamp playTime()
	{
		return time();
	}
	void processCommand(const MidiCommand& command)
	{
		if((command.status & mcsCommandMask) == mcsNoteOn)
		{
			mcopbyte ch = command.status & mcsChannelMask;

			printf("noteon ch = %d, note = %d, vel = %d\n",
							ch,command.data1,command.data2);
		}
	}
	void processEvent(const MidiEvent& event)
	{
		printf("At %ld.%06ld: ",event.time.sec,event.time.usec);
		processCommand(event.command);
	}
};

REGISTER_IMPLEMENTATION(MidiTest_impl);
}

