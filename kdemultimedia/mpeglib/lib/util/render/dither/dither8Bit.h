/*
  dither 8 bit depth yuv images
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __DITHER_8BIT_H
#define __DITHER_8BIT_H


#include "colorTable8Bit.h"

#define DITH_SIZE 16


class Dither8Bit {

  /* Structures used to implement hybrid ordered dither/floyd-steinberg
     dither algorithm.
  */
  
  unsigned char *l_darrays[DITH_SIZE];
  unsigned char *cr_darrays[DITH_SIZE];
  unsigned char *cb_darrays[DITH_SIZE];

  // private colormap
  unsigned char pixel[256];

  ColorTable8Bit* colorTable8Bit;
 
  // Arrays holding quantized value ranged for lum, cr, and cb. 
  // (used for 8 Bit)
  
  int* lum_values;
  int* cr_values;
  int* cb_values;
  

 public:
  Dither8Bit(unsigned char pixel[256]);
  ~Dither8Bit();

  void  ditherImageOrdered (unsigned char* lum,
			    unsigned char* cr,
			    unsigned char* cb,
			    unsigned char* out,
			    int h,
			    int w);
  
 private:
  void initOrderedDither();
};

#endif

