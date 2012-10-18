/*
  copys RGB images to a destination
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#ifndef __DITHERRGB_H
#define __DITHERRGB_H

#include "colorTableHighBit.h"

class DitherRGB {

  int flipSize;
  unsigned char* flipSpace;

 public:
  DitherRGB();
  ~DitherRGB();

  // Note: this methods swaps the image
  // itsself
  void ditherRGBImage(unsigned char* dest,unsigned char* src,
		      int depth,int width,int height,int offset);
  void ditherRGBImage_x2(unsigned char* dest,unsigned char* src,
			 int depth,int width,int height,int offset);
 private:
  int getDepth(int pixel);
  // depth is here in byte!
  void ditherRGB1Byte_x2(unsigned char* dest,unsigned char* src,
			 int depth,int width,int height,int offset);
  void ditherRGB2Byte_x2(unsigned char* dest,unsigned char* src,
			 int depth,int width,int height,int offset);
  void ditherRGB4Byte_x2(unsigned char* dest,unsigned char* src,
			 int depth,int width,int height,int offset);
 
};

#endif
