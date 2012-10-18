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

#ifndef MIDIMANAGERPORT_IMPL_H
#define MIDIMANAGERPORT_IMPL_H

#include "artsmidi.h"

namespace Arts {

class MidiClient_impl;

class MidiManagerPort_impl : public MidiPort_skel
{
	MidiClient_impl *client;
	SystemMidiTimer timer;
public:
	MidiManagerPort_impl(MidiClient_impl *client);
	TimeStamp time();
	TimeStamp playTime();
	void processCommand(const MidiCommand& command);
	void processEvent(const MidiEvent& event);
};

}

#endif /* MIDIMANAGERPORT_IMPL_H */
