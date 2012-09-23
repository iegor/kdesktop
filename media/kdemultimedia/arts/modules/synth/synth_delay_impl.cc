/*

    Copyright (C) 2000 Jeff Tranter
                       tranter@pobox.com
                       Stefan Westerfeld
                       stefan@space.twc.de
                       Jens Hahn
                       Jens.Hahn@t-online.de
                  2001 Matthias Kretz
                       kretz@kde.org

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

#include <math.h>

using namespace Arts;

// This delays the input signal for an amount of time. The time
// specification must be between 0 and 1 for a delay between 0 seconds
// and 1 second.
//
// This kind of delay may not be used in feedback structures. This is
// because it's a variable delay. You can modify it's length while it
// is running, and even set it down to zero. But since in a feedback
// structure the own output is needed to calculate the next samples, a
// delay whose value could drop to zero during synthesis could lead to
// a stall situation.
//
// Use CDELAYs in that setup, perhaps combine a small constant delay
// (of 0.001 seconds) with a flexible delay.
//
// You can also combine a CDELAY and a DELAY to achieve a variable
// length delay with a minimum value in a feedback loop. Just make
// sure that you have a CDELAY involved.

class Synth_DELAY_impl : virtual public Synth_DELAY_skel,
							virtual public StdSynthModule
{
protected:
	unsigned long _buffersize;
	unsigned long _bitmask;
	float * _buffer;
	float _maxdelay;
	unsigned int _writepos;
 
public:
	Synth_DELAY_impl() : _buffersize( 0 ), _bitmask( 0 ), _buffer( 0 ), _maxdelay( 0 ), _writepos( 0 )
	{
		maxdelay( 1 ); // take a one second buffer if nothing else is specified
	}

	~Synth_DELAY_impl()
	{
		delete[] _buffer;
	}

	void streamInit()
	{
		// initialize buffer to all zeroes
		for ( unsigned long i = 0; i < _buffersize; i++ )
			_buffer[i] = 0.0;
	}

	void calculateBlock(unsigned long samples)
	{
		for( unsigned long i = 0; i <samples; i++ )
		{
			double int_pos;
			double error = modf( time[i] * samplingRateFloat, &int_pos );
			unsigned long readpos1 = ( _writepos - (unsigned long)(int_pos) ) & _bitmask;
			unsigned long readpos2 = ( readpos1 - 1 ) & _bitmask; // Shouldn't this be +1? (mkretz)
			// No, it's right this way:
			// ( 1 - error ) needs to be multiplied with the second
			// sample; error with the first
			_buffer[_writepos] = invalue[i];
			outvalue[i] = _buffer[readpos1] * ( 1 - error ) + _buffer[readpos2] * error;
			_writepos++;
			_writepos &= _bitmask;
		}
	}

	float maxdelay() { return _maxdelay; }

	void maxdelay(float newmaxdelay)
	{
		if( newmaxdelay <= 0 )
			return;
		_maxdelay = newmaxdelay;
		double n = ceil( log( double(_maxdelay * samplingRateFloat) ) / log( 2. ) );
		unsigned long newbuffersize = (unsigned long)( pow( 2, n ) );
		unsigned long newbitmask = newbuffersize - 1;
		if( newbuffersize != _buffersize )
		{
			float *newbuffer = new float[newbuffersize];
			if( newbuffersize > _buffersize ) {
				for( unsigned long i = 0; i < _buffersize; i++ ) {
					newbuffer[i] = _buffer[_writepos];
					_writepos++;
					_writepos &= newbitmask;
				}
				for( unsigned long i = _buffersize; i < newbuffersize; i++ )
					newbuffer[i] = 0;
			} else {
				_writepos -= newbuffersize;
				_writepos &= newbitmask;
				for( unsigned long i = 0; i < newbuffersize; i++ ) {
					newbuffer[i] = _buffer[_writepos];
					_writepos++;
					_writepos &= newbitmask;
				}
			}
			_buffer = newbuffer;
			_buffersize = newbuffersize;
			_bitmask = newbitmask;
		}
		maxdelay_changed( _maxdelay );
	}         
};

REGISTER_IMPLEMENTATION(Synth_DELAY_impl);
