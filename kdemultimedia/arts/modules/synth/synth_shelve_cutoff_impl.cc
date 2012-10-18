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
#include "stdsynthmodule.h"
#include "c_filter_stuff.h"

using namespace Arts;

class Synth_SHELVE_CUTOFF_impl : virtual public Synth_SHELVE_CUTOFF_skel,
								 virtual public StdSynthModule
{
protected:
	filter f;
public:
	void streamInit();
	void calculateBlock(unsigned long samples);
};

REGISTER_IMPLEMENTATION(Synth_SHELVE_CUTOFF_impl);

void Synth_SHELVE_CUTOFF_impl::streamInit()
{
	initfilter(&f);
}

void Synth_SHELVE_CUTOFF_impl::calculateBlock(unsigned long samples)
{
	float filterfrequency = frequency[0];

	// the shelve-lowpass-filter is extremely sensitive to frequencies which
	// are out of it's range (it produces NaN's then) so we'll be careful ;)
	if(filterfrequency > 22000.0) filterfrequency = 22000.0;
	if(filterfrequency < 1.0) filterfrequency = 1.0;
	setfilter_shelvelowpass(&f,filterfrequency,80.0);

	unsigned long i;

	for(i=0;i<samples;i++)
	{
		// better paste applyfilter here instead of:
		// *outsig++ = 0.95 * applyfilter(&f,*insig++);
  		f.x = invalue[i];
  		f.y = f.cx * f.x + f.cx1 * f.x1 + f.cx2 * f.x2
    		+ f.cy1 * f.y1 + f.cy2 * f.y2;
  		f.x2 = f.x1;
  		f.x1 = f.x;
  		f.y2 = f.y1;
  		f.y1 = f.y;
  		outvalue[i] = 0.95 * f.y;
	}
}
