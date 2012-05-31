/*

    Copyright (C) 2000 Jeff Tranter
                       tranter@pobox.com
                       Stefan Westerfeld
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

#include <math.h>
#include "artsmodulessynth.h"
#include "stdsynthmodule.h"

using namespace Arts;

class Synth_FX_CFLANGER_impl : virtual public Synth_FX_CFLANGER_skel,
							virtual public StdSynthModule
{
protected:
	float _mintime;
	float _maxtime;
	enum { SAMPLINGRATE = 44100, MAXDELAY = 44100 };
	float *dbuffer;
	unsigned long dbpos;
	float center;
	float range;
 
public:
	Synth_FX_CFLANGER_impl()
	{
		dbuffer=new float[MAXDELAY];
	}
	~Synth_FX_CFLANGER_impl()
	{
		delete [] dbuffer;
	}

	float mintime() { return _mintime; }

	void mintime(float newMintime) { _mintime = newMintime; }

	float maxtime() { return _maxtime; }

	void maxtime(float newMaxtime) { _maxtime = newMaxtime; }

	void streamInit()
	{
		center = (_maxtime + _mintime) / 2;
		range = _maxtime - center;
		for (int i=0; i<MAXDELAY; i++)
			dbuffer[i] = 0;
		dbpos = 0;
	}

	void calculateBlock(unsigned long samples)
	{
		unsigned long i;
		float delay, floor_delay;
		long start_pos, end_pos;
		float start_val, end_val;
 
		for(i=0; i<samples; i++)
		{
			dbuffer[dbpos] = invalue[i];
			// Delaytime i.e. = 35ms + (+/- LFO[-1 bis 1] * 15ms) / 1000 * 44100
			delay = ((center + (lfo[i] * range)) / 1000.0) * (float) SAMPLINGRATE;
			floor_delay = floor(delay);
			start_pos = dbpos - (long)(floor_delay);
			end_pos = start_pos-1;
			if (start_pos < 0) start_pos += MAXDELAY; // wrapping exception
			if (end_pos < 0) end_pos += MAXDELAY;
			start_val = dbuffer[start_pos];
			end_val = dbuffer[end_pos];
			outvalue[i] = start_val + ((delay - floor_delay) * (end_val - start_val));
			dbpos++;
			if (dbpos == MAXDELAY) dbpos = 0;
		}
	}
};

REGISTER_IMPLEMENTATION(Synth_FX_CFLANGER_impl);
