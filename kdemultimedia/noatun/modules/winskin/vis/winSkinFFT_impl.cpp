/*
  implementation for winskin fft
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "winSkinFFT_impl.h"
#include <audiosubsys.h>
#include <cstring>

#define __BANDS  75

namespace Noatun {

WinSkinFFT_impl::WinSkinFFT_impl() {
  fftBands_short=256;
  realFFTFilter= new RealFFTFilter(fftBands_short);
  fftArray=new int[fftBands_short];
  bands=0;

  fragCnt=(int)(AudioSubSystem::the()->fragmentCount());
  visQueue=new VISQueue(fragCnt);
  writePos=0;

}

WinSkinFFT_impl::~WinSkinFFT_impl(){
  delete realFFTFilter;
  delete fftArray;
  delete visQueue;
}

void WinSkinFFT_impl::streamInit() {
}


void WinSkinFFT_impl::streamStart() {
}


void WinSkinFFT_impl::calculateBlock(unsigned long samples) {


  unsigned long i;

  // monitoring only tasks can't be done with that StereoEffect
  // interface nicely - copy input to output until there is
  // something better
  // (when?)
  int n=sizeof(float)*samples;
  memcpy(outleft,inleft,n);
  memcpy(outright,inright,n);


  if (realFFTFilter->fft16(inleft,inright,samples) == false) {
    return;
  }


  
  //
  // The following modifications have nothing to do
  // with an fft, they only make the output look nice.
  // (mostly scaling)

  short* fftPtr;
  int* bitReversed;

  fftPtr=realFFTFilter->getPointPtr();
  bitReversed=realFFTFilter->getBitReversed();

  int pos=0;
  int step=realFFTFilter->getPoints()/__BANDS;


  int re;
  int im;
  int tmp;

  float max=0.0;
  float avg=0.0;



  for (i=0;i<__BANDS;i++) {
    re=(int)fftPtr[bitReversed[pos]];
    im=(int)fftPtr[bitReversed[pos]+1];
    
    tmp=re*re+im*im;  
    // Here I check a new idea. We remove all low values
    // and all values over xyz to xyz.
    fftArray[pos]=(int)(::sqrt(::sqrt(tmp)));
    
    if (fftArray[pos]<=15) {
      max+=fftArray[pos];
    } else {
      max+=15+fftArray[pos]/2;
    }
    pos=pos+step;
  }
  avg=0.65*max/(float)__BANDS;
  
  pos=0;
  vector<float>* visAnalyserArray=visQueue->getElement(writePos);
  visAnalyserArray->clear();
  visAnalyserArray->reserve(__BANDS);
  for (i=0;i<__BANDS;i++) {
    float val=(float)(fftArray[pos]-avg);
    visAnalyserArray->push_back(val);
    pos=pos+step;
  }
  writePos++;
  if (writePos >= fragCnt) writePos=0;

}


void WinSkinFFT_impl::bandResolution(float res) {
  bands=(int)res;
}

float WinSkinFFT_impl::bandResolution() {
  return (float)bands;
}


vector<float>* WinSkinFFT_impl::scope() {
  int delay=writePos+1;
  if (delay >= fragCnt) delay=0;
 

  vector<float>* visAnalyserArray=visQueue->getElement(delay);
    
  return new vector<float>(*visAnalyserArray);
}


REGISTER_IMPLEMENTATION(WinSkinFFT_impl);
  
}
