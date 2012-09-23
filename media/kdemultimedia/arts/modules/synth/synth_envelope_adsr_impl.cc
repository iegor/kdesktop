    /*

    Copyright (C) 2000 Stefan Westerfeld
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

#include "artsmodulessynth.h"
#include "debug.h"
#include "stdsynthmodule.h"

using namespace Arts;

class Synth_ENVELOPE_ADSR_impl : virtual public Synth_ENVELOPE_ADSR_skel,
							virtual public StdSynthModule
{
protected:
	enum { NOOUT, ATTACK, SUSTAIN, DECAY, RELEASE } currentphase;
	float level,increment,decrement;
public:
	void streamInit()
	{
		currentphase = NOOUT;
		level = 0;
	}
	void calculateBlock(unsigned long samples);
};

void Synth_ENVELOPE_ADSR_impl::calculateBlock(unsigned long samples)
{
	/* FIXME:
     * should be rewritten as generic envelope, would certainly
	 * be faster & more flexible
	 */
	unsigned long i;

	for(i=0;i<samples;i++)
	{
		done[i] = 0;
		if(active[i] < 0.5)
		{
			if(currentphase == NOOUT)
			{
				level = 0;
				done[i] = 1;
			}
			else
			{
				if(currentphase != RELEASE) {
					artsdebug("ADSR: entering release phase\n");
					currentphase = RELEASE;
					decrement = level / (release[i] * samplingRateFloat);
				}
				level -= decrement;
				if(level <= 0)
				{
					level = 0;
					currentphase = NOOUT;
				}
			}
		}
		else
		{
			switch(currentphase)
			{
				//quickly kill the note that is still there (channel busy ;)
				case RELEASE:
						level -= 1/200;
						if(level <= 0) {
							currentphase = NOOUT;
							level = 0;
						}
					break;
				case NOOUT:
						artsdebug("ADSR: entering attack\n");
						increment = 1 / (attack[i] * samplingRateFloat);
						currentphase = ATTACK;
					break;
				case ATTACK:
						level += increment;
						if (level >= 1)
						{
							level = 1;
							currentphase = DECAY;
							decrement = (1-sustain[i]) /
										(decay[i] * samplingRateFloat);
						}
					break;
				case DECAY:
						level -= decrement;
						if (level <= sustain[i])
						{
							level = sustain[i];
							currentphase = SUSTAIN;
						}
					break;
				case SUSTAIN:
						level = sustain[i];
					break;
			}
		}
		outvalue[i] = invalue[i] * level;
	}
}

REGISTER_IMPLEMENTATION(Synth_ENVELOPE_ADSR_impl);
