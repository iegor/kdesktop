/*
   This file is part of KDE/aRts - xine integration
   Copyright (C) 2003 Ewald Snel <ewald@rambo.its.tudelft.nl>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
*/

#ifndef __VIDEOSCALER_H
#define __VIDEOSCALER_H

void scaleYuvToRgb32(  int width, int height,
		       unsigned char *base[3], unsigned int pitches[3],
		       int scaledWidth, int scaledHeight,
		       unsigned int *pixels, unsigned int bytesPerLine );

void scaleYuy2ToRgb32( int width, int height,
		       unsigned char *base, unsigned int pitch,
		       int scaledWidth, int scaledHeight,
		       unsigned int *pixels, unsigned int bytesPerLine );

#endif
