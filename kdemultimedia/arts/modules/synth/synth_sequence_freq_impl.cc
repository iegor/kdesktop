/*  This file is part of the KDE project
    Copyright (C) 2000 Jeff Tranter <tranter@pobox.com>
                  2003 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include <stdio.h>
#include <stdlib.h>
#include "artsmodulessynth.h"
#include "stdsynthmodule.h"
#include <debug.h>

using namespace std;
using namespace Arts;

class Synth_SEQUENCE_FREQ_impl : virtual public Synth_SEQUENCE_FREQ_skel,
                                 virtual public StdSynthModule
{
protected:
	float _speed;
	string _seq;
	long posn, delay;
	float *fsequence;
	float *slen;

public:
	Synth_SEQUENCE_FREQ_impl()
		: _speed( 1 )
		, fsequence( 0 )
		, slen( 0 )
	{
	}

	~Synth_SEQUENCE_FREQ_impl()
	{
		delete[] fsequence;
		delete[] slen;
	}

	float speed() { return _speed; }
	void speed(float newSpeed) { _speed = newSpeed; }

	string seq() { return _seq; }
	void seq(const string &newSeq)
	{
		_seq = newSeq;
		parseSeqString();
	}

	void parseSeqString()
	{
		delete[] fsequence;
		delete[] slen;

		long bufferlen = _seq.length();
		fsequence = new float[ bufferlen ];
		slen = new float[ bufferlen ];

		int i = 0;
		int oldpos = 0;
		int pos = _seq.find_first_of( ",;", 0 );
		arts_debug( "tokenizer: parse %s", _seq.c_str() );
		while( pos > 0 )
		{
			string token = _seq.substr( oldpos, pos - oldpos );
			arts_debug( "tokenizer: pos = %d, oldpos = %d, token = %s", pos, oldpos, token.c_str() );
			handleToken( token, i++ );
			oldpos = pos + 1;
			pos = _seq.find_first_of( ",;", oldpos );
		}
		string token = _seq.substr( oldpos, _seq.length() - oldpos );
		arts_debug( "tokenizer: pos = %d, oldpos = %d, token = %s", pos, oldpos, token.c_str() );
		handleToken( token, i++ );
		fsequence[ i ] = -1.0;
	}

	void handleToken( const string & token, int i )
	{
		int colon = token.find( ':' );
		if( colon > -1 )
		{
			slen[ i ] = atof( &token.c_str()[ colon + 1 ] );
			fsequence[ i ] = atof( token.substr( 0, colon ).c_str() );
		}
		else
		{
			slen[ i ] = 1;
			fsequence[ i ] = atof( token.c_str() );
		}
	}

	void streamInit()
	{
		delay = 0;
		posn = 0;
	}

	void calculateBlock(unsigned long samples)
	{
		for	(unsigned int i=0; i < samples; i++)
		{
			delay++;
			if (delay > _speed * samplingRate * slen[posn])
			{
				posn++;
				if (fsequence[posn] == -1.0)
					posn = 0;
				delay = 0;
			}
			pos[i] = (int)delay / (_speed * samplingRate * slen[posn]);
			frequency[i] = fsequence[posn];
		}
	}

};

REGISTER_IMPLEMENTATION(Synth_SEQUENCE_FREQ_impl);
// vim: sw=4 ts=4 noet
