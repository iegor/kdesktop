/*
  global definitions for dithering
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __DITHERDEF_H
#define __DITHERDEF_H


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern "C" {
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
}


#ifdef __GNUC__
#if (__GNUC__ < 2 || ( __GNUC__ == 2 && __GNUC_MINOR__ < 91 ) )
#ifndef _AIX
#warning "inline code disabled! (buggy egcs version)"
#undef __NO_MATH_INLINES
#define __NO_MATH_INLINES 1
#endif
#endif
#endif
#include <math.h>



/* Gamma correction stuff */
extern int gammaCorrectFlag;
extern double gammaCorrect;

/* Chroma correction stuff */
extern int chromaCorrectFlag;
extern double chromaCorrect;


#define CB_BASE 1
#define CR_BASE (CB_BASE*CB_RANGE)
#define LUM_BASE (CR_BASE*CR_RANGE)

#define TABTYPE short

#ifdef SIXTYFOUR_BIT
#define PIXVAL long
#else
#define PIXVAL int
#endif

#ifdef SIXTYFOUR_BIT
#define ONE_TWO 1
#else
#define ONE_TWO 2
#endif



#define Min(x,y) (((x) < (y)) ? (x) : (y))
#define Max(x,y) (((x) > (y)) ? (x) : (y))

#define CHROMA_CORRECTION128(x) ((x) >= 0 \
                        ? Min(127,  (int)(((x) * chromaCorrect))) \
                        : Max(-128, (int)(((x) * chromaCorrect))))
#define CHROMA_CORRECTION256D(x) ((x) >= 128 \
                        ? 128.0 + Min(127.0, (((x)-128.0) * chromaCorrect)) \
                        : 128.0 - Min(128.0, (((128.0-(x))* chromaCorrect))))



#define GAMMA_CORRECTION(x) ((int)(pow((x) / 255.0, 1.0/gammaCorrect)* 255.0))

#define CHROMA_CORRECTION128D(x) ((x) >= 0 \
                        ? Min(127.0,  ((x) * chromaCorrect)) \
                        : Max(-128.0, ((x) * chromaCorrect)))

#define CHROMA_CORRECTION256(x) ((x) >= 128 \
                        ? 128 + Min(127, (int)(((x)-128.0) * chromaCorrect)) \
                        : 128 - Min(128, (int)((128.0-(x)) * chromaCorrect)))

// Range values for lum, cr, cb. 
#define  LUM_RANGE  8
#define  CR_RANGE   4
#define  CB_RANGE   4

 
#endif
