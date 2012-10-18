/***************************************************************************
                          rgb2yuv32.h  -  description
                             -------------------
    begin                : Tue Nov 2 2000
    copyright            : (C) 2000 by Christian Gerlach
    email                : cgerlach@rhrk.uni-kl.de
 ***************************************************************************/
 
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _RGB2YUV32_H_
#define _RGB2YUV32_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../yuvPicture.h"
#include "rgb2yuvdefs.h"

void rgb2yuv32(unsigned char* rgb, unsigned char* dest);

// slow C rountines
void rgb2yuv24bit(unsigned char* rgbSource,
		  unsigned char* lum,
		  unsigned char* cr,
		  unsigned char* cb,int height, int width);

void rgb2yuv32bit(unsigned char* rgbSource,
		  unsigned char* lum,
		  unsigned char* cr,
		  unsigned char* cb,int height, int width);




#ifndef INTEL
void rgb2yuv24bit_mmx(unsigned char* rgbSource,
		      unsigned char* lum,
		      unsigned char* cr,
		      unsigned char* cb,int height, int width);

void rgb2yuv32bit_mmx(unsigned char* rgbSource,
		      unsigned char* lum,
		      unsigned char* cr,
		      unsigned char* cb,int height, int width);
#endif


#ifdef INTEL

void rgb2yuv24bit_mmx(unsigned char* rgbSource,
		      unsigned char* lum,
		      unsigned char* cr,
		      unsigned char* cb,int height, int width);

void rgb2yuv32bit_mmx(unsigned char* rgbSource,
		      unsigned char* lum,
		      unsigned char* cr,
		      unsigned char* cb,int height, int width);


void rgb2yuv24bit_mmx444_row(unsigned char* rgb, 
			     unsigned char* lum, unsigned char* cr,
			     unsigned char* cb, int pixel);


void rgb2yuv24bit_mmx422_row(unsigned char* rgb, 
			     unsigned char* lum, unsigned char* cr,
			     unsigned char* cb, int pixel);

void rgb2yuv32bit_mmx422_row(unsigned char* rgb, 
			     unsigned char* lum, unsigned char* cr,
			     unsigned char* cb, int pixel);

void rgb2y24bit_mmx_row(unsigned char* rgbSource, 
			unsigned char* lum, int pixel);

void rgb2y32bit_mmx_row(unsigned char* rgbSource, 
			unsigned char* lum, int pixel); 

#endif
//INTEL


#endif
