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
#ifndef AUDIOTIMER_H
#define AUDIOTIMER_H

#include "artsmidi.h"
#include "artsflow.h"
#include "stdsynthmodule.h"
#include "miditimercommon.h"
#include "flowsystem.h"

namespace Arts {

/**
 * EXPERIMENTAL (this interface will probably not stay this way) way to handle
 * audio based time stamps. Callback interface.
 */
class AudioTimerCallback {
public:
	virtual void updateTime() = 0;
};

/**
 * EXPERIMENTAL (this interface will probably not stay this way) way to handle
 * audio based time stamps. Timer class for audio timestamps.
 */
class AudioTimer : public SynthModule_skel, public StdSynthModule
				   /* we are a SynthModule to get the timing */
{
protected:
	AudioTimer();
	virtual ~AudioTimer();

	std::list<AudioTimerCallback *> callbacks;

	long samples;	/* at most samplingRate, overflow goes in seconds */
	long seconds;

public:
	// allocation: share one AudioTimer for everbody who needs one
	static AudioTimer *subscribe();
	void unsubscribe() { _release(); }

	void notify(const Notification& n);
	TimeStamp time();
	void calculateBlock(unsigned long samples);

	void addCallback(AudioTimerCallback *callback);
	void removeCallback(AudioTimerCallback *callback);
};

}

#endif
