/*
  a FFT filter
  Copyright (C) 1998  Martin Vogt;Philip VanBaren, 2 September 1993

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __REALFFTFILTER_H
#define __REALFFTFILTER_H



#include "realFFT.h"
#include <limits.h>



class RealFFTFilter {
  
  int fftPoints;
  RealFFT* realFFT;

  short* data;


 public:
  RealFFTFilter(int points);
  ~RealFFTFilter();
  int fft16(float* left,float* right,int len);

  int* getBitReversed();
  int getPoints();
  short* getPointPtr();

 private:
  int min(int x1,int x2);

};


#endif
