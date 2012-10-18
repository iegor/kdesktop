/*
Copyright (C) 2000 Stefan Westerfeld <stefan@space.twc.de>
              2000 Charles Samuels   <charles@kde.org>

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
#include <cmath>
#include <cstring>

#include <iostream>

using namespace std;
using namespace Arts;

namespace Noatun
{

#define SAMPLES 4096

static void doFft(float combine, float *inBuffer, vector<float> &scope)
{
	float out_real[SAMPLES],out_img[SAMPLES];
	fft_float(SAMPLES,0,inBuffer,0,out_real,out_img);

	scope.clear();

	int previous=0;
	int index=20;

	while (previous < 2048 && index < 2048)
	{
		int end = int(std::exp(double(index)*combine));
		float xrange = 0.0;

		while (previous < end)
		{
			xrange += (std::fabs(out_img[previous]) + std::fabs(out_real[previous]));
			previous++;
		}

		xrange /= float(SAMPLES);
		scope.push_back(xrange);

		index++;
	}
}


class FFTScopeStereo_impl : public FFTScopeStereo_skel, public StdSynthModule
{
protected:
	vector<float> mScopeLeft;
	vector<float> mScopeRight;

	float mCombine;

	float *mWindow;

	float *mInBufferLeft, *mInBufferRight;
	unsigned long mInBufferPos;

public:
	void bandResolution(float res) { mCombine=res; }
	float bandResolution() { return mCombine; }
	void streamInit()
	{
		unsigned long i;
		for(i=0;i<SAMPLES;i++)
		{
			float x = (float)i/(float)SAMPLES;
			// double it, since we're at half intensity without
			// adding both channels
			mWindow[i] = sin(x*DDC_PI)*sin(x*DDC_PI)*2;

			mInBufferLeft[i] = 0;
			mInBufferRight[i] = 0;
		}
		doFft(mCombine, mInBufferLeft, mScopeLeft);
		doFft(mCombine, mInBufferRight, mScopeRight);
	}
	void streamStart()
	{
		mInBufferPos = 0;
	}
	vector<float> *scopeLeft()
	{
		return new vector<float>(mScopeLeft);
	}
	vector<float> *scopeRight()
	{
		return new vector<float>(mScopeRight);
	}
	/*
	  in audio stream inleft, inright;
	  out audio stream outleft, outright;
	*/
	void calculateBlock(unsigned long samples)
	{
		unsigned long i;
		for(i=0;i<samples;i++)
		{
			mInBufferLeft[mInBufferPos] = inleft[i]*mWindow[mInBufferPos];
			mInBufferRight[mInBufferPos] = inright[i]*mWindow[mInBufferPos];
			if(++mInBufferPos == SAMPLES)
			{
				doFft(mCombine, mInBufferLeft, mScopeLeft);
				doFft(mCombine, mInBufferRight, mScopeRight);
				mInBufferPos = 0;
			}
			/*
			   monitoring only tasks can't be done with that StereoEffect
			   interface nicely - copy input to output until there is
			   something better
			 */
			outleft[i] = inleft[i];
			outright[i] = inright[i];
		}
	}

	FFTScopeStereo_impl() : mCombine(0.152492)
	{
		mWindow = new float[SAMPLES];
		mInBufferLeft = new float[SAMPLES];
		mInBufferRight = new float[SAMPLES];

	}
	~FFTScopeStereo_impl()
	{
		delete [] mWindow;
		delete [] mInBufferLeft;
		delete [] mInBufferRight;
	}

	AutoSuspendState autoSuspend() { return asSuspend; }

};

class FFTScope_impl : public FFTScope_skel, public StdSynthModule
{
protected:
	vector<float> mScope;

	float mCombine;

	float *mWindow;
	float *mInBuffer;
	unsigned long mInBufferPos;

public:
	void bandResolution(float res) { mCombine=res; }
	float bandResolution() { return mCombine; }
	void streamInit()
	{
		unsigned long i;
		for(i=0;i<SAMPLES;i++)
		{
			float x = (float)i/(float)SAMPLES;
			mWindow[i] = sin(x*DDC_PI)*sin(x*DDC_PI);
			mInBuffer[i] = 0;
		}
		doFft(mCombine, mInBuffer, mScope); // initialize so that we never return an empty scope
	}
	void streamStart()
	{
		mInBufferPos = 0;
	}
	vector<float> *scope()
	{
		return new vector<float>(mScope);
	}

	/*
	  in audio stream inleft, inright;
	  out audio stream outleft, outright;
	*/
	void calculateBlock(unsigned long samples)
	{
		unsigned long i;

		float *inBufferIt=mInBuffer+mInBufferPos;
		float *inleftIt=inleft;
		float *inrightIt=inright;
		float *windowIt=mWindow+mInBufferPos;

		for(i=0;i<samples;i++)
		{
			*inBufferIt = (*inleftIt + *inrightIt)* (*windowIt);
			if(++mInBufferPos == SAMPLES)
			{
				doFft(mCombine, mInBuffer, mScope);
				mInBufferPos = 0;
				inBufferIt=mInBuffer;
			}
			inBufferIt++;
			inleftIt++;
			inrightIt++;
			windowIt++;
		}
		/*
		   monitoring only tasks can't be done with that StereoEffect
		   interface nicely - copy input to output until there is
		   something better
		 */
		memcpy(outleft, inleft, sizeof(float)*samples);
		memcpy(outright, inright, sizeof(float)*samples);

	}

	FFTScope_impl() : mCombine(0.152492)
	{
		mWindow = new float[SAMPLES];
		mInBuffer = new float[SAMPLES];
	}

	~FFTScope_impl()
	{
		delete [] mWindow;
		delete [] mInBuffer;
	}

	AutoSuspendState autoSuspend() { return asSuspend; }
};

class RawScope_impl : public RawScope_skel, public StdSynthModule
{
protected:
	float *mScope;

	int mScopeLength;
	float *mScopeEnd;
	float *mCurrent;

public:
	vector<float> *scope()
	{
		vector<float> *buf = new vector<float>;
		buf->resize(mScopeLength);
		char *front = (char *)(&buf->front());
		memcpy(front, mCurrent, (mScopeEnd - mCurrent) * sizeof(float));
		memcpy(front + (mScopeEnd - mCurrent)*sizeof(float), mScope,
		                        (mCurrent - mScope) * sizeof(float));
		return buf;
	}

	void buffer(long len)
	{
		delete [] mScope;

		mScopeLength=len;
		mScope=new float[len];
		mScopeEnd=mScope+mScopeLength;
		mCurrent=mScope;

		memset(mScope, 0, mScopeLength);
	}

	long buffer()
	{
		return mScopeLength;
	}

	void calculateBlock(unsigned long samples)
	{
		for (unsigned long i=0; i<samples; ++i)
		{
			for (; mCurrent<mScopeEnd && i<samples; ++mCurrent, ++i)
			{
				*mCurrent = inleft[i] + inright[i];
			}
			if (mCurrent>=mScopeEnd)
				mCurrent=mScope;
		}

		memcpy(outleft, inleft, sizeof(float)*samples);
		memcpy(outright, inright, sizeof(float)*samples);

	}

	RawScope_impl()
	{
		mScope=0;
		buffer(512);

	}

	~RawScope_impl()
	{
		delete [] mScope;
	}

	AutoSuspendState autoSuspend() { return asSuspend; }
};

class RawScopeStereo_impl : public RawScopeStereo_skel, public StdSynthModule
{
protected:
	int mScopeLength;

	float *mScopeLeft;
	float *mScopeEndLeft;
	float *mCurrentLeft;

	float *mScopeRight;
	float *mScopeEndRight;
	float *mCurrentRight;

public:
	vector<float> *scopeLeft()
	{
		vector<float> *buf = new vector<float>;
		buf->resize(mScopeLength);
		char *front = (char *)(&buf->front());
		memcpy(front, mCurrentLeft, (mScopeEndLeft - mCurrentLeft) * sizeof(float));
		memcpy(front + (mScopeEndLeft - mCurrentLeft)*sizeof(float), mScopeLeft,
		       (mCurrentLeft - mScopeLeft) * sizeof(float));
		return buf;
	}

	vector<float> *scopeRight()
	{
		vector<float> *buf = new vector<float>;
		buf->resize(mScopeLength);
		char *front = (char *)(&buf->front());
		memcpy(front, mCurrentRight, (mScopeEndRight - mCurrentRight) * sizeof(float));
		memcpy(front + (mScopeEndRight - mCurrentRight)*sizeof(float), mScopeRight,
		       (mCurrentRight - mScopeRight) * sizeof(float));
		return buf;
	}

	void buffer(long len)
	{
		delete [] mScopeRight;
		delete [] mScopeLeft;

		mScopeLength=len;
		mScopeRight=new float[len];
		mScopeLeft=new float[len];
		mScopeEndRight=mScopeRight+mScopeLength;
		mScopeEndLeft=mScopeLeft+mScopeLength;
		mCurrentRight=mScopeRight;
		mCurrentLeft=mScopeLeft;

		memset(mScopeRight, 0, mScopeLength);
		memset(mScopeLeft, 0, mScopeLength);
	}

	long buffer()
	{
		return mScopeLength;
	}

	void calculateBlock(unsigned long samples)
	{
		for (unsigned long i=0; i<samples; ++i)
		{
			for (; mCurrentLeft<mScopeEndLeft && i<samples; ++mCurrentLeft, ++i)
			{
				*mCurrentLeft = inleft[i];
			}
			if (mCurrentLeft>=mScopeEndLeft)
				mCurrentLeft=mScopeLeft;
		}

		for (unsigned long i=0; i<samples; ++i)
		{
			for (; mCurrentRight<mScopeEndRight && i<samples; ++mCurrentRight, ++i)
			{
				*mCurrentRight = inright[i];
			}
			if (mCurrentRight>=mScopeEndRight)
				mCurrentRight=mScopeRight;
		}

		memcpy(outleft, inleft, sizeof(float)*samples);
		memcpy(outright, inright, sizeof(float)*samples);
	}

	RawScopeStereo_impl()
	{
		mScopeLeft=mScopeRight=0;
		buffer(512);

	}

	~RawScopeStereo_impl()
	{
		delete [] mScopeRight;
		delete [] mScopeLeft;
	}

	AutoSuspendState autoSuspend() { return asSuspend; }
};



REGISTER_IMPLEMENTATION(FFTScope_impl);
REGISTER_IMPLEMENTATION(FFTScopeStereo_impl);
REGISTER_IMPLEMENTATION(RawScope_impl);
REGISTER_IMPLEMENTATION(RawScopeStereo_impl);

}

#undef SAMPLES
