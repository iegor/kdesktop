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

// This is used for frequency modulation. Put your frequency to the
// frequency input and put another signal on the modulator input. Then
// set modlevel to something, say 0.3. The frequency will be modulated
// with modulator then. Just try it. Works nice when you put a
// feedback in there, that means take a combination of the delayed
// output signal from the Synth_FM_SOURCE (you need to put it to some
// oscillator as it only takes the role of Synth_FREQUENCY) and some
// other signal to get good results. Works nicely in combination with
// Synth_WAVE_SIN oscillators.

class Synth_FM_SOURCE_impl : virtual public Synth_FM_SOURCE_skel,
							virtual public StdSynthModule
{
protected:
	static const int SAMPLINGRATE = 44100;
	float posn;
 
public:
	void streamInit() { posn = 0; }

	void calculateBlock(unsigned long samples)
	{
		for (unsigned long i=0; i<samples; i++)
		{
			float pinc = frequency[i] / (float) SAMPLINGRATE;
			posn += pinc;
			if (posn > 1)
				posn -= 1;
			pos[i] = posn + modulator[i] * modlevel[i];
		}
	}
};

REGISTER_IMPLEMENTATION(Synth_FM_SOURCE_impl);
