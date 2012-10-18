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

#include <stdio.h>
#include <math.h>
#include "artsmodulessynth.h"
#include "stdsynthmodule.h"

using namespace Arts;

// This can be used to get the input signal to the normalized range
// between -1 and 1 that Synth_PLAY can process. The louder the input
// signal, the more the signal is distorted by this module. For very
// small input signals, the output signal is about the input signal
// (no change).

class Synth_ATAN_SATURATE_impl : virtual public Synth_ATAN_SATURATE_skel,
							virtual public StdSynthModule
{
protected:
	float _inscale;

public:
	float inscale() { return _inscale; }

  void inscale(float newInscale) { _inscale = newInscale; }

	void calculateBlock(unsigned long samples)
	{
		for (unsigned long i=0; i<samples; i++)
			outvalue[i] = atan(invalue[i]*_inscale)/(M_PI/2.0);
	}
};

REGISTER_IMPLEMENTATION(Synth_ATAN_SATURATE_impl);
