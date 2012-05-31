/*

    Copyright (C) 2000 Jeff Tranter
                       tranter@pobox.com

              (C) 1999 Stefan Westerfeld
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

#include <math.h>
#include "artsmodulessynth.h"
#include "stdsynthmodule.h"

using namespace Arts;

class Synth_RC_impl : virtual public Synth_RC_skel,
								 virtual public StdSynthModule
{
protected:
	float _b, _f;
	float B, dB;
	float F, dF, oF, oU, U, Fsoll, Bsoll;
	float oldvalue;

public:
	float b() { return _b; }
	void b(float newB) { _b = newB; }

	float f() { return _f; }
	void f(float newF) { _f = newF; }

	void streamInit()
	{
		oldvalue = 0;
		B = 0;
		F = 0; oF = 0;
		U = 0; oU = 0;
	}
	
	void calculateBlock(unsigned long samples)
	{
		unsigned long i, hits;
		const float zero_lower = -0.00000001;
		const float zero_upper =  0.00000001;

		if (zero_lower < invalue[0] && invalue[0] < zero_upper)
		{
			/* for comments see equalizer.cc/Synth_STD_EQUALIZER implementation */
			
			/*
			 * This implementation differs from the implementation there,
			 * because it is done as a kind of powersafing. If no input is
			 * found, then no output is generated.
			 */
			if (zero_lower < oldvalue && oldvalue < zero_upper)
			{
				oldvalue = 0.0;
				B = 0.0;
				F = 0.0; oF = 0.0;
				U = 0.0; oU = 0.0;
				hits = 0;
				for (i=0; i<samples; i++)
				{
					if (zero_lower < invalue[i] && invalue[i] < zero_upper)
					{
						// try to zero out the whole block
						outvalue[i] = 0.0;
						hits++;
					}
				}
				if (hits == samples) return;
			}
		}

		for (i=0; i<samples; i++)
		{
			B = B + (invalue[i] - oldvalue); /* input into RC */
			oldvalue = invalue[i];

			Bsoll = U - oU;
			oU = U;
			dB = (Bsoll - B) / _b;
			B += dB;
			U -= dB;
			Fsoll = U;
			dF = (Fsoll - F) / _f;
			F += dF;                 /* Energie dF wird ins Feld uebertragen */
			U -= dF;
			outvalue[i] = (F - oF) * (_b + _f);
			oF = F;
		}
	}
};

REGISTER_IMPLEMENTATION(Synth_RC_impl);
