/*
 *	Copyright (C) 2002  Michael Zuercher
 *	                    mzuerche@iastate.edu
 *
 *	Based on an algorithm by Stephan M. Sprenger, http://www.dspdimension.com
 *
 *	All rights reserved.
 *
 *	Redistribution and use in source and binary forms, with or without modification,
 *	are permitted provided that the following conditions are met:
 *  
 *  1. Redistributions of source code must retain the above copyright notice, this list 
 *     of conditions and the following disclaimer. 
 *  
 *  2. Redistributions in binary form must reproduce the above copyright notice, this 
 *     list of conditions and the following disclaimer in the documentation and/or 
 *     other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
 *  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
 *  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 *  SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
 *  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
 *  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
 *  DAMAGE.
 */



#include "artsmodulessynth.h"
#include "stdsynthmodule.h"
#include <stdio.h> //debug only
#include <arts/fft.h>
#include <string.h>
#include <math.h>

#define MAX(a,b) (((a) > (b) ? (a) : (b)))
#define MIN(a,b) (((a) < (b) ? (a) : (b)))

using namespace Arts;

class Synth_PITCH_SHIFT_FFT_impl : virtual public Synth_PITCH_SHIFT_FFT_skel,
							   virtual public StdSynthModule
{
	private:
		struct fftBin
		{
			float magnitude;
			float frequency;
			float phase;
		};

		bool addPi;
		
		/* the attributes (gui changeable) */
		/* these can happen on the fly */
		float _scaleFactor, _speed;
		/* these require calling setStreamOpts() */
		unsigned int _frameSize, _oversample;
		
		/* the buffers */
		float *inBuffer, *outBuffer; /* circular buffers (float) */
		float *windowedData; /* windowed and unrolled buffer (float) */
		fftBin *analysisBuf, *synthesisBuf; /* workspaces (fftBin) */
		float *real, *imag; /* place for the FFT to output */
		float *windowCoeffient;
		float *scratch; /* used to store imag IFFT results that we don't need */
		float *phaseDiff;

		/* variables to keep us in the right place of the buffers */
		unsigned long bufferOffset;
		/* stream not yet ready to go until we have prerolled this many windows */
		unsigned int initStepsRemaining;
		
		/* some commonly used variables */
		unsigned long stepSize;
		double expectedPhaseDiff;
		double freqPerBin;

		/* Helper functions */
		void inWindow(float windowedData[], const float *inBuffer, const unsigned int basePopPoint);
		void analysis(fftBin analysisBuf[], const float real[]);
		void pitchScale(fftBin synthesisBuf[], const fftBin analysisBuf[]);
		void synthesis(float windowedData[], fftBin synthesisBuf[]);
		void outWindow(float *outBuffer, const unsigned int basePushPoint, const float windowedData[]);

		
	public:
		/* functions for the plugin interface */
		float speed() { return _speed; }
		void speed(float newSpeed) { _speed = newSpeed; }

		float scaleFactor() { return _scaleFactor; }
		void scaleFactor(float newScaleFactor) { _scaleFactor = newScaleFactor; }
			
		long frameSize() { return (long)_frameSize; }
		void frameSize(long newFrameSize) 
		{ 
			setStreamOpts(newFrameSize, _oversample); 
		}

		long oversample() { return (long)_oversample; }
		void oversample(long newOversample) 
		{
			setStreamOpts(_frameSize, newOversample); 
		}

		/* gets called by arts when it needs more data */
		void calculateBlock(unsigned long samples);
			
		void streamInit()
		{
			inBuffer = outBuffer = NULL;
			analysisBuf = synthesisBuf = NULL;
			real = imag = NULL;
			windowedData = NULL;
			windowCoeffient = NULL;
			scratch = NULL;
			phaseDiff = NULL;

			/* setup default stream parameters */
			_speed = 1.0;
			_scaleFactor = 0.9;
			setStreamOpts(4096,2);

			addPi = false;
		}

		void streamEnd()
		{
			/* clean up buffers */
			delete [] inBuffer;
			delete [] outBuffer;
			delete [] windowedData;
			delete [] analysisBuf;
			delete [] synthesisBuf;
			delete [] real;
			delete [] imag;
			delete [] windowCoeffient;
			delete [] scratch;
			delete [] phaseDiff;
		}

		void setStreamOpts(unsigned int frameSize, unsigned int oversample)
		{
			/* clear any buffers left around */
			delete [] inBuffer;
			delete [] outBuffer;
			delete [] windowedData;
			delete [] analysisBuf;
			delete [] synthesisBuf;
			delete [] real;
			delete [] imag;
			delete [] windowCoeffient;
			delete [] scratch;
			delete [] phaseDiff;
			
			_frameSize = frameSize;
			_oversample = oversample;

			/* create the buffers */
			inBuffer = new float[_frameSize];
			outBuffer = new float[_frameSize];
			windowedData = new float[_frameSize];
			analysisBuf = new fftBin[_frameSize];
			synthesisBuf = new fftBin[_frameSize];
			real = new float[_frameSize];
			imag = new float[_frameSize];
			windowCoeffient = new float[_frameSize];
			scratch = new float[_frameSize];
			phaseDiff = new float[_oversample];
			

			/* set up the windowing coeffients */
			for(unsigned int sample=0; sample < _frameSize; sample++)
			{
				windowCoeffient[sample] = -0.5*cos(2.0*M_PI*(double)sample/(double)_frameSize)+0.5;
			}
			
			/* we should start at the beginning of the buffers */
			bufferOffset = 0;

			/* stream not yet ready to go until we have prerolled this many windows */
			initStepsRemaining = _oversample;
			
			/* calculate some commonly used variables */
			stepSize = _frameSize / _oversample;
			expectedPhaseDiff = 2*M_PI*(double)stepSize/(double)_frameSize;
			freqPerBin = samplingRate/(double)_frameSize; 

			for(unsigned int bin=0; bin < _oversample; bin++)
			{
				phaseDiff[bin] = bin*expectedPhaseDiff;
			}

			memset(outBuffer, 0 ,stepSize * sizeof(float)); /* clear the first part of the output accumulator */
			memset(analysisBuf, 0 ,_frameSize * sizeof(fftBin)); 
			memset(synthesisBuf, 0 ,_frameSize * sizeof(fftBin)); 
		}
};

void Synth_PITCH_SHIFT_FFT_impl::calculateBlock(unsigned long samples)
{
	unsigned long samplesRemaining = samples;

	/* pointers to the arts streams */
	float *inData = inStream;
	float *outData = outStream;
	
	while(samplesRemaining > 0)
	{
		/* either fill the next window, or take all we have */
		int samplesThisPass = MIN(samplesRemaining,stepSize - (bufferOffset % stepSize));
		
		/* copy the incoming data into the buffer */
		memcpy(inBuffer + bufferOffset, inData, samplesThisPass * sizeof(float));
		/* set inData to data we haven't already taken */
		inData += samplesThisPass;

		if((bufferOffset+samplesThisPass) % stepSize)
		{
			/* if we still have only a partial window (input is still in the 
			 * middle of a window), we can't run it yet, but we have leftover
			 * output we can use */
		}
		else
		{
			/* round down the the nearest stepSize, and this window is full */

			if(initStepsRemaining > 0) /* we need to have enough old data for a full block too */
			{
				initStepsRemaining--; /* one less step to fill before we can start for real */
			}
			else
			{
				unsigned int stepOffset = (bufferOffset + samplesThisPass) - stepSize;
				/* now we have a complete block (not still filling at init) to add the 
				 * new complete window on to */

				/* ############################ prepare one stepSize ########################### */
				
				inWindow(windowedData,inBuffer,stepOffset);
				analysis(analysisBuf,windowedData);
				pitchScale(synthesisBuf,analysisBuf);
				synthesis(windowedData,synthesisBuf);
				outWindow(outBuffer,bufferOffset,windowedData);

				/* ############################################################################# */
			}
		}

		memcpy(outData, outBuffer + bufferOffset, samplesThisPass * sizeof(float));
		outData  += samplesThisPass;
		memset(outBuffer + bufferOffset, 0 ,samplesThisPass * sizeof(float)); /* clear the output space that we have used */
		bufferOffset += samplesThisPass;
		bufferOffset %= _frameSize; /* wrap if needed before the next frame starts */
		samplesRemaining -= samplesThisPass;
	}
}

void Synth_PITCH_SHIFT_FFT_impl::inWindow(float windowedData[], const float *inBuffer, const unsigned int basePopPoint)
{
	unsigned int sample;
	for(sample=0; sample < _frameSize-basePopPoint; sample++)
	{
		/* window the data and unroll the buffers */
		windowedData[sample] = inBuffer[basePopPoint + sample] * windowCoeffient[sample];
	}
	for(; sample < _frameSize; sample++)
	{
		/* window the data and unroll the buffers */
		windowedData[sample] = inBuffer[(basePopPoint + sample) - _frameSize] * windowCoeffient[sample];
	}
}

void Synth_PITCH_SHIFT_FFT_impl::analysis(fftBin analysisBuf[], const float windowedData[])
{
	float lastPhase;
	float phaseDrift;
	
	/* do forward FFT */
	/* const_cast because arts_fft_float is silly */
	arts_fft_float(_frameSize, 0, const_cast<float *>(windowedData), NULL, real, imag);
		
	/* the actual analysis loop */
	for(unsigned int bin=0; bin < _frameSize/2; bin++)
	{
		lastPhase = analysisBuf[bin].phase;
		
		/* compute magnitude and phase */
		analysisBuf[bin].magnitude = 2.0 * sqrt(real[bin]*real[bin] + imag[bin]*imag[bin]);
		analysisBuf[bin].phase = atan2(imag[bin],real[bin]);
		
		
		/* compute phase difference and subtract expected phase difference */
		phaseDrift = (analysisBuf[bin].phase - lastPhase) - float(phaseDiff[bin % _oversample]);
		
		/* we now need to map it into the +/- Pi interval */
		while(phaseDrift < -M_PI) 
			phaseDrift += 2*M_PI; 
		while(phaseDrift > M_PI) 
			phaseDrift -= 2*M_PI;
		
		/* compute true frequency */
		analysisBuf[bin].frequency = (bin + ((phaseDrift * _oversample) / (2*M_PI)))*freqPerBin;
		//analysisBuf[bin].frequency = (bin + (phaseDrift/(2*M_PI)))*freqPerBin;
	}
	
}

void Synth_PITCH_SHIFT_FFT_impl::pitchScale(fftBin synthesisBuf[], const fftBin analysisBuf[])
{
	unsigned int sourceBin;
	for(unsigned int destBin=0; destBin < _frameSize/2; destBin++)
	{
		sourceBin = (unsigned int)floor(destBin/_scaleFactor);
		if(sourceBin < _frameSize/2)
		{
			/* new bin overrides existing if magnitude is higher */ 
			//if(analysisBuf[sourceBin].magnitude > synthesisBuf[destBin].magnitude)
			//{
				synthesisBuf[destBin].magnitude = analysisBuf[sourceBin].magnitude;
				synthesisBuf[destBin].frequency = analysisBuf[sourceBin].frequency * _scaleFactor;
			//}
#if 0
			/* fill empty bins with nearest neighbor */
			if((synthesisBuf[destBin].frequency == 0.0) && (destBin > 0))
			{
				cerr << "Empty bins\n";
				synthesisBuf[destBin].frequency = synthesisBuf[destBin-1].frequency; 
				synthesisBuf[destBin].magnitude = synthesisBuf[destBin-1].magnitude; 
			}
#endif
		}
		else
		{
			synthesisBuf[destBin].magnitude = 0;
		}
	}
#if 0
	for(unsigned int destBin=0; destBin < _frameSize/2; destBin++)
	{
		synthesisBuf[destBin].magnitude = analysisBuf[destBin].magnitude;
		synthesisBuf[destBin].frequency = analysisBuf[destBin].frequency;
	}
#endif
}

void Synth_PITCH_SHIFT_FFT_impl::synthesis(float windowedData[], fftBin synthesisBuf[])
{
	double phaseDrift;

#if 0
	double test;
	if(addPi == true)
		test = -M_PI;
	else
		test = 0;
#endif
	
	for(unsigned int bin=0;bin < _frameSize/2; bin++)
	{
		/* deviation of this bin's phase from one exactly at the true bin frequency */
		//phaseDrift = (((synthesisBuf[bin].frequency - bin*freqPerBin)/ freqPerBin)*(2*M_PI))/_oversample;
		phaseDrift = (synthesisBuf[bin].frequency / freqPerBin - bin)*(2*M_PI)/_oversample;
		//phaseDrift = 0;
		
				
		/* calculate the real and imag data */
		real[bin] = synthesisBuf[bin].magnitude * cos(synthesisBuf[bin].phase);
		imag[bin] = synthesisBuf[bin].magnitude * sin(synthesisBuf[bin].phase);
		
		/* accumulate current phase for this wave */
		synthesisBuf[bin].phase += (phaseDrift + phaseDiff[bin % _oversample]);
		//synthesisBuf[bin].phase += (phaseDrift + phaseDiff[bin % _oversample] + test);
		
		/* keep it so that -M_PI < phase < M_PI */
		while(synthesisBuf[bin].phase > M_PI)
				synthesisBuf[bin].phase -= 2*M_PI;
		while(synthesisBuf[bin].phase <= -M_PI)
			synthesisBuf[bin].phase += 2*M_PI;


#if 0
		//this needs to happen so that that 'strongest wave' picking in pitchScale works
		//but this isn't really the right place to do it
		synthesisBuf[bin].magnitude = 0;
		synthesisBuf[bin].frequency = 0;
#endif
		
	}
	
	/* zero the conjugate numbers */
	for(unsigned int i = _frameSize/2; i < _frameSize; i++)
	{
		real[i] = 0.0;
		imag[i] = 0.0;
	}
	
#if 0
	if(addPi == false)
		addPi = true;
	else
		addPi = false;
#endif
	
	/* do the inverse transform */
	arts_fft_float(_frameSize, 1, real, imag, windowedData, scratch);
}

void Synth_PITCH_SHIFT_FFT_impl::outWindow(float *outBuffer, const unsigned int basePushPoint, const float windowedData[])
{
	unsigned int sample;
	
	for(sample=0; sample < _frameSize - basePushPoint; sample++)
	{
		/* window the data and accumulate it back into the circular buffer */
		outBuffer[sample+basePushPoint] += 2.0 * windowCoeffient[sample] * windowedData[sample]/(_oversample);
	}
	for(; sample < _frameSize; sample++)
	{
		/* window the data and accumulate it back into the circular buffer */
		outBuffer[(sample+basePushPoint) - _frameSize] += 2.0 * windowCoeffient[sample] * windowedData[sample]/(_oversample);
	}
}


REGISTER_IMPLEMENTATION(Synth_PITCH_SHIFT_FFT_impl);
