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

#include "artsmodulessynth.h"
#include "stdsynthmodule.h"

using namespace Arts;

/*
Try this. It's a really nice 4pole. Very Moog like.

in[x] and out[x] are member variables, init to 0.0
the controls:

fc = cutoff, nearly linear [0,1] -> [0, fs/2]
res = resonance [0, 4] -> [no resonance, self-oscillation]
*/

class Synth_MOOG_VCF_impl : virtual public Synth_MOOG_VCF_skel,
							virtual public StdSynthModule
{
protected:
	float _frequency, _resonance;
	double freqcorrect;
	double in1, in2, in3, in4;
	double out1, out2, out3, out4;

public:
	float frequency() { return _frequency; }
	void frequency(float newFrequency) { _frequency = newFrequency; }

	float resonance() { return _resonance; }
	void resonance(float newResonance) { _resonance = newResonance; }

	void streamInit()
	{
		in1 = in2 = in3 = in4 = out1 = out2 = out3 = out4 = 0.0;
	}
	
	void calculateBlock(unsigned long samples)
	{
		freqcorrect = 1.16 / (double)(samplingRate / 2);

		for	(unsigned int i=0; i < samples; i++)
		{
			double input = invalue[i];
			double fc = _frequency;
			double res = _resonance;
			double f = fc * freqcorrect;
			double fb = res * (1.0 - 0.15 * f * f);

			input -= out4 * fb;
			input *= 0.35013 * (f * f) * (f * f);
			
			out1 = input + 0.3 * in1 + (1 - f) * out1; // Pole 1
			in1  = input;
			out2 = out1 + 0.3 * in2 + (1 - f) * out2;  // Pole 2
			in2  = out1;
			out3 = out2 + 0.3 * in3 + (1 - f) * out3;  // Pole 3
			in3  = out2;
			out4 = out3 + 0.3 * in4 + (1 - f) * out4;  // Pole 4
			in4  = out3;
			
			outvalue[i] = out4;
		}
	}

};

REGISTER_IMPLEMENTATION(Synth_MOOG_VCF_impl);
