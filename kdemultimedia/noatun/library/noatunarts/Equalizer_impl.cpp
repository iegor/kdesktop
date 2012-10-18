/*
Copyright (C) 2001 Charles Samuels   <charles@kde.org>

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

#include "noatunarts.h"
#include "artsflow.h"
#include "fft.h"
#include <stdsynthmodule.h>
#include <math.h>
#include <cstring>

using namespace std;
using namespace Arts;

/**
 * This class is _VERY_ picky
 * which is why Noatun has it's own Equalizer class,
 * that does all the error checking and sends it to here
 **/

namespace Noatun
{

void resize(vector<float> &vec, unsigned int newsize)
{
	while (newsize < vec.size())
		vec.pop_back();
	while (newsize > vec.size())
		vec.push_back(0.0);
}

class Equalizer_impl : public Equalizer_skel, public StdSynthModule
{
	vector<float> mLevels;

	vector<BandPassInfo> mBandLeft, mBandRight;
	
	vector<float> mLevelWidths;
	vector<float> mLevelCenters;

	bool mEnabled;
	float mPreamp;
		

	float *mBuffer;
	unsigned int mBufferLength;

	void reinit()
	{
		mBandLeft.clear();
		mBandRight.clear();
		for (unsigned int i=0; i< mLevelWidths.size(); ++i)
		{
			BandPassInfo nfo;
			BandPassInit(&nfo, mLevelCenters[i], mLevelWidths[i]);
			mBandLeft.push_back(nfo);
			mBandRight.push_back(nfo);
		}
		
	}
	
public:
	void set(const std::vector<float>& levels, const std::vector<float>& centers, const std::vector<float>& widths)
	{
		mLevelCenters=centers;
		mLevelWidths=widths;

		mLevels=levels;
		reinit();
	}

	vector<float>* levelCenters()
	{
		return new vector<float>(mLevelCenters);
	}

	void levelCenters(const vector<float> &l)
	{
		mLevelCenters=l;
		reinit();
	}
	
	vector<float>* levelWidths()
	{
		return new vector<float>(mLevelWidths);
	}

	void levelWidths(const vector<float> &l)
	{
		mLevelWidths=l;
		reinit();
	}
	
	vector<float>* levels()
	{
		return new vector<float>(mLevels);
	}

	void levels(const vector<float> &l)
	{
		mLevels=l;
		reinit();
	}
	
	long bands()
	{
		return mLevels.size();
	}

	void bands(long b)
	{
		resize(mLevels, (int)b);
		resize(mLevelWidths, (int)b);
		resize(mLevelCenters, (int)b);
		reinit();
	}

	long enabled()
	{
		return (long)mEnabled;
	}

	void enabled(long enabled)
	{
		mEnabled=(bool)enabled;
	}

	float preamp()
	{
		return mPreamp;
	}

	void preamp(float a)
	{
		mPreamp=a;
	}
	
	void streamInit()
	{
	
	}
	
	void streamStart()
	{
	
	}

/* BandPassInit(&nfoLeft, 15000.0, 5000.0);
 * BandPassInit(&nfoLeft, 15000.0, 5000.0);
 */
	
	void calculateBlock(unsigned long samples)
	{
		// works by separating the bands
		// multiplying, then adding
		if (mEnabled && samples && &mLevels.front())
		{
			
			{ // preamp;

				float *left=inleft;
				float *right=inright;
				float *end=left+samples;

				float *oleft=outleft;
				float *oright=outright;

				while (left<end)
				{
					// see the _long_ comment in
					// kdemultimedia/arts/modules/synth_std_equalizer_impl.cc
					if (::fabs(*left) + ::fabs(*right) < 0.00000001)
						goto copy; // if you apologize, it's becomes ok
					*oleft=*left * mPreamp;
					*oright=*right * mPreamp;
					++left;
					++right;
					++oleft;
					++oright;
				}
			}
				
			BandPassInfo *leftBand=&mBandLeft.front();
			BandPassInfo *rightBand=&mBandRight.front();
			float *level=&mLevels.front();
			float *end=&mLevels.back();
			float intensity=1.0/(float)mLevels.size();

			if (mBufferLength != samples)
			{
				delete mBuffer;
				mBuffer = new float[samples];
				mBufferLength = samples;
			}
			
			register float *buffer=mBuffer;
			register float *bufferEnd=buffer+samples;
			while (level<end)
			{
				register float *buffIter, *outIter;
				float levelAndIntensity=*level * intensity;
				
				BandPass(leftBand, outleft, buffer, samples);
				for (buffIter=buffer, outIter=outleft; buffIter<bufferEnd; ++buffIter, ++outIter)
					*outIter+=(*buffIter) * levelAndIntensity;

				BandPass(rightBand, outright, buffer, samples);
				for (buffIter=buffer, outIter=outright; buffIter<bufferEnd; ++buffIter, ++outIter)
					*outIter+=(*buffIter) * levelAndIntensity;

				++level;
				++leftBand;
				++rightBand;
			}
		}
		else
		{
		copy:
			// ASM optimized, so much faster
			memcpy(outleft, inleft, samples*sizeof(float));
			memcpy(outright, inright, samples*sizeof(float));
		}

	}

	Equalizer_impl() : mEnabled(false)
	{
		mBuffer=0;
		mBufferLength=0;
	}
	
	~Equalizer_impl()
	{
		delete [] mBuffer;
	}

	// speed hack! assume that someone else will
	// suspend us
	AutoSuspendState autoSuspend() { return asSuspend; }
	
};






class EqualizerSSE_impl : public EqualizerSSE_skel, public StdSynthModule
{
	vector<float> mLevels;

	vector<BandPassInfo> mBandLeft, mBandRight;
	
	vector<float> mLevelWidths;
	vector<float> mLevelCenters;

	bool mEnabled;
	float mPreamp;
		


	void reinit()
	{
		mBandLeft.clear();
		mBandRight.clear();
		for (unsigned int i=0; i< mLevelWidths.size(); ++i)
		{
			BandPassInfo nfo;
			BandPassInit(&nfo, mLevelCenters[i], mLevelWidths[i]);
			mBandLeft.push_back(nfo);
			mBandRight.push_back(nfo);
		}
		
	}
	
public:
	void set(const std::vector<float>& levels, const std::vector<float>& centers, const std::vector<float>& widths)
	{
		mLevelCenters=centers;
		mLevelWidths=widths;

		mLevels=levels;
		reinit();
	}

	vector<float>* levelCenters()
	{
		return new vector<float>(mLevelCenters);
	}

	void levelCenters(const vector<float> &l)
	{
		mLevelCenters=l;
		reinit();
	}
	
	vector<float>* levelWidths()
	{
		return new vector<float>(mLevelWidths);
	}

	void levelWidths(const vector<float> &l)
	{
		mLevelWidths=l;
		reinit();
	}
	
	vector<float>* levels()
	{
		return new vector<float>(mLevels);
	}

	void levels(const vector<float> &l)
	{
		mLevels=l;
		reinit();
	}
	
	long bands()
	{
		return mLevels.size();
	}

	void bands(long b)
	{
		resize(mLevels, (int)b);
		resize(mLevelWidths, (int)b);
		resize(mLevelCenters, (int)b);
		reinit();
	}

	long enabled()
	{
		return (long)mEnabled;
	}

	void enabled(long enabled)
	{
		mEnabled=(bool)enabled;
	}

	float preamp()
	{
		return mPreamp;
	}

	void preamp(float a)
	{
		mPreamp=a;
	}
	
	void streamInit()
	{
	
	}
	
	void streamStart()
	{
	
	}

/* BandPassInit(&nfoLeft, 15000.0, 5000.0);
 * BandPassInit(&nfoLeft, 15000.0, 5000.0);
 */
	
	void calculateBlock(unsigned long samples)
	{
#ifdef __i386__
		// works by separating the bands
		// multiplying, then adding
		if (mEnabled && samples)
		{
			if (*inleft + *inright == 0.0)
				goto copy; // just shut up :)
			
			{ // preamp;

				float *left=inleft;
				float *right=inright;
				float *end=left+samples;

				float *oleft=outleft;
				float *oright=outright;

				while (left<end)
				{
					*oleft=*left * mPreamp;
					*oright=*right * mPreamp;
					++left;
					++right;
					++oleft;
					++oright;
				}
			}
				
			BandPassInfo *leftBand=&mBandLeft.front();
			BandPassInfo *rightBand=&mBandRight.front();
			float *level=&mLevels.front();
			float *end=&mLevels.back();
			float intensity=1.0/(float)mLevels.size();

			register float *buffer=new float[samples];
			register float *bufferEnd=buffer+samples;
			while (level<end)
			{
				register float *buffIter, *outIter;
				float levelAndIntensity=*level * intensity;
				
				BandPassSSE(leftBand, outleft, buffer, samples);
				for (buffIter=buffer, outIter=outleft; buffIter<bufferEnd; ++buffIter, ++outIter)
					*outIter+=(*buffIter) * levelAndIntensity;

				BandPassSSE(rightBand, outright, buffer, samples);
				for (buffIter=buffer, outIter=outright; buffIter<bufferEnd; ++buffIter, ++outIter)
					*outIter+=(*buffIter) * levelAndIntensity;

				++level;
				++leftBand;
				++rightBand;
			}
			delete [] buffer;
		}
		else
		{
		copy:
			// ASM optimized, so much faster
			memcpy(outleft, inleft, samples*sizeof(float));
			memcpy(outright, inright, samples*sizeof(float));
		}
#else
		(void)samples; // squelch warnings
#endif
	}

	EqualizerSSE_impl() : mEnabled(false)
	{
		
	}
	
	~EqualizerSSE_impl()
	{
	}

	// speed hack! assume that someone else will
	// suspend us
	AutoSuspendState autoSuspend() { return asSuspend; }
};


REGISTER_IMPLEMENTATION(Equalizer_impl);
REGISTER_IMPLEMENTATION(EqualizerSSE_impl);

}

#undef SAMPLES

