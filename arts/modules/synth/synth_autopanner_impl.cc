/*

    Copyright (C) 2000 Jeff Tranter
                       tranter@pobox.com

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

#include "artsmodulessynth.h"
#include "stdsynthmodule.h"

using namespace Arts;

// An Autopanner is used to automatically pan the input signal between
// the left and the right output. This makes mixes more lively. A
// standard application would be a guitar or lead sound.  Connect a
// LFO, a sine or saw wave for example to "inlfo" and select a
// frequency between 0.1 and 5Hz for a traditional effect or even more
// for Special FX.

class Synth_AUTOPANNER_impl : virtual public Synth_AUTOPANNER_skel,
							virtual public StdSynthModule
{
public:
	void calculateBlock(unsigned long samples)
	{
		unsigned long i;
		for(i=0; i<samples; i++)
		{
			outvalue1[i] = invalue[i] * (1.0 - (inlfo[i] + 1.0) / 2.0);
			outvalue2[i] = invalue[i] * (inlfo[i] + 1.0) / 2.0;
		}
	}
};

REGISTER_IMPLEMENTATION(Synth_AUTOPANNER_impl);
