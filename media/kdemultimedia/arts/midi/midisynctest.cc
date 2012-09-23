    /*

    Copyright (C) 2002 Stefan Westerfeld
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

#include "config.h"
#include "artsmidi.h"
#include "soundserver.h"
#include "timestampmath.h"
#include "debug.h"
#include <stdio.h>
#include <math.h>

using namespace Arts;
using namespace std;

int main()
{
	Dispatcher dispatcher;

	MidiManager midiManager = DynamicCast(Reference("global:Arts_MidiManager"));
	if(midiManager.isNull())
		arts_fatal("midimanager is null");

	SoundServer soundServer = DynamicCast(Reference("global:Arts_SoundServer"));
	if(soundServer.isNull())
		arts_fatal("soundServer is null");

	MidiSyncGroup syncGroup = midiManager.addSyncGroup();
	MidiClient client = midiManager.addClient(mcdPlay, mctApplication, "midisynctest", "midisynctest");
	syncGroup.addClient(client);

	MidiPort port = client.addOutputPort();

	MidiClient client2 = midiManager.addClient(mcdPlay, mctApplication, "midisynctest2", "midisynctest2");

	syncGroup.addClient(client2);

	MidiPort port2 = client2.addOutputPort();

	/* setup audio synchronization */
	AudioSync audioSync;
	audioSync = DynamicCast(soundServer.createObject("Arts::AudioSync"));
	if(audioSync.isNull())
		arts_fatal("audioSync is null");

	syncGroup.addAudioSync(audioSync);

	const int C = 60;
	const int D = 62;
	const int E = 64;
	const int F = 65, f = F - 12;
	const int G = 67, g = G - 12;
	const int A = 69, a = A - 12;
	const int H = 71, h = H - 12;
	int np = 0;
	int notes[] = { C,E,G,E,a,C,E,C,f,a,C,a,g,h,D,h,0 };

	printf("connect port1 and port2 to two different ports in the artscontrol midi manager,\n"
			"hit return");
	getchar();

	TimeStamp t = port.time();
	timeStampInc(t,TimeStamp(0,100000));
	for(;;)
	{
		Synth_PLAY_WAV wav;
		Synth_AMAN_PLAY sap;

		MidiEvent e;

		e = MidiEvent(t,MidiCommand(mcsNoteOn|0, notes[np], 100));

		port.processEvent(e);
		port2.processEvent(e);

		if((np & 1) == 0)
		{
			/* setup wave player */
			wav = DynamicCast(soundServer.createObject("Arts::Synth_PLAY_WAV"));
			if(wav.isNull())
				arts_fatal("can't create Arts::Synth_PLAY_WAV");

			sap = DynamicCast(soundServer.createObject("Arts::Synth_AMAN_PLAY"));
			if(sap.isNull())
				arts_fatal("can't create Arts::Synth_AMAN_PLAY");

			wav.filename("/opt/kde3/share/sounds/pop.wav");
			sap.title("midisynctest2");
			sap.autoRestoreID("midisynctest2");
			connect(wav,sap);

			audioSync.queueStart(wav);
			audioSync.queueStart(sap);
			audioSync.executeAt(t);
		}

		timeStampInc(t,TimeStamp(0,100000));

		e = MidiEvent(t,MidiCommand(mcsNoteOff|0, notes[np], 100));

		port.processEvent(e);
		port2.processEvent(e);

		if((np & 1) == 0)
		{
			audioSync.queueStop(wav);
			audioSync.queueStop(sap);
			audioSync.executeAt(t);
		}

		timeStampInc(t,TimeStamp(0,400000));

		while(port.time().sec < (t.sec - 2))
			usleep(100000);

		np++;
		if(notes[np] == 0) np = 0;
	}
}
