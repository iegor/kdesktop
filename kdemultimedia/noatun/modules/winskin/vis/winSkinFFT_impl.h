/*
  implementation for winskin fft
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __WINSKINFFT_IMPL_H
#define __WINSKINFFT_IMPL_H

#include <artsflow.h>
#include <stdsynthmodule.h>
#include "winskinvis.h"
#include "realFFTFilter.h"
#include "visQueue.h"


using namespace std;
using namespace Arts;

namespace Noatun {

class WinSkinFFT_impl : public WinSkinFFT_skel, public StdSynthModule {
  
 public:
  
  WinSkinFFT_impl();
  ~WinSkinFFT_impl();
  
  void streamInit();
  void streamStart();
  
  // in:  audio stream inleft,  inright;
  // out: audio stream outleft, outright;
  void calculateBlock(unsigned long samples);
  
  void bandResolution(float res);
  float bandResolution();
  vector<float> *scope();

 private:
  RealFFTFilter* realFFTFilter;
  int fftBands_short;
  int* fftArray;
  VISQueue* visQueue;
  int bands;

  int fragCnt;
  int writePos;
  int readPos;
};


}

#endif
