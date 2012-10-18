#    /*

    Copyright (C) 2001 Stefan Westerfeld
                       stefan@space.twc.de

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
#include <sys/stat.h>
#include <stdsynthmodule.h>
#include <unistd.h>
#include <math.h>
#include <debug.h>
#include <cache.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace std;

namespace Arts {

namespace PatchLoader {
	typedef unsigned char byte;
	typedef unsigned short int word;
	typedef unsigned int dword;
	typedef char sbyte;
	typedef short int sword;
	typedef int sdword;

	static int pos = 0;
	static int apos = 0;

	inline void xRead(FILE *file, int len, void *data)
	{
		//	printf("(0x%2x) - 0x%02x  ... reading %d bytes\n",apos,pos,len);
		pos += len;
		apos += len;
		if(fread(data, len, 1, file) != 1)
			fprintf(stdout, "short read\n");
	}

	inline void skip(FILE *file, int len)
	{
		//	printf("(0x%2x) - 0x%02x  ... skipping %d bytes\n",apos,pos,len);
		pos += len;
		apos += len;
		while(len > 0)
		{
			char junk;
			if(fread(&junk, 1, 1, file) != 1)
				fprintf(stdout, "short read\n");
			len--;
		}
	}


	inline void readBytes(FILE *file, unsigned char *bytes, int len)
	{
		xRead(file, len, bytes);
	}

	inline void readString(FILE *file, char *str, int len)
	{
		xRead(file, len, str);
	}

	/* readXXX with sizeof(xxx) == 1 */
	inline void readByte(FILE *file, byte& b)
	{
		xRead(file, 1, &b);
	}

	/* readXXX with sizeof(xxx) == 2 */
	inline void readWord(FILE *file, word& w)
	{
		byte h, l;
		xRead(file, 1, &l);
		xRead(file, 1, &h);
		w = (h << 8) + l;
	}

	inline void readSWord(FILE *file, sword& sw)
	{
		word w;
		readWord(file, w);
		sw = (sword)w;
	}

	/* readXXX with sizeof(xxx) == 4 */
	inline void readDWord(FILE *file, dword& dw)
	{
		byte h, l, hh, hl;
		xRead(file, 1, &l);
		xRead(file, 1, &h);
		xRead(file, 1, &hl);
		xRead(file, 1, &hh);
		dw = (hh << 24) + (hl << 16) + (h << 8) + l;
	}

	struct PatHeader {
		char id[12];					/* ID='GF1PATCH110' */
		char manufacturer_id[10];		/* Manufacturer ID */
		char description[60];			/* Description of the contained Instruments
										   or copyright of manufacturer. */
		byte instruments;				/* Number of instruments in this patch */
		byte voices;					/* Number of voices for sample */
		byte channels;					/* Number of output channels
										   (1=mono,2=stereo) */
		word waveforms;					/* Number of waveforms */
		word mastervolume;				/* Master volume for all samples */
		dword size;						/* Size of the following data */
		char reserved[36];				/* reserved */

		PatHeader(FILE *file)
		{
			readString(file, id, 12);
			readString(file, manufacturer_id, 10);
			readString(file, description, 60);
			/*		skip(file, 2);*/

			readByte(file, instruments);
			readByte(file, voices);
			readByte(file, channels);

			readWord(file, waveforms);
			readWord(file, mastervolume);
			readDWord(file, size);

			readString(file, reserved, 36);
		}
	};

	struct PatInstrument {
		word	number;
		char	name[16];
		dword 	size;					/* Size of the whole instrument in bytes. */
		byte	layers;
		char	reserved[40];

		/* layer? */
		word	layerUnknown;
		dword	layerSize;
		byte	sampleCount;			/* number of samples in this layer (?) */
		char	layerReserved[40];

		PatInstrument(FILE *file)
		{
			readWord(file, number);
			readString(file, name, 16);
			readDWord(file, size);
			readByte(file, layers);
			readString(file, reserved, 40);

			/* layer: (?) */
			readWord(file, layerUnknown);
			readDWord(file, layerSize);
			readByte(file, sampleCount);
			readString(file, reserved, 40);
		}
	};

	struct PatPatch {
		char	filename[7];			/* Wave file name */
		byte	fractions;				/* Fractions */
		dword	wavesize;				/* Wave size.
										   Size of the wave digital data */
		dword	loopStart;
		dword	loopEnd;
		word	sampleRate;
		dword	minFreq;
		dword	maxFreq;
		dword	origFreq;
		sword	fineTune;
		byte	balance;
		byte	filterRate[6];
		byte	filterOffset[6];
		byte	tremoloSweep;
		byte	tremoloRate;
		byte	tremoloDepth;
		byte	vibratoSweep;
		byte	vibratoRate;
		byte	vibratoDepth;
		byte	waveFormat;
		sword	freqScale;
		word	freqScaleFactor;
		char	reserved[36];

		PatPatch(FILE *file)
		{
			readString(file, filename, 7);
			readByte(file, fractions);
			readDWord(file, wavesize);
			readDWord(file, loopStart);
			readDWord(file, loopEnd);
			readWord(file, sampleRate);
			readDWord(file, minFreq);
			readDWord(file, maxFreq);
			readDWord(file, origFreq);
			readSWord(file, fineTune);
			readByte(file, balance);
			readBytes(file, filterRate, 6);
			readBytes(file, filterOffset, 6);
			readByte(file, tremoloSweep);
			readByte(file, tremoloRate);
			readByte(file, tremoloDepth);
			readByte(file, vibratoSweep);
			readByte(file, vibratoRate);
			readByte(file, vibratoDepth);
			readByte(file, waveFormat);
			readSWord(file, freqScale);
			readWord(file, freqScaleFactor);
			readString(file, reserved, 36);
		}
	};
}

class CachedPat : public CachedObject
{
protected:
	struct stat oldstat;
	string filename;
	bool initOk;
	long dataSize;

	CachedPat(Cache *cache, const string& filename);
	~CachedPat();

public:

	struct Data {
		PatchLoader::PatPatch patch;
		mcopbyte *rawdata;
		Data(FILE *file)
			: patch(file)
		{
			rawdata = new mcopbyte[patch.wavesize];
			fread(rawdata, 1, patch.wavesize, file);

			// sign conversion only for 16bit!
			if(patch.waveFormat & (1 << 1))
			{
				for(unsigned int i = 1; i < patch.wavesize; i+=2)
					rawdata[i] ^= 0x80;
			}

			// unfold ping-pong loops
			if(patch.waveFormat & (1 << 3))
			{
				int looplen = patch.loopEnd - patch.loopStart;
				arts_assert(looplen > 0);

				mcopbyte *newdata = new mcopbyte[patch.wavesize + looplen];

				// copy head
				memcpy(&newdata[0], &rawdata[0], patch.loopStart + looplen);

				// 16 bit unfolding only:
				for(int i=0; i<looplen; i+=2)
				{
					newdata[patch.loopStart+looplen+i] =
						newdata[patch.loopStart+looplen-i-2];
					newdata[patch.loopStart+looplen+i+1] =
						newdata[patch.loopStart+looplen-i-1];
				}

				// copy tail:
				memcpy(&newdata[patch.loopStart+2*looplen],
					   &rawdata[patch.loopStart+looplen],
					   patch.wavesize - patch.loopEnd);

				delete[] rawdata;
				rawdata = newdata;

				patch.wavesize += looplen;
				patch.loopEnd += looplen;
				patch.waveFormat &= ~(1 << 3);
			}
		}
		~Data()
		{
			delete[] rawdata;
		}
	};

	list<Data*> dList;

	static CachedPat *load(Cache *cache, const string& filename);
	/**
	 * validity test for the cache - returns false if the object is having
	 * reflecting the correct contents anymore (e.g. if the file on the
	 * disk has changed), and there is no point in keeping it in the cache any
	 * longer
	 */
	bool isValid();
	/**
	 * memory usage for the cache
	 */
	int memoryUsage();
};

CachedPat *CachedPat::load(Cache *cache, const string& filename)
{
	CachedPat *pat;

	pat = (CachedPat *)cache->get(string("CachedPat:")+filename);
	if(!pat) {
		pat = new CachedPat(cache, filename);

		if(!pat->initOk)		// loading failed
		{
			pat->decRef();
			return 0;
		}
	}

	return pat;
}

bool CachedPat::isValid()
{
	if(!initOk)
		return false;

	struct stat newstat;

	lstat(filename.c_str(),&newstat);
	return(newstat.st_mtime == oldstat.st_mtime);
}

int CachedPat::memoryUsage()
{
	return dataSize;
}

CachedPat::CachedPat(Cache *cache, const string& filename)
	: CachedObject(cache), filename(filename), initOk(false), dataSize(0)
{
	setKey(string("CachedPat:")+filename);

	if(lstat(filename.c_str(),&oldstat) == -1)
	{
		arts_info("CachedPat: Can't stat file '%s'", filename.c_str());
		return;
	}

	FILE *patfile = fopen(filename.c_str(), "r");
	if(patfile)
	{
		//PatchLoader::PatHeader header(patfile);
		PatchLoader::PatInstrument ins(patfile);

		for(int i=0;i<ins.sampleCount;i++)
		{
			Data *data = new Data(patfile);
			dList.push_back(data);
			dataSize += data->patch.wavesize;
		}
		fclose(patfile);

		arts_debug("loaded pat %s",filename.c_str());
		arts_debug("  %d patches, datasize total is %d bytes",
											ins.sampleCount, dataSize);

		initOk = true;
	}
}

CachedPat::~CachedPat()
{
	while(!dList.empty())
	{
		delete dList.front();
		dList.pop_front();
	}
}

class Synth_PLAY_PAT_impl : virtual public Synth_PLAY_PAT_skel,
							virtual public StdSynthModule
{
protected:
	string _filename;
	CachedPat *pat;
	CachedPat::Data *selected;
	float fpos;

public:
	Synth_PLAY_PAT_impl()
	{
		pat = 0;
		selected = 0;
	}

	void unload()
	{
		if(pat)
		{
			pat->decRef();
			pat = 0;
		}
	}

	~Synth_PLAY_PAT_impl()
	{
		unload();
	}

	void streamStart()
	{
		fpos = 0.0;
		selected = 0;
	}
	
	void calculateBlock(unsigned long samples)
	{
		/* the normal offset is 60
		   60 = 12*5
		   so we scale with 2^5 = 32
		 */
		float freq = frequency[0]; /* * 32.0 / 2.0; * why /2.0? */
		int ifreq = (int)(freq * 1024.0);
		if(!selected && pat)
		{
			int bestdiff = 20000 * 1024;

			list<CachedPat::Data*>::iterator i;
			for(i = pat->dList.begin(); i != pat->dList.end(); i++)
			{
				int diff = ::abs(double(ifreq - (*i)->patch.origFreq));
				if(diff < bestdiff)
				{
					selected = *i;
					bestdiff = diff;
				}
			}

			/* drums */
			if(selected && selected->patch.freqScaleFactor == 0)
				ifreq = selected->patch.origFreq;
		}
		if(selected)
		{
			const PatchLoader::PatPatch& patch = selected->patch;

			/*
			 * thats just a *guess*, I have no idea how tuning actually
			 * should work
			 */
#if 0
			float tonetune = float(pat.fineTune)/float(pat.freqScaleFactor);
			float tuning = pow(2.0, tonetune/12.0);
			freq *= tuning;
#endif
			//printf("tonetune: %f\n",tonetune);
			//printf("tuning: %f\n",tuning);

			float step = double(patch.sampleRate)/samplingRateFloat *
				  		 float(ifreq) / float(patch.origFreq);
			for(unsigned int i = 0; i < samples; i++)
			{
				// ASSUME 16bit signed, native byte order
				int ifpos = int(fpos)*2;

				//                     looped?      bidi?     backw?
				if((patch.waveFormat & ((1 << 2) + (1 << 3) + (1 << 4)))
							== (1 << 2))
				{
					while(ifpos >= signed(patch.loopEnd))
					{
						ifpos -= (patch.loopEnd - patch.loopStart);
						fpos -= (patch.loopEnd - patch.loopStart)/2;
					}
				}
				
				short int *x = (short int *)&selected->rawdata[ifpos];
				float sample = (ifpos >= 0 && ifpos < signed(patch.wavesize))?
										x[0]/32768.0:0.0;
				float sample1 = ((ifpos+2) >= 0 && (ifpos+2) < signed(patch.wavesize))?
										x[1]/32768.0:0.0;
				float error = fpos - (int)fpos;
				outvalue[i] = sample * (1.0-error) + sample1 * error;

				fpos += step;
			}
		}
		else
		{
			for(unsigned int i = 0; i < samples; i++)
				outvalue[i] = 0.0;
		}
	}

	void clearDList()
	{
		selected = 0;

	}

	string filename() { return _filename; }
	void filename(const string& newFile)
	{
		if(newFile == _filename) return;
	
		unload();
		pat = CachedPat::load(Cache::the(), newFile);

		_filename = newFile;
		filename_changed(newFile);
	}
};

REGISTER_IMPLEMENTATION(Synth_PLAY_PAT_impl);

}
