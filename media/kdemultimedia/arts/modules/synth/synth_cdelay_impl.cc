/*

    Copyright (C) 2000 Jeff Tranter
                       tranter@pobox.com
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
#include <cstring>

using namespace Arts;

// This delays the input signal for an amount of time. The time
// specification can be any number greater or equal zero.
// The delay is constant during the calculation, that means it
// can't be modified. This saves computing time as no interpolation is
// done, and is useful for recursive structures. Actually it can be
// modified, but without interpolation it won't sound too good. See
// the description for Synth_DELAY.

class Synth_CDELAY_impl : virtual public Synth_CDELAY_skel,
                          virtual public StdSynthModule
{
protected:
	unsigned long _buffersize;
	unsigned long _bitmask;
	float *_buffer; // holds the data to be delayed (variable size)
	float _delaytime;
	unsigned int _readpos;
	unsigned int _writepos;
 
public:
	Synth_CDELAY_impl() : _buffersize( 0 ), _bitmask( 0 ), _buffer( 0 ), _delaytime( 0 ), _readpos( 0 ), _writepos( 0 )
	{
	}
	
	~Synth_CDELAY_impl()
	{
		delete[] _buffer;
	}
	
	float time() { return _delaytime; }

	void time(float newTime)
	{
		_delaytime = newTime;
		double n = ceil( log( double(_delaytime * samplingRateFloat )) / log( 2. ) );
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
		_readpos = (unsigned long)rint( _writepos - _delaytime * samplingRateFloat ) & _bitmask;
		time_changed( _delaytime );
	}

	void streamInit()
	{
		// initialize buffer to all zeroes
		if( _buffer )
			for( unsigned long i = 0; i < _buffersize; i++ )
				_buffer[i] = 0.0;
	}

	void calculateBlock(unsigned long samples)
	{
		if( ! _buffer ) {
			memcpy( outvalue, invalue, sizeof( float ) * samples );
			return;
		}
		for( unsigned long i = 0; i < samples; i++ ) {
			_buffer[_writepos] = invalue[i];
			outvalue[i] = _buffer[_readpos];
			_readpos++;
			_readpos &= _bitmask;
			_writepos++;
			_writepos &= _bitmask;
		}
	}
};

// vim:sw=4:ts=4

REGISTER_IMPLEMENTATION(Synth_CDELAY_impl);
