/*

    Copyright (C) 2000 Jeff Tranter
                       tranter@pobox.com

              (C) 1999 Stefan Westerfeld
                       stefan@space.twc.de

              (C) 1999 Martin Lorenz
                       lorenz@ch.tum.de

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

class Synth_STD_EQUALIZER_impl : virtual public Synth_STD_EQUALIZER_skel,
								 virtual public StdSynthModule
{
protected:
	float _low, _mid, _high, _frequency, _q;
	float tlow, tmid, thigh, tfrequency;
	float a1, a2, b0, b1, b2, x_0, x_1, x_2, y_1, y_2;
	unsigned long all;

public:
	float low() { return _low; }
	void low(float newLow)
	{
		if(newLow != _low)
		{
			_low = newLow;
			calcParameters();
			high_changed(newLow);
		}
	}


	float mid() { return _mid; }
	void mid(float newMid)
	{
		if(newMid != _mid)
		{
			_mid = newMid;
			calcParameters();
			mid_changed(newMid);
		}
	}

	float high() { return _high; }
	void high(float newHigh)
	{
		if(newHigh != _high)
		{
			_high = newHigh;
			calcParameters();
			high_changed(newHigh);
		}
	}


	float frequency() { return _frequency; }
	void frequency(float newFrequency)
	{
		if(newFrequency != _frequency)
		{
			_frequency = newFrequency;
			calcParameters();
			frequency_changed(newFrequency);
		}
	}

	float q() { return _q; }
	void q(float newQ)
	{
		if(newQ != _q)
		{
			_q = newQ;
			calcParameters();
			q_changed(newQ);
		}
	}

	Synth_STD_EQUALIZER_impl() {
		_low = _mid = _high = 0; _q = 0.5;
		_frequency = 300;
	}
	
	void calcParameters()
	{
		/*

		 * _low, _mid, _high are in dB, transform them to tlow, tmid,
		 * thigh using:
		 * -6dB => 0.5 ; 0dB => 1 ; 6dB = 2.0 ; ... 
		 */
		
		tlow = exp(_low * 0.115524530093324);		// exp(p[LOW]*ln(2)/6)
		tmid = exp(_mid * 0.115524530093324);
		thigh = exp(_high * 0.115524530093324);

		// _frequency is given in Hz, we need the w-value (and do clipping if
		// it exceeds SR/2)
		const float SAMPLING_RATE = 44100.0;
		tfrequency = _frequency;
		if (tfrequency > SAMPLING_RATE / 2.01)
			tfrequency = SAMPLING_RATE / 2.01;
		float w = 2 * M_PI * tfrequency / SAMPLING_RATE;

		// Calculations:
		float t = 1/tan(w/2);
		float tq = t/_q;
		float t2 = t*t;

		float a0 = 1+tq+t2;
		float a0r = 1/a0;

		// and now the real filter values:
		a1 = (2 - 2 * t2) * a0r;
		a2 = (1 - tq + t2) * a0r;
		b0 = (tlow + tmid * tq + thigh * t2) * a0r;
		b1 = (2 * tlow -2 * thigh * t2) * a0r;
		b2 = (tlow - tmid * tq + thigh * t2) * a0r;

		// TODO: try if we need that here, or if we can change filter
		// coefficients without setting the state to 0
		x_0 = x_1 = x_2 = y_1 = y_2 = 0.0;
		all = 0;
	}

	void streamInit()
	{
		calcParameters();
	}
	
	void calculateBlock(unsigned long samples)
	{
		all += samples;

		if (all > 1024)
		{
			/* The _problem_: (observed on a PII-350)
			 *
			 * I am not quite sure what happens here, but it seems to be like that:
			 * 
			 * If an ordinary signal (a mp3 for instance) is sent through the
			 * equalizer, and then no more input is given (zeros as input),
			 * the y_1 and y_2 values oscillate for some time, coming closer and
			 * close to zero.
			 *
			 * But before the reach zero, they reach the smallest negative number
			 * (or smallest positive, or whatever), and stay there
			 * (because 0.005*smallest_negative will remain smallest_negative).
			 *
			 * Since then, the CPU usage for all operations on these floats
			 * increases, (since handling of smallest_negative seems to be a rare
			 * case).
			 *
			 * The _fix_:
			 *
			 * We observe the value of y_1. If it's very close to zero (may be as
			 * well smallest_positive/smallest_negative), we set it to zero,
			 * together with y_2. This shouldn't significantly influence
			 * correctness of the filter, but effectively solves the problem.
			 *
			 * If you don't believe me, try without this fix and tell me what
			 * happens on your computer.
			 */
			const float zero_lower =-0.00000001;
			const float zero_upper = 0.00000001;
			all = 0;
			
			if(zero_lower < y_1 && y_1 < zero_upper)
				y_1 = y_2 = 0.0;
		}
		
		unsigned long i;
		float tmp;
		for (i=0; i<samples; i++)
		{
			x_0 = invalue[i];
			tmp = x_0 * b0 + x_1 * b1 + x_2 * b2 - y_1 * a1 - y_2 * a2;
			x_2 = x_1; x_1 = x_0; y_2 = y_1; y_1 = tmp;
			outvalue[i] = tmp;
		}
	}

};

REGISTER_IMPLEMENTATION(Synth_STD_EQUALIZER_impl);
