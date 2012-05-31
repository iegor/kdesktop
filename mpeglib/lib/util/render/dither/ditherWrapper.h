/*
  wrapper for X11 Window
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __DITHERWRAPPER_H
#define __DITHERWRAPPER_H

 
#include "../../mmx/mmx.h"

#include "../yuvPicture.h"
#include "../imageBase.h"
#include <stdlib.h>
#include "ditherMMX.h"
#include "dither8Bit.h"
#include "dither16Bit.h"
#include "dither32Bit.h"
#include "ditherRGB_flipped.h"
#include "ditherRGB.h"


/**
   Wraps all calls to software ditherer and the different 
   resolutions,mmx enhancements, and doublesize ditherers.
*/


class DitherWrapper {

  int lmmx;

  int bpp;
  // colorMask
  unsigned int redMask;
  unsigned int greenMask;
  unsigned int blueMask;

  Dither8Bit* dither8Bit;
  Dither16Bit* dither16Bit;
  Dither32Bit* dither32Bit;
  DitherRGB_flipped* ditherRGB_flipped;
  DitherRGB* ditherRGB;
  
 public:
  DitherWrapper(int bpp,unsigned int redMask,
		unsigned int greenMask,unsigned int blueMask,
		unsigned char pixel[256]);
  ~DitherWrapper();
  
/*    int getDitherSize(); */
/*    void setDitherSize(int ditherMode); */

  void doDither(YUVPicture* pic,int depth,int imageMode,
		unsigned char* dest,int offset);
  

 private:
  void doDitherYUV(YUVPicture* pic,int depth,int imageMode,
		   unsigned char* dest,int offset);
  void doDitherRGB(YUVPicture* pic,int depth,int imageMode,
		   unsigned char* dest,int offset);
  void doDitherRGB_NORMAL(YUVPicture* pic,int depth,int imageMode,
			  unsigned char* dest,int offset);
  void doDitherRGB_FLIPPED(YUVPicture* pic,int depth,int imageMode,
			   unsigned char* dest,int offset);
     
  void doDither_std(YUVPicture* pic,int depth,unsigned char* dest,int offset);
  void doDither_x2(YUVPicture* pic,int depth,unsigned char* dest,int offset);
};

#endif
