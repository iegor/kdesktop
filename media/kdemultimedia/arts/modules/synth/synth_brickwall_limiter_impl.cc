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

// A brickwall limiter is used to protect equipment (and your ears..)
// from peaks that exceed the dynamic range of your system. It doesn't
// sound good but it's better than digital distortion.

class Synth_BRICKWALL_LIMITER_impl : virtual public Synth_BRICKWALL_LIMITER_skel,
							virtual public StdSynthModule
{
public:
	void calculateBlock(unsigned long samples)
	{
		unsigned long i;
		for(i=0; i<samples; i++)
		{
			if (invalue[i] > 1.0)
				outvalue[i] = 1.0;
			else if (invalue[i] < -1.0)
				outvalue[i] = -1.0;
			else
				outvalue[i] = invalue[i];
		}
	}
};

REGISTER_IMPLEMENTATION(Synth_BRICKWALL_LIMITER_impl);
