/*
  a FFT class
  Copyright (C) 1998  Martin Vogt;Philip VanBaren

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __REALFFT_H
#define __REALFFT_H

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/**
 <pre>
 *     Program: REALFFTF.C
 *      Author: Philip VanBaren
 *        Date: 2 September 1993
 *
 * Description: These routines perform an FFT on real data.
 *              On a 486/33 compiled using Borland C++ 3.1 with full
 *              speed optimization and a small memory model, a 1024 point 
 *              FFT takes about 16ms.
 *              This code is for floating point data.
 *
 *  Note: Output is BIT-REVERSED! so you must use the BitReversed to
 *        get legible output, (i.e. Real_i = buffer[ BitReversed[i] ]
 *                                  Imag_i = buffer[ BitReversed[i]+1 ] )
 *        Input is in normal order.
 </pre>
 */



class RealFFT {

  int* BitReversed;
  short* SinTable;
  int Points;

 public:
  RealFFT(int fftlen);
  ~RealFFT();

  void fft(short* buffer);
  int* getBitReversed();

 private:

  short *A,*B;
  short *sptr;
  short *endptr1,*endptr2;
  int *br1,*br2;
  long HRplus,HRminus,HIplus,HIminus;


};


#endif
