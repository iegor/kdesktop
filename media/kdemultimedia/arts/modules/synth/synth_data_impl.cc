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
#include "artsmodulessynth.h"
#include "stdsynthmodule.h"

using namespace Arts;

// This module outputs a constant stream of data corresponding to the
// value given as it's parameter.

class Synth_DATA_impl : virtual public Synth_DATA_skel,
							virtual public StdSynthModule
{
protected:
	float _value;

public:
	float value() { return _value; }

  void value(float newValue) { _value = newValue; }

	void calculateBlock(unsigned long samples)
	{
		for (unsigned long i=0; i<samples; i++)
			outvalue[i] = _value;
	}
};

REGISTER_IMPLEMENTATION(Synth_DATA_impl);
