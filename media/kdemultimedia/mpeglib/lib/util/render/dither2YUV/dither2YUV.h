/*
  this class dithery RGB picture to yuv12
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __DITHER2YUV_H
#define __DITHER2YUV_H

 
#include "../../mmx/mmx.h"
#include "../yuvPicture.h"


#include <stdlib.h>
#include "rgb2yuv16.h"
#include "rgb2yuv32.h"

#define _SIZE_NONE     0
#define _SIZE_NORMAL   1
#define _SIZE_DOUBLE   2


/**
   Wraps all calls to software ditherer and the different 
   resolutions,mmx enhancements, and doublesize ditherers.
*/


class Dither2YUV {

  int lmmx;

  int bpp;

  
 public:
  Dither2YUV();
  ~Dither2YUV();
  
  int getDitherSize();
  void setDitherSize(int ditherSize);

  void doDither(YUVPicture* pic,int depth,int ditherSize,
		unsigned char* dest,int offset);
  

 private:
  void doDitherRGB_NORMAL(YUVPicture* pic,int depth,int ditherSize,
			  unsigned char* dest,int offset);
  
  void doDither2YUV_std(YUVPicture* pic,int depth,
			unsigned char* dest,int offset);

};

#endif
