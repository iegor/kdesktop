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

class Synth_PSCALE_impl : virtual public Synth_PSCALE_skel,
						  virtual public StdSynthModule
{
protected:
	float _top;

public:
	float top() { return _top; }
	void top(float newTop) { _top = newTop; }

	void calculateBlock(unsigned long samples)
	{
		for	(unsigned int i=0; i < samples; i++)
		{
			if (pos[i] >= _top)
				outvalue[i] = invalue[i] * (1 - pos[i])/(1 - _top);
			else
				outvalue[i] = invalue[i] * pos[i] / _top;
		}
	}

};

REGISTER_IMPLEMENTATION(Synth_PSCALE_impl);
