/***************************************************************************
                          rgb2yuv16.h  -  description
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

#ifndef __RGB2YUV16_H
#define __RGB2YUV16_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../yuvPicture.h"
#include "rgb2yuvdefs.h"

// slow C implementation
void rgb2yuv16bit(unsigned char* rgbSource,
		  unsigned char* destLum,
		  unsigned char* destCr,
		  unsigned char* destCb,int height, int width);



//
// We compile with MMX if we are on INTEL arch
// (this does not mean that we really support MMX, 
// this is a seperate/runtime check)
//

#ifdef INTEL

void rgb2yuv16bit_mmx(unsigned char* rgbSource,
		      unsigned char* lum,
		      unsigned char* cr,
		      unsigned char* cb,int height, int width);

void rgb2yuv16bit_mmx_fast(unsigned char* rgbSource,
		      unsigned char* lum,
		      unsigned char* cr,
		      unsigned char* cb,int height, int width);

void rgb2yuv16bit_mmx422_row(unsigned char* rgb, 
			     unsigned char* lum, unsigned char* cr,
			     unsigned char* cb, int pixel);

void rgb2y16bit_mmx_row(unsigned char* rgbSource, 
			unsigned char* lum, int pixel);

void rgb2yuv16bit_mmx422_row_fast(unsigned char* rgb, 
				  unsigned char* lum, unsigned char* cr,
				  unsigned char* cb, int pixel);

void rgb2y16bit_mmx_row_fast(unsigned char* rgb, 
			     unsigned char* lum, int pixel);


#endif
// INTEL



#endif
