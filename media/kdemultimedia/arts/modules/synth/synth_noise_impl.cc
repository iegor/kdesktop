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

#include <stdlib.h>

#include "artsmodulessynth.h"
#include "stdsynthmodule.h"

using namespace Arts;

namespace Arts {

#define NOISE_SIZE 8192

class Synth_NOISE_impl : virtual public Synth_NOISE_skel,
							virtual public StdSynthModule
{
	static float noise[NOISE_SIZE];
	static bool noiseInit;
	unsigned long pos;
public:
	Synth_NOISE_impl()
	{
		if(!noiseInit)
		{
			for(unsigned long i=0;i<NOISE_SIZE;i++)
				noise[i] = ((float)rand()/(float)RAND_MAX)*2.0-1.0;
			noiseInit = true;
		}
	}
	void calculateBlock(unsigned long samples)
	{
		unsigned long i;
		pos = rand();
		for(i=0;i<samples;i++) outvalue[i] = noise[pos++ & (NOISE_SIZE-1)];
	}
};

float Synth_NOISE_impl::noise[8192];
bool Synth_NOISE_impl::noiseInit = false;

REGISTER_IMPLEMENTATION(Synth_NOISE_impl);

}
