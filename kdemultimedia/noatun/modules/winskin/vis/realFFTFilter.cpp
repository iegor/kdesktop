/*
  a FFT filter
  Copyright (C) 1998  Martin Vogt;Philip VanBaren, 2 September 1993

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "realFFTFilter.h"

RealFFTFilter::RealFFTFilter(int fftPoints) {
  this->fftPoints = fftPoints;
  data=new short[fftPoints*4];  
  realFFT= new RealFFT(fftPoints*2);
}


RealFFTFilter::~RealFFTFilter() {
  delete data;
  delete realFFT;
}


int RealFFTFilter::getPoints() {
  return fftPoints;
}


short* RealFFTFilter::getPointPtr() {
  return data;
}


/*
  the array is expected to be a PCM stream (real data)
*/
int RealFFTFilter::fft16(float* left,float* right,int len) {
  int i;

  len=len/4;


  int mixTmp;

  // take care for no array overflows:
  int n=min(len,fftPoints);

  // copy things into fftArray.

  for (i = 0 ; i < n; i++) {
    mixTmp=(int) (16384.0*(left[i]+right[i]));

    if (mixTmp < SHRT_MIN) {
      data[i]= SHRT_MIN; 
    } else {
      if (mixTmp > SHRT_MAX) {
	data[i] = SHRT_MAX; 
      } else {
	data[i]=(short)mixTmp;
      }
    }
  }

  realFFT->fft(data);
  return true;
}



int* RealFFTFilter::getBitReversed() { 
  return realFFT->getBitReversed();
}



int RealFFTFilter::min(int x1,int x2) { 
  if (x1 < x2) {
    return x1;
  }
  return x2;
}


