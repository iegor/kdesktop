	/*

	Copyright (C) 2000, 2001 Stefan Westerfeld
                       stefan@space.twc.de, Matthias Kretz <kretz@kde.org>

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
#include "convert.h"
#include "mcoputils.h"
#include "stdsynthmodule.h"
#include "debug.h"
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

using namespace std;
namespace Arts {

class Synth_CAPTURE_WAV_impl :virtual public Synth_CAPTURE_WAV_skel,
							  virtual public StdSynthModule
{
	bool audioinit, scaleerr, running;
	int audiofd, byteorder, v,datalen,channels;

	unsigned char *outblock;
	unsigned long maxsamples;

	string _filename;

/****

   WAV writing code and header taken from kwave. Many thanks go to
   Martin Wilz who has written this ;)

 ****/

	struct wavheader
	{
    	char        riffid[4];
    	long        filelength;
    	char        wavid[4];
    	char        fmtid[4];
    	long        fmtlength;
    	short int   mode;
    	short int   channels;
    	long        rate;
    	long        AvgBytesPerSec;
    	short int   BlockAlign;
    	short int   bitspersample;
	} header;

public:
	Synth_CAPTURE_WAV_impl();
	void streamInit();
	void calculateBlock(unsigned long samples);
	void streamEnd();
	string filename() { return _filename; }
	void filename( const string &newFilename );
};

Synth_CAPTURE_WAV_impl::Synth_CAPTURE_WAV_impl()
	: running(false), _filename( "capture" )
{
}

void Synth_CAPTURE_WAV_impl::streamInit()
{
	/*
	 * we use createFilePath to prevent the usual symlink security issues
	 * in /tmp - add .wav manually as createFilePath substitutes . with _
	 */
	string filename = MCOPUtils::createFilePath(_filename)+ ".wav";
	audiofd = open(filename.c_str(),O_WRONLY|O_CREAT|O_TRUNC,0644);

/* write header */

	int rate=44100;
	int bit=16;

	channels = 2;		/* hardcoded here - make it a parameter? */

	arts_info("capturing output to %s", filename.c_str());
	datalen=0;

    strncpy (header.riffid,"RIFF",4);
    strncpy (header.wavid,"WAVE",4);
    strncpy (header.fmtid,"fmt ",4);
    header.fmtlength=16;
    header.filelength=sizeof(struct wavheader);
    header.mode=1;
    header.channels=channels;
    header.rate=rate;
    header.AvgBytesPerSec=rate*bit/8;
    header.BlockAlign=channels*bit/8;
    header.bitspersample=bit;

    write(audiofd,&header,sizeof (struct wavheader));
    write(audiofd,"data",4);
    write(audiofd,&datalen,4);

	maxsamples = 0;
	outblock = 0;
	v = 0;
	running = true;
}

void Synth_CAPTURE_WAV_impl::calculateBlock(unsigned long samples)
{
	if(samples > maxsamples)
	{
		maxsamples = samples;
		outblock = (unsigned char *)realloc(outblock, maxsamples * 4);
													// 2 channels, 16 bit
	}

	if(channels == 1)
		convert_mono_float_16le(samples,left,outblock);

	if(channels == 2)
		convert_stereo_2float_i16le(samples,left,right,outblock);

	write(audiofd,outblock,samples*channels*2);
	datalen += samples*channels*2;
}

void Synth_CAPTURE_WAV_impl::streamEnd()
{
/* rewrite header which now contains the correct size of the file */
	lseek(audiofd,0,SEEK_SET);
    header.filelength=sizeof(struct wavheader)+datalen;
    write(audiofd,&header,sizeof (struct wavheader));
    write(audiofd,"data",4);
    write(audiofd,&datalen,4);

	close(audiofd);

	running = false;
}
void Synth_CAPTURE_WAV_impl::filename( const string &newFilename )
{
	if(_filename != newFilename) {
		_filename = newFilename;
		if(running)
		{
			streamEnd();
			streamInit();
		}
		filename_changed(newFilename);
	}
}

REGISTER_IMPLEMENTATION(Synth_CAPTURE_WAV_impl);

}
