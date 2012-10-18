/*

   Copyright (C) 2001 Matthias Kretz <kretz@kde.org>

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
#include "debug.h"

#include <math.h>
#include <string.h>

#ifndef LN2
# define LN2 0.69314718
#endif

#ifndef MAX
# define MAX(a,b) (((a) > (b) ? (a) : (b)))
#endif

using namespace std;
namespace Arts {

class Synth_COMPRESSOR_impl : virtual public Synth_COMPRESSOR_skel,
							  virtual public StdSynthModule
{
protected:
	float _attack, _release, _threshold, _ratiominus1, _output;
	float _attackfactor, _releasefactor;
	float _volume;
	float _compfactor;
	bool _autooutput;

public:
	float attack() { return _attack; }
	float release() { return _release; }
	float threshold() { return _threshold; }
	float ratio() { return _ratiominus1 + 1.0; }
	float output() { return _output; }

	Synth_COMPRESSOR_impl()
		: _threshold( 1 )
		, _ratiominus1( -0.2 )
		, _output( 0 )
		, _autooutput( true )
	{
		newCompFactor();
		attack( 10 );
		release( 10 );
	}

	void newCompFactor()
	{
		_compfactor = _output / pow( _threshold, _ratiominus1 );
	}

	void streamInit()
	{
		_volume = 0;
	}

	void calculateBlock(unsigned long samples)
	{
		for( unsigned long i = 0; i < samples; i++ ) {
			float delta = fabs( invalue[i] ) - _volume;
			if( delta > 0.0 )
				_volume += _attackfactor * delta;
			else
				_volume += _releasefactor * delta;

			if( _volume > _threshold )
				// compress
				// this is what it does:
				// UtodB(x) = 20 * log( x )
				// dBtoU(x) = pow( 10, x / 20 )
				// outvalue[i] = dBtoU( ( UtodB( volume ) - UtodB( threshold ) ) * ratio + UtodB( threshold ) ) / volume * output * invalue[ i ];
				// showing that it's equal to the formula below
				// is left as an exercise to the reader.
				outvalue[i] = pow( _volume, _ratiominus1 ) * _compfactor * invalue[ i ];
			else
				outvalue[i] = invalue[i] * _output;
		}
	}

	void attack( float newAttack )
	{ // in ms
		_attack = newAttack;
		// _attackfactor has to be <= 1, that's why we need the MAX here
		_attackfactor = LN2 / MAX( _attack / 1000 * samplingRateFloat, LN2 );
		attack_changed( newAttack );
	}

	void release( float newRelease )
	{ // in ms
		_release = newRelease;
		// _releasefactor has to be <= 1, that's why we need the MAX here
		_releasefactor = LN2 / MAX( _release / 1000 * samplingRateFloat, LN2 );
		release_changed( newRelease );
	}

	void threshold( float newThreshold )
	{ // in V not in dB
		_threshold = newThreshold;
		newCompFactor();
		threshold_changed( newThreshold );
	}

	void ratio( float newRatio )
	{
		_ratiominus1 = newRatio - 1;
		newCompFactor();
		ratio_changed( newRatio );
	}

	void output( float newOutput )
	{ // in V not in dB
		_output = newOutput;
		newCompFactor();
		output_changed( newOutput );
	}
};

REGISTER_IMPLEMENTATION(Synth_COMPRESSOR_impl);
}
