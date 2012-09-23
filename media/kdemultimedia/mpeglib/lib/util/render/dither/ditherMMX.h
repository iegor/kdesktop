/*
  mmx ditherer
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __DITHERMMX_H
#define __DITHERMMX_H

#include "ditherDef.h"


// The mmx dither routine come from NIST
// NIST is an mpeg2/dvd player
// more: http://home.germany.net/100-5083/
extern void  ditherBlock(unsigned char *lum, 
			 unsigned char *cr, 
			 unsigned char *cb,
			 unsigned char *out,
			 int rows, int cols, int mod);

extern void dither32_mmx(unsigned char* lum,
			 unsigned char* cr,
			 unsigned char* cb,
			 unsigned char* out,
			 int rows,
			 int cols,
			 int mod);


#endif
