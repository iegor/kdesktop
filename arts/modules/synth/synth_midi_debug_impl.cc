/*

    Copyright (C) 2000 Jeff Tranter
                  tranter@pobox.com

              (C) 1998 Stefan Westerfeld
                  stefan@space.twc.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    */

#include <stdio.h>
#include "debug.h"
#include "artsmodulessynth.h"
#include "stdsynthmodule.h"

using namespace Arts;

class Synth_MIDI_DEBUG_impl : virtual public Synth_MIDI_DEBUG_skel,
							virtual public StdSynthModule
{
	SystemMidiTimer timer;
	MidiClient client;
public:
	Synth_MIDI_DEBUG self() { return Synth_MIDI_DEBUG::_from_base(_copy()); }

	void streamInit()
	{
		printf("MIDI_DEBUG: streamInit\n");
		MidiManager manager = Reference("global:Arts_MidiManager");
		if(!manager.isNull())
		{
			client = manager.addClient(mcdRecord,mctDestination,"midi debug",
											"Arts::Synth_MIDI_DEBUG");
			client.addInputPort(self());
		}
		else
			arts_warning("Synth_MIDI_DEBUG: no midi manager found "
						 "- not registered");
	}
 
	void processEvent(const MidiEvent& event)
	{
		printf("MIDI_DEBUG: scheduling event at %ld.%ld\n",
			   				event.time.sec, event.time.usec);
		timer.queueEvent(self(),event);
	}
	void processCommand(const MidiCommand& command)
	{
		mcopbyte channel = command.status & mcsChannelMask;
		switch(command.status & mcsCommandMask)
		{
			case mcsNoteOn:  printf("MIDI_DEBUG: note on  channel %d, "
								   "note %d, velocity %d\n", channel,
								   command.data1, command.data2);
						break;
			case mcsNoteOff: printf("MIDI_DEBUG: note off channel %d, "
								   "note %d, velocity %d\n", channel,
								   command.data1, command.data2);
						break;
		}
	}

	TimeStamp time()
	{
		return timer.time();
	}

	TimeStamp playTime()
	{
		return timer.time();
	}
};

REGISTER_IMPLEMENTATION(Synth_MIDI_DEBUG_impl);
