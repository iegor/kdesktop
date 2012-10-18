/*
   This file is part of KDE/aRts - xine integration
   Copyright (C) 2003 Ewald Snel <ewald@rambo.its.tudelft.nl>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   inspired by code from the xine project

   Copyright (C) 2003 the xine project
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <config.h>

#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif

#include "videoscaler.h"

#define THUMBNAIL_BRIGHTNESS	+32
#define THUMBNAIL_CONTRAST	128
#define THUMBNAIL_SATURATION	128


// Colorspace conversion tables
static int tableLY[256];
static int tableRV[256], tableBU[256], tableGU[256], tableGV[256];
static int clipR[2240], clipG[2240], clipB[2240];

static pthread_once_t once_control = PTHREAD_ONCE_INIT;


static void init_once_routine()
{
    int cly = ( 76309 * THUMBNAIL_CONTRAST   + 64) / 128;
    int crv = (104597 * THUMBNAIL_SATURATION + 64) / 128;
    int cbu = (132201 * THUMBNAIL_SATURATION + 64) / 128;
    int cgu = ( 25675 * THUMBNAIL_SATURATION + 64) / 128;
    int cgv = ( 53279 * THUMBNAIL_SATURATION + 64) / 128;
    int i;

    for (i=0; i < 256; i++)
    {
	tableLY[i] = cly * (i + THUMBNAIL_BRIGHTNESS - 16) + (864 << 16) + 32768;
	tableRV[i] = crv * (i - 128);
	tableBU[i] = cbu * (i - 128);
	tableGU[i] = cgu * (i - 128);
	tableGV[i] = cgv * (i - 128);
    }
    for (i=0; i < 2240; i++)
    {
	int c = (i < 864) ? 0 : ((i > 1119) ? 255 : (i - 864));

	clipR[i] = c << 16;
	clipG[i] = c << 8;
	clipB[i] = c;
    }
}

static void yuvToRgb32( unsigned char *bufy, unsigned char *bufu, unsigned char *bufv,
			unsigned int *pixels, int width )
{
    for (int i=0; i < width; i++)
    {
	int l = tableLY[bufy[i]];

	pixels[i] = clipR[(l + tableRV[bufv[i]]) >> 16] |
		    clipG[(l - tableGU[bufu[i]] - tableGV[bufv[i]]) >> 16] |
		    clipB[(l + tableBU[bufu[i]]) >> 16];
    }
}

static inline void scaleLine( unsigned char *src[2], int width,
			      unsigned char *dst, int scaledWidth,
			      int scale, int weight, int step, int offset )
{
    int a, b, c, d;
    int x = (scale / 2) - 32768;
    unsigned char *p0 = (src[0] + offset);
    unsigned char *p1 = (src[1] + offset);

    weight >>= 8;

    if (scaledWidth > width)
    {
	/* Trailing pixels, no horizontal filtering */
	c  = scaledWidth - (((width << 16) - 32768 - (scale / 2)) / scale);
	a  = p0[(step * width) - step];
	b  = p1[(step * width) - step];
	a += (128 + (b - a) * weight) >> 8;
	scaledWidth -= c;
	memset( &dst[scaledWidth], a, c );

	/* Leading pixels, no horizontal filtering */
	c  = (32767 + (scale / 2)) / scale;
	a  = p0[0];
	b  = p1[0];
	a += (128 + (b - a) * weight) >> 8;
	scaledWidth -= c;
	memset( dst, a, c );

	/* Adjust source and destination */
	dst += c;
	x += (c * scale);
    }

    for (int i=0; i < scaledWidth; i++)
    {
	int xhi = (step == 1) ? (x >> 16)
			      : ((step == 2) ? (x >> 15) & ~0x1
					     : (x >> 14) & ~0x3);
	int xlo = (x & 0xFFFF) >> 8;

	/* Four nearest points for bilinear filtering */
	a = p0[xhi];
	b = p0[xhi + step];
	c = p1[xhi];\
	d = p1[xhi + step];

	/* Interpolate horizontally */
	a = (256 * a) + (b - a) * xlo;
	c = (256 * c) + (d - c) * xlo;

	/* Interpolate vertically and store bilinear filtered sample */
	*(dst++) = ((256 * a) + (c - a) * weight + 32768) >> 16;

	x += scale;
    }
}

void scaleYuvToRgb32( int width, int height,
		      unsigned char *base[3], unsigned int pitches[3],
		      int scaledWidth, int scaledHeight,
		      unsigned int *pixels, unsigned int bytesPerLine )
{
    int chromaWidth = (width + 1) / 2;
    int chromaHeight = (height + 1) / 2;
    int scaleX = (width << 16) / scaledWidth;
    int scaleY = (height << 16) / scaledHeight;
    int scaleCX = (scaleX / 2);
    int y = (scaleY / 2) - 32768;

    // Temporary line buffers (stack)
    unsigned char *bufy = (unsigned char *)alloca( scaledWidth );
    unsigned char *bufu = (unsigned char *)alloca( scaledWidth );
    unsigned char *bufv = (unsigned char *)alloca( scaledWidth );

    pthread_once( &once_control, init_once_routine );

    for (int i=0; i < scaledHeight; i++)
    {
	unsigned char *twoy[2], *twou[2], *twov[2];
	int y2 = (y / 2) - 32768;

	// Calculate luminance scanlines for bilinear filtered scaling
	if (y < 0)
	{
	    twoy[0] = twoy[1] = base[0];
	}
	else if (y >= ((height - 1) << 16))
	{
	    twoy[0] = twoy[1] = base[0] + (height - 1) * pitches[0];
	}
	else
	{
	    twoy[0] = base[0] + (y >> 16) * pitches[0];
	    twoy[1] = twoy[0] + pitches[0];
	}

	// Calculate chrominance scanlines for bilinear filtered scaling
	if (y2 < 0)
	{
	    twou[0] = twou[1] = base[1];
	    twov[0] = twov[1] = base[2];
	}
	else if (y2 >= ((chromaHeight - 1) << 16))
	{
	    twou[0] = twou[1] = base[1] + (chromaHeight - 1) * pitches[1];
	    twov[0] = twov[1] = base[2] + (chromaHeight - 1) * pitches[2];
	}
	else
	{
	    twou[0] = base[1] + (y2 >> 16) * pitches[1];
	    twov[0] = base[2] + (y2 >> 16) * pitches[2];
	    twou[1] = twou[0] + pitches[1];
	    twov[1] = twov[0] + pitches[2];
	}

	// Bilinear filtered scaling
	scaleLine( twoy, width, bufy, scaledWidth, scaleX, (y & 0xFFFF), 1, 0 );
	scaleLine( twou, chromaWidth, bufu, scaledWidth, scaleCX, (y2 & 0xFFFF), 1, 0 );
	scaleLine( twov, chromaWidth, bufv, scaledWidth, scaleCX, (y2 & 0xFFFF), 1, 0 );

	// YUV to RGB32 comnversion
	yuvToRgb32( bufy, bufu, bufv, pixels, scaledWidth );

	pixels = (unsigned int *)(((char *)pixels) + bytesPerLine);
	y += scaleY;
    }
}

void scaleYuy2ToRgb32( int width, int height,
                       unsigned char *base, unsigned int pitch,
                       int scaledWidth, int scaledHeight,
                       unsigned int *pixels, unsigned int bytesPerLine )
{
    int chromaWidth = (width + 1) / 2;
    int scaleX = (width << 16) / scaledWidth;
    int scaleY = (height << 16) / scaledHeight;
    int scaleCX = (scaleX / 2);
    int y = (scaleY / 2) - 32768;

    // Temporary line buffers (stack)
    unsigned char *bufy = (unsigned char *)alloca( scaledWidth );
    unsigned char *bufu = (unsigned char *)alloca( scaledWidth );
    unsigned char *bufv = (unsigned char *)alloca( scaledWidth );

    pthread_once( &once_control, init_once_routine );

    for (int i=0; i < scaledHeight; i++)
    {
	unsigned char *two[2];

	// Calculate scanlines for bilinear filtered scaling
	if (y < 0)
	{
	    two[0] = two[1] = base;
	}
	else if (y >= ((height - 1) << 16))
	{
	    two[0] = two[1] = base + (height - 1) * pitch;
	}
	else
	{
	    two[0] = base + (y >> 16) * pitch;
	    two[1] = two[0] + pitch;
	}

	// Bilinear filtered scaling
	scaleLine( two, width, bufy, scaledWidth, scaleX, (y & 0xFFFF), 2, 0 );
	scaleLine( two, chromaWidth, bufu, scaledWidth, scaleCX, (y & 0xFFFF), 4, 1 );
	scaleLine( two, chromaWidth, bufv, scaledWidth, scaleCX, (y & 0xFFFF), 4, 3 );

	// YUV to RGB32 comnversion
	yuvToRgb32( bufy, bufu, bufv, pixels, scaledWidth );

	pixels = (unsigned int *)(((char *)pixels) + bytesPerLine);
	y += scaleY;
    }
}
