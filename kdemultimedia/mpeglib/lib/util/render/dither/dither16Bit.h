/*
  dither 16 bit depth yuv images
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#ifndef __DITHER16Bit_H
#define __DITHER16Bit_H

#include "colorTableHighBit.h"

class Dither16Bit {

  ColorTableHighBit* colorTableHighBit;

  TABTYPE *L_tab;
  TABTYPE *Cr_r_tab;
  TABTYPE *Cr_g_tab;
  TABTYPE *Cb_g_tab;
  TABTYPE *Cb_b_tab;

  PIXVAL *r_2_pix;
  PIXVAL *g_2_pix;
  PIXVAL *b_2_pix;

 public:
  Dither16Bit(unsigned int redMask,
	      unsigned int greenMask,unsigned int blueMask);
  ~Dither16Bit();

  void ditherImageColor16(unsigned char* lum, 
			  unsigned char* cr, 
			  unsigned char* cb,
			  unsigned char* out,
			  int rows,
			  int cols,
			  int offset);

  void ditherImageTwox2Color16(unsigned char* lum,
			       unsigned char* cr,
			       unsigned char* cb,
			       unsigned char* out,
			       int rows,
			       int cols,
			       int mod);

};

#endif
