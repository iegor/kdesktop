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

#include <math.h>
#include "artsmodulessynth.h"
#include "stdsynthmodule.h"

using namespace Arts;

// The tremolo module modulates the amplitude according to a LFO-Wave.
// Traditionally you would use a sine wave but why limit yourself?
// What you get is a very intense effect that cuts through most
// arrangements because of its high dynamic range.  The tremolo effect
// is still one of guitarists favorite effects although it's not as
// popular as in the 1960's.

class Synth_TREMOLO_impl : virtual public Synth_TREMOLO_skel,
							virtual public StdSynthModule
{
public:
	void calculateBlock(unsigned long samples)
	{
		unsigned long i;
		for(i=0; i<samples; i++)
		{
			// simply modulate the amplitude
			outvalue[i] = invalue[i] * fabs(inlfo[i]);
		}
	}
};

REGISTER_IMPLEMENTATION(Synth_TREMOLO_impl);
