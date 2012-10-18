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

using namespace std;
using namespace Arts;

// You can use this for debugging. It will print out the value of the
// signal at invalue in regular intervals (ca. 1 second), combined
// with the comment you have specified. That way you can find out if
// some signals stay in certain ranges, or if they are there at all.

class Synth_DEBUG_impl : virtual public Synth_DEBUG_skel,
							virtual public StdSynthModule
{
protected:
	string _comment;
	int i;

public:
	string comment() { return _comment; }

  void comment(const string &newComment) { _comment = newComment; }

	void streamInit() { i = 0; }

	void calculateBlock(unsigned long samples)
	{
		for (unsigned long j=0; j<samples; j++)
		{
			i++;
			if ((i % 65536) == 0)
				printf("Synth_DEBUG: %s %f\n", _comment.c_str(), invalue[j]);
		}
	}
};

REGISTER_IMPLEMENTATION(Synth_DEBUG_impl);
