	/*

	Copyright (C) 2000 Jeff Tranter
			           tranter@pobox.com

			  (C) 1999 Stefan Westerfeld
                       stefan@space.twc.de

			  (C) 2003 Matthias Kretz
                       kretz@kde.org

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "artsmodulessynth.h"
#include "stdsynthmodule.h"

using namespace std;
using namespace Arts;

class Synth_SEQUENCE_impl : virtual public Synth_SEQUENCE_skel,
							virtual public StdSynthModule
{
protected:
	float _speed;
	string _seq;
	long posn, delay;
	float *fsequence;
	float *slen;

public:
	Synth_SEQUENCE_impl()
		: _speed( 1 )
		, fsequence( 0 )
		, slen( 0 )
	{
	}

	~Synth_SEQUENCE_impl()
	{
		delete [] fsequence;
		delete [] slen;
	}

	float speed() { return _speed; }
	void speed(float newSpeed) { _speed = newSpeed; }

	string seq() { return _seq; }
	void seq(const string &newSeq) { _seq = newSeq; }

	void streamInit()
	{
		char notea[][4]= {"C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#-", "B-", "\0"};
		char noteb[][3]= {"C-", "Db", "D-", "Eb", "E-", "F-", "Gb", "G-", "Ab",  "A-", "Bb", "B-", "\0"};
		float freq[] = {261.7,277.2,293.7,311.2,329.7,349.3,370.0,392.0,415.3,440.0, 466.2, 493.9, 0 };
		float zhoch[] = {1,2,4,8,16,32,64,128,256};
		int s = 0, i, oktave;
		char *nptr;
		float f;
		char buffer[1024];

		strncpy(buffer, _seq.c_str(), 1023);
		buffer[ 1023 ] = '\0';
		long bufferlen = strlen(buffer);
		delete[] fsequence;
		delete[] slen;
		fsequence = new float[ bufferlen ];
		slen = new float[ bufferlen ];
		nptr = strtok(buffer, ",;");
		while (nptr)
		{
			if (nptr[3] == ':')
				slen[s] = atof(&nptr[4]);
			else
				slen[s] = 1;
			fprintf(stderr," <%d> %s\n", s, nptr);
			oktave = atol(&nptr[2]);
			nptr[2] = 0;
			f = 0;
			for (i=0; notea[i][0]; i++)
				if (strcmp(nptr,notea[i]) == 0)
					f = freq[i];
			for (i=0; noteb[i][0]; i++)
				if (strcmp(nptr, noteb[i]) == 0)
					f = freq[i];
			f *= zhoch[oktave] / zhoch[4];
			fsequence[s++] = f;
			fprintf(stderr, ">%2.2f\n", f);
			nptr = strtok(NULL,",;");
		}
		fsequence[s] = 0;
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
				if (fsequence[posn] == 0)
					posn = 0;
				delay = 0;
			}
			pos[i] = (int)delay / (_speed * samplingRate * slen[posn]);
			frequency[i] = fsequence[posn];
		}
	}

};

REGISTER_IMPLEMENTATION(Synth_SEQUENCE_impl);
// vim: sw=4 ts=4 noet
