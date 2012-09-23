/*
  flips RGB images
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#ifndef __DITHERRGB_FLIPPED_H
#define __DITHERRGB_FLIPPED_H

#include "colorTableHighBit.h"

class DitherRGB_flipped {

  int flipSize;
  unsigned char* flipSpace;

 public:
  DitherRGB_flipped();
  ~DitherRGB_flipped();

  // Note: this methods swaps the image
  // itsself
  void flipRGBImage(unsigned char* dest,unsigned char* src,
		    int depth,int width,int height,int offset);

};

#endif
