/*  This file is part of the KDE project
    Copyright (C) 2002-2003 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

*/
// $Id: synth_voice_removal_impl.cc 215419 2003-03-21 12:51:42Z akrille $

#include "artsmoduleseffects.h"
#include <stdsynthmodule.h>
#include <c_filter_stuff.h>
#include <algorithm>

using namespace Arts;
using namespace std;

class Synth_VOICE_REMOVAL_impl : virtual public Synth_VOICE_REMOVAL_skel,
                                 virtual public StdSynthModule
{
protected:
	float _position, _frequency;
	filter fleft, fright;

public:
	Synth_VOICE_REMOVAL_impl()
		: _position( 0 )
		, _frequency( 200 )
	{
	}

	float position() { return _position; }
	void position(float newPosition)
	{
		if(newPosition != _position)
		{
			_position = newPosition;
			position_changed(newPosition);
		}
	}

	float frequency() { return _frequency; }
	void frequency(float newFrequency)
	{
		if(newFrequency != _frequency)
		{
			_frequency = newFrequency;
			// the shelve-lowpass-filter is extremely sensitive to frequencies which
			// are out of it's range (it produces NaN's then) so we'll be careful ;)
			if(_frequency > 22000.0) _frequency = 22000.0;
			if(_frequency < 1.0) _frequency = 1.0;
			frequency_changed(_frequency);
		}
	}
	
	void streamInit()
	{
		initfilter(&fleft);
		initfilter(&fright);
	}
	
	void calculateBlock(unsigned long samples)
	{
		setfilter_shelvelowpass(&fleft,_frequency,80.0);
		setfilter_shelvelowpass(&fright,_frequency,80.0);

		unsigned long i;
		for (i=0; i<samples; i++)
		{
			fleft.x = inleft[i];// * min(float(1), (1 - _position));
			fleft.y = fleft.cx * fleft.x + fleft.cx1 * fleft.x1 + fleft.cx2
				* fleft.x2 + fleft.cy1 * fleft.y1 + fleft.cy2 * fleft.y2;
			fleft.x2 = fleft.x1;
			fleft.x1 = fleft.x;
			fleft.y2 = fleft.y1;
			fleft.y1 = fleft.y;
			float highleft = inleft[i] - 0.95 * fleft.y;

			fright.x = inright[i];// * min(float(1), (1 + _position));
			fright.y = fright.cx * fright.x + fright.cx1 * fright.x1 + fright.cx2
				* fright.x2 + fright.cy1 * fright.y1 + fright.cy2 * fright.y2;
			fright.x2 = fright.x1;
			fright.x1 = fright.x;
			fright.y2 = fright.y1;
			fright.y1 = fright.y;
			float highright = inright[i] - 0.95 * fright.y;

			outleft[i] = (inleft[i] - highright);// / min(float(1), (1 - _position));
			outright[i] = (inright[i] - highleft);// / min(float(1), (1 + _position));
		}
	}

};

REGISTER_IMPLEMENTATION(Synth_VOICE_REMOVAL_impl);

// vim: sw=4 ts=4
