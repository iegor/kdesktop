/***************************************************************************
                          rgb2yuv16.c  -  description
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

#include "rgb2yuv16.h"
#include <iostream>

static unsigned short KEEPR[4] = { 63488, 63488, 63488, 63488 };
unsigned short KEEPG[4] = {  2016,  2016,  2016,  2016 };
unsigned short KEEPB[4] = {    31,    31,    31,    31 };

short Y_RED[4]   = {  307,  307,  307,  307 };
short Y_GREEN[4] = {  302,  302,  302,  302 };
short Y_BLUE[4]  = {  117,  117,  117,  117 };

short U_RED[4]   = { -150, -150, -150, -150 };
short U_GREEN[4] = { -147, -147, -147, -147 };
short U_BLUE[4]  = {  444,  444,  444,  444 };

short V_RED[4]   = {  632,  632,  632,  632 };
short V_GREEN[4] = { -265, -265, -265, -265 };
short V_BLUE[4]  = { -102, -102, -102, -102 };


// how to avoid these nasty compiler warinings?
// heres one (maybe bad) method
void dummyRGB2YUV16Bit() {
  
  printf("%p\n",KEEPR);
  printf("%p\n",KEEPG);
  printf("%p\n",KEEPB);
  printf("%p\n",Y_RED);
  printf("%p\n",Y_GREEN);
  printf("%p\n",Y_BLUE);
  printf("%p\n",U_RED);
  printf("%p\n",U_GREEN);
  printf("%p\n",U_BLUE);
  printf("%p\n",V_RED);
  printf("%p\n",V_GREEN);
  printf("%p\n",V_BLUE);
}
  

#ifndef INTEL   
void rgb2yuv16bit_mmx(unsigned char* ,unsigned char* ,unsigned char* ,
		      unsigned char* ,int , int ) {
  std::cout << "RGB->YUV not compiled with INTEL" << std::endl;
  exit(0);
}

void rgb2yuv16bit_mmx_fast(unsigned char* ,unsigned char* ,unsigned char* ,
		      unsigned char* ,int , int ) {
  std::cout << "RGB->YUV not compiled with INTEL" << std::endl;
  exit(0);
}
#endif


void rgb2yuv16(unsigned char* rgbSource, unsigned char* dest)
{
	int rgb = *((unsigned short*) rgbSource++ );
	int r = RED(rgb);
	int g = GREEN(rgb);
	int b = BLUE(rgb);
	
	dest[0] = Y_RGB(r, g, b);
	dest[1] = U_RGB(r, g, b);
	dest[2] = V_RGB(r, g, b);
}

void rgb2yuv16bit(unsigned char* rgbSource,
		  unsigned char* lum,
		  unsigned char* cr,
		  unsigned char* cb,int height, int width) {

	int height2 = height / 2;
	int width2 = width / 2;
	int r, g, b, row, col, rgb;

	for (row=0 ; row<height2 ; row++) {
		for (col=0 ; col<width2 ; col++) {
			rgb = *((unsigned short*) rgbSource++ );
  			r = RED(rgb);
  			g = GREEN(rgb);
  			b = BLUE(rgb);
			
			*lum++ = Y_RGB(r, g, b);
			*cr++  = U_RGB(r, g, b);
			*cb++  = V_RGB(r, g, b);
			
			rgb = *((unsigned short*) rgbSource++ );
  			r = RED(rgb);
  			g = GREEN(rgb);
  			b = BLUE(rgb);
			
			*lum++ = Y_RGB(r, g, b);
		}
		for (col=0 ; col<width ; col++) {
			rgb = *((unsigned short*) rgbSource++ );
  			r = RED(rgb);
  			g = GREEN(rgb);
  			b = BLUE(rgb);
			
			*lum++ = Y_RGB(r, g, b);
		}
	}
}


#ifdef INTEL

void rgb2yuv16bit_mmx(unsigned char* rgbSource,
		      unsigned char* lum,
		      unsigned char* cr,
		      unsigned char* cb,int height, int width) {
	int height2 = height / 2;
	int width2 = width / 2;
	int bytesPerLine = width * 2;
	
	for (int row=0 ; row<height2 ; row++) {
		rgb2yuv16bit_mmx422_row(rgbSource, lum, cr, cb, width);
		rgbSource += bytesPerLine;
		lum += width;
		cr += width2;
		cb += width2;

		rgb2y16bit_mmx_row(rgbSource, lum, width);
		rgbSource += bytesPerLine;
		lum += width;
	}
}

void rgb2yuv16bit_mmx_fast(unsigned char* rgbSource,
			  unsigned char* lum,
		      unsigned char* cr,
		      unsigned char* cb,int height, int width) {

	int height2 = height / 2;
	int width2 = width / 2;
	int bytesPerLine = width * 2;

	for (int row=0 ; row<height2 ; row++) {
		rgb2yuv16bit_mmx422_row_fast(rgbSource, lum, cr, cb, width);
		rgbSource += bytesPerLine;
		lum += width;
		cr += width2;
		cb += width2;

		rgb2y16bit_mmx_row_fast(rgbSource, lum, width);
		rgbSource += bytesPerLine;
		lum += width;
	}
}

void rgb2yuv16bit_mmx422_row(unsigned char* rgb, 
			     unsigned char* lum, unsigned char* cr,
			     unsigned char* cb, int pixel) {
	unsigned int buf[17];

	// 36%5 = TEMP0
	// 44%5 = TEMPY
	// 52%5 = TEMPU
	// 60%5 = TEMPV

	__asm__ __volatile__ (
		"1:\n"

		// unpack hicolor ( pixel 1 - 4)
		"movq      (%0),    %%mm0\n"

		"movq      %%mm0,   %%mm1\n"
		"pand      KEEPR,   %%mm1\n"  
  		"psrlq     $8,      %%mm1\n"  // B3B2B1B0 -> mm1
  		"movq      %%mm0,   %%mm2\n"
 		"pand      KEEPG,   %%mm2\n"
		"psrlq     $3,      %%mm2\n"  // G3G2G1G0 -> mm2
  		"movq      %%mm0,   %%mm3\n"
  		"pand      KEEPB,   %%mm3\n" 
		"psllq     $3,      %%mm3\n"  // G3G2G1G0 -> mm3
		
		"movq      %%mm2,   %%mm0\n"
		"punpcklbw %%mm1,   %%mm2\n" 
		"punpckhbw %%mm1,   %%mm0\n" 

		"pxor      %%mm5,   %%mm5\n"
		"movq      %%mm3,   %%mm4\n"
		"punpcklbw %%mm5,   %%mm3\n" 
		"punpckhbw %%mm5,   %%mm4\n" 
		
		"psllq     $8,      %%mm2\n"
		"por       %%mm2,   %%mm3\n"  // 0B1G1R10B0G0G0 -> mm3
		"psllq     $8,      %%mm0\n"
		"por       %%mm0,   %%mm4\n"  // 0B3G3R30B2G2G2 -> mm4

		"movq      %%mm3,   %5\n"
		"movq      %%mm4,   8%5\n"

		// next 4 pixels ------------------------------

		"movq      8(%0),    %%mm0\n"

		"movq      %%mm0,   %%mm1\n"
		"pand      KEEPR,   %%mm1\n"  
  		"psrlq     $8,      %%mm1\n"  // B3B2B1B0 -> mm1
  		"movq      %%mm0,   %%mm2\n"
 		"pand      KEEPG,   %%mm2\n"
		"psrlq     $3,      %%mm2\n"  // G3G2G1G0 -> mm2
  		"movq      %%mm0,   %%mm3\n"
  		"pand      KEEPB,   %%mm3\n" 
		"psllq     $3,      %%mm3\n"  // G3G2G1G0 -> mm3

		"movq      %%mm2,   %%mm0\n"
		"punpcklbw %%mm1,   %%mm2\n" 
		"punpckhbw %%mm1,   %%mm0\n" 

		"pxor      %%mm5,   %%mm5\n"
		"movq      %%mm3,   %%mm4\n"
		"punpcklbw %%mm5,   %%mm3\n" 
		"punpckhbw %%mm5,   %%mm4\n" 
		
		"psllq     $8,      %%mm2\n"
		"por       %%mm2,   %%mm3\n"  // 0B1G1R10B0G0G0 -> mm3
		"psllq     $8,      %%mm0\n"
		"por       %%mm0,   %%mm4\n"  // 0B3G3R30B2G2G2 -> mm4

		"movq      %%mm3,   16%5\n"
		"movq      %%mm4,   24%5\n"

		"add       $16,     %0\n"

		// standard algorithm --------------------------------------------------
		
		// pack rgb
		// was:	"movq      (%0), %%mm1\n"  // load G2R2B1G1R1B0G0R0
		// ------------------------------
		// (uses: mm0, mm1)
		"movd      8%5,    %%mm0\n"
		"psllq     $24,      %%mm0\n"  
  		"movd      4%5,    %%mm1\n"
		"por       %%mm1,    %%mm0\n"
  		"psllq     $24,      %%mm0\n"
  		"movd      %5,     %%mm1\n"
 		"por       %%mm0,    %%mm1\n"
		// ------------------------------

		"pxor      %%mm6,   %%mm6\n"  // 0 -> mm6
		"movq      %%mm1,   %%mm0\n"  // G2R2B1G1R1B0G0R0 -> mm0
		"psrlq     $16,     %%mm1\n"  // 00G2R2B1G1R1B0 -> mm1
		"punpcklbw ZEROSX,  %%mm0\n"  // R1B0G0R0 -> mm0
		"movq      %%mm1,   %%mm7\n"  // 00G2R2B1G1R1B0 -> mm7
		"punpcklbw ZEROSX,  %%mm1\n"  // B1G1R1B0 -> mm1
		"movq      %%mm0,   %%mm2\n"  // R1B0G0R0 -> mm2
		"pmaddwd   YR0GRX,  %%mm0\n"  // yrR1,ygG0+yrR0 -> mm0

		"movq      %%mm1,   %%mm3\n"  // B1G1R1B0 -> mm3
		"pmaddwd   YBG0BX,  %%mm1\n"  // ybB1+ygG1,ybB0 -> mm1
		"movq      %%mm2,   %%mm4\n"  // R1B0G0R0 -> mm4
		"pmaddwd   UR0GRX,  %%mm2\n"  // urR1,ugG0+urR0 -> mm2
		"movq      %%mm3,   %%mm5\n"  // B1G1R1B0 -> mm5
		"pmaddwd   UBG0BX,  %%mm3\n"  // ubB1+ugG1,ubB0 -> mm3
		"punpckhbw %%mm6,   %%mm7\n"  // 00G2R2 -> mm7
		"pmaddwd   VR0GRX,  %%mm4\n"  // vrR1,vgG0+vrR0 -> mm4
		"paddd     %%mm1,   %%mm0\n"  // Y1Y0 -> mm0

		"pmaddwd   VBG0BX,  %%mm5\n"  // vbB1+vgG1,vbB0 -> mm5

		// pack rgb
		// was:	"movq      8(%0),%%mm1\n"     // R5B4G4R4B3G3R3B2 -> mm1
		// ------------------------------
		// (uses: mm1, mm6)
		"movd      20%5,  %%mm1\n"
		"psllq     $24,     %%mm1\n"
		"movd      16%5,  %%mm6\n"
		"por       %%mm6,   %%mm1\n"
		"psllq     $24,     %%mm1\n"
 		"movd      12%5,  %%mm6\n"
		"por       %%mm6,   %%mm1\n"
  		"psllq     $8,      %%mm1\n"	  
   		"movd      8%5,   %%mm6\n"
  		"psrlq     $16,     %%mm6\n"	  
		"por       %%mm6,   %%mm1\n"
		// ------------------------------

		"paddd     %%mm3,   %%mm2\n"  // U1U0 -> mm2

		"movq      %%mm1,   %%mm6\n"  // R5B4G4R4B3G3R3B2 -> mm6
		"punpcklbw ZEROSX,  %%mm1\n"  // B3G3R3B2 -> mm1
		"paddd     %%mm5,   %%mm4\n"  // V1V0 -> mm4

		//----------------------------------------------------------------------		
		
		"movq      %%mm1,   %%mm5\n"  // B3G3R3B2 -> mm5
		"psllq     $32,     %%mm1\n"  // R3B200 -> mm1
		
		"paddd     %%mm7,   %%mm1\n"  // R3B200+00G2R2=R3B2G2R2->mm1
		
		"punpckhbw ZEROSX,  %%mm6\n"  // R5B4G4R3 -> mm6
 		"movq      %%mm1,   %%mm3\n"  // R3B2G2R2 -> mm3
		
 		"pmaddwd   YR0GRX,  %%mm1\n"  // yrR3,ygG2+yrR2 -> mm1
 		"movq      %%mm5,   %%mm7\n"  // B3G3R3B2 -> mm7
		
		"pmaddwd   YBG0BX,  %%mm5\n"  // ybB3+ygG3,ybB2 -> mm5
 		"psrad     $15,     %%mm0\n"  // 32-bit scaled Y1Y0 -> mm0
		
		"movq      %%mm6,   36%5\n"  // R5B4G4R4 -> TEMP0
		"movq      %%mm3,   %%mm6\n"  // R3B2G2R2 -> mm6
		"pmaddwd   UR0GRX,  %%mm6\n"  // urR3,ugG2+urR2 -> mm6
		"psrad     $15,     %%mm2\n"  // 32-bit scaled U1U0 -> mm2
		
		"paddd     %%mm5,   %%mm1\n"  // Y3Y2 -> mm1
		"movq      %%mm7,   %%mm5\n"  // B3G3R3B2 -> mm5
	 	"pmaddwd   UBG0BX,  %%mm7\n"  // ubB3+ugG3,ubB2
		"psrad     $15,     %%mm1\n"  // 32-bit scaled Y3Y2 -> mm1
		
		"pmaddwd   VR0GRX,  %%mm3\n"  // vrR3,vgG2+vgR2
		"packssdw  %%mm1,   %%mm0\n"  // Y3Y2Y1Y0 -> mm0
		
		"pmaddwd   VBG0BX,  %%mm5\n"  // vbB3+vgG3,vbB2 -> mm5
		"psrad     $15,     %%mm4\n"  // 32-bit scaled V1V0 -> mm4

		//----------------------------------------------------------------------

		"paddd     %%mm7,    %%mm6\n"  // U3U2 -> mm6    

		// pack rgb
		// was:	"movq      16(%0), %%mm1\n"     // B7G7R7B6G6R6B5G5 -> mm1
		// ------------------------------
		// (uses: mm1, mm7)
		"movd      28%5,    %%mm1\n"
		"psllq     $24,     %%mm1\n"
		"movd      24%5,    %%mm7\n"
		"por       %%mm7,   %%mm1\n"
		"psllq     $16,     %%mm1\n"
  		"movd      20%5,    %%mm7\n"
		"psrlq     $8,      %%mm7\n"	  
		"por       %%mm7,   %%mm1\n"
		// ------------------------------

		"movq      %%mm1,    %%mm7\n"  // B7G7R7B6G6R6B5G5 -> mm1
		"psrad     $15,      %%mm6\n"  // 32-bit scaled U3U2 -> mm6

		"paddd     %%mm5,    %%mm3\n"  // V3V2 -> mm3
		"psllq     $16,      %%mm7\n"  // R7B6G6R6B5G500 -> mm7
		
		"movq      %%mm7,    %%mm5\n"  // R7B6G6R6B5G500 -> mm5
		"psrad     $15,      %%mm3\n"  // 32-bit scaled V3V2 -> mm3
		
		"movq      %%mm0,    44%5\n"  // 32-bit scaled Y3Y2Y1Y0 -> TEMPY

		"packssdw  %%mm6,    %%mm2\n"  // 32-bit scaled U3U2U1U0 -> mm2
		
		"movq      36%5,    %%mm0\n"  // R5B4G4R4 -> mm0
		
		"punpcklbw ZEROSX,   %%mm7\n"  // B5G500 -> mm7
		"movq      %%mm0,    %%mm6\n"  // R5B4G4R4 -> mm6
		
		"movq      %%mm2,    52%5\n"  // 32-bit scaled U3U2U1U0 -> TEMPU
		"psrlq     $32,      %%mm0\n"  // 00R5B4 -> mm0
		
		"paddw     %%mm0,    %%mm7\n"  // B5G5R5B4 -> mm7
		"movq      %%mm6,    %%mm2\n"  // B5B4G4R4 -> mm2
		
		"pmaddwd   YR0GRX,   %%mm2\n"  // yrR5,ygG4+yrR4 -> mm2
		"movq      %%mm7,    %%mm0\n"  // B5G5R5B4 -> mm0
		
		"pmaddwd   YBG0BX,   %%mm7\n"  // ybB5+ygG5,ybB4 -> mm7
		"packssdw  %%mm3,    %%mm4\n"  // 32-bit scaled V3V2V1V0 -> mm4

		//----------------------------------------------------------------------
		
		"movq      %%mm4,    60%5\n"  // (V3V2V1V0)/256 -> mm4

		"movq      %%mm6,    %%mm4\n"  // B5B4G4R4 -> mm4
		
		"pmaddwd   UR0GRX,   %%mm6\n"  // urR5,ugG4+urR4
		"movq      %%mm0,    %%mm3\n"  // B5G5R5B4 -> mm0
		
		"pmaddwd   UBG0BX,   %%mm0\n"  // ubB5+ugG5,ubB4
		"paddd     %%mm7,    %%mm2\n"  // Y5Y4 -> mm2

		//----------------------------------------------------------------------
		
		"pmaddwd   VR0GRX,   %%mm4\n"  // vrR5,vgG4+vrR4 -> mm4
		"pxor      %%mm7,    %%mm7\n"  // 0 -> mm7
		
		"pmaddwd   VBG0BX,   %%mm3\n"  // vbB5+vgG5,vbB4 -> mm3
		"punpckhbw %%mm7,    %%mm1\n"  // B7G7R7B6 -> mm1
		
		"paddd     %%mm6,    %%mm0\n"  // U5U4 -> mm0
		"movq      %%mm1,    %%mm6\n"  // B7G7R7B6 -> mm6
		
		"pmaddwd   YBG0BX,   %%mm6\n"  // ybB7+ygG7,ybB6 -> mm6
		"punpckhbw %%mm7,    %%mm5\n"  // R7B6G6R6 -> mm5
		
		"movq      %%mm5,    %%mm7\n"  // R7B6G6R6 -> mm7
		"paddd     %%mm4,    %%mm3\n"  // V5V4 -> mm3
		
		"pmaddwd   YR0GRX,   %%mm5\n"  // yrR7,ygG6+yrR6 -> mm5
		"movq      %%mm1,    %%mm4\n"  // B7G7R7B6 -> mm4
		
		"pmaddwd   UBG0BX,   %%mm4\n"  // ubB7+ugG7,ubB6 -> mm4
		"psrad     $15,      %%mm0\n"  // 32-bit scaled U5U4 -> %%mm0
		
		//----------------------------------------------------------------------
		
 		"paddd     OFFSETWX, %%mm0\n"  // add offset to U5U4 -> mm0
		"psrad     $15,      %%mm2\n"  // 32-bit scaled Y5Y4 -> mm2
		
		"paddd     %%mm5,    %%mm6\n"  // Y7Y6 -> mm6
		"movq      %%mm7,    %%mm5\n"  // R7B6G6R6 -> mm5
		
		"pmaddwd   UR0GRX,   %%mm7\n"  // urR7,ugG6+ugR6 -> mm7
		"psrad     $15,      %%mm3\n"  // 32-bit scaled V5V4 -> mm3
		
		"pmaddwd   VBG0BX,   %%mm1\n"  // vbB7+vgG7,vbB6 -> mm1
		"psrad     $15,      %%mm6\n"  // 32-bit scaled Y7Y6 -> mm6
		
		"paddd     OFFSETDX, %%mm4\n"  // add offset to U7U6 
		"packssdw  %%mm6,    %%mm2\n"  // Y7Y6Y5Y4 -> mm2
		
		"pmaddwd   VR0GRX,   %%mm5\n"  // vrR7,vgG6+vrR6 -> mm5
		"paddd     %%mm4,    %%mm7\n"  // U7U6 -> mm7    
		
		"psrad     $15,      %%mm7\n"  // 32-bit scaled U7U6 -> mm7

		//----------------------------------------------------------------------
		
		"movq      44%5,    %%mm6\n"  // 32-bit scaled Y3Y2Y1Y0 -> mm6
		"packssdw  %%mm7,    %%mm0\n"  // 32-bit scaled U7U6U5U4 -> mm0
		
		"movq      52%5,    %%mm4\n"  // 32-bit scaled U3U2U1U0 -> mm4
		"packuswb  %%mm2,    %%mm6\n"  // all 8 Y values -> mm6
		
		"movq      OFFSETBX, %%mm7\n"  // 128,128,128,128 -> mm7
		"paddd     %%mm5,    %%mm1\n"  // V7V6 -> mm1
		
		"paddw     %%mm7,    %%mm4\n"  // add offset to U3U2U1U0/256 
		"psrad     $15,      %%mm1\n"  // 32-bit scaled V7V6 -> mm1
		
		//----------------------------------------------------------------------

		"movq      %%mm6,    (%1)\n"     // store Y

		"packuswb  %%mm0,    %%mm4\n"    // all 8 U values -> mm4
		"movq      60%5,    %%mm5\n"  // 32-bit scaled V3V2V1V0 -> mm5

		"packssdw  %%mm1,    %%mm3\n"  // V7V6V5V4 -> mm3
		"paddw     %%mm7,    %%mm5\n"  // add offset to  V3V2V1V0
		"paddw     %%mm7,    %%mm3\n"  // add offset to  V7V6V5V4
		
		"packuswb  %%mm3,    %%mm5\n"  // ALL 8 V values -> mm5

		"movq      CLEARX, %%mm2\n"
		"pand      %%mm2, %%mm4\n"
		"pand      %%mm2, %%mm5\n"

		"packuswb  %%mm5, %%mm4\n" 

		"movd      %%mm4,    (%2)\n"
		"psrlq     $32,      %%mm4\n"
		"movd      %%mm4,    (%3)\n"

		"add       $8,       %1\n"
		"add       $4,       %2\n"
		"add       $4,       %3\n"
		
		"sub       $8,       %4\n"
  		"jnz       1b\n"

		"emms\n"

		:
		: "r" (rgb), "r" (lum), "r" (cr), "r" (cb),
		"m" (pixel), "m" (buf[0])

		);
}

void rgb2yuv16bit_mmx422_row_fast(unsigned char* rgb, 
				  unsigned char* lum, unsigned char* cr,
				  unsigned char* cb, int pixel)
{
	__asm__ __volatile__ (
		"1:\n"

		// unpack hicolor ( pixel 0 - 3)
		"movq      (%0),    %%mm0\n"

		"movq      %%mm0,   %%mm1\n"
		"pand      KEEPR,   %%mm1\n"  
  		"psrlq     $11,     %%mm1\n"  // B3B2B1B0 -> mm1

  		"movq      %%mm0,   %%mm2\n"
 		"pand      KEEPG,   %%mm2\n"
		"psrlq     $5,      %%mm2\n"  // G3G2G1G0 -> mm2

  		"movq      %%mm0,   %%mm3\n"
  		"pand      KEEPB,   %%mm3\n"  // R3R2R1R0 -> mm3

		// unpack hicolor ( pixel 4 - 7)
		"movq      8(%0),    %%mm0\n"

		"movq      %%mm0,   %%mm4\n"
		"pand      KEEPR,   %%mm4\n"  
  		"psrlq     $11,     %%mm4\n"  // B7B6B5B4 -> mm4

  		"movq      %%mm0,   %%mm5\n"
 		"pand      KEEPG,   %%mm5\n"
		"psrlq     $5,      %%mm5\n"  // G7G6G5G4 -> mm5

  		"movq      %%mm0,   %%mm6\n"
  		"pand      KEEPB,   %%mm6\n"  // R7R6R5R4 -> mm6

		// calculate Y
		"movq      %%mm6,   %%mm7\n"
		"pmullw    Y_RED,   %%mm7\n"

		"movq      %%mm5,   %%mm0\n"
		"pmullw    Y_GREEN, %%mm0\n"
		"paddw     %%mm0,   %%mm7\n"

		"movq      %%mm4,   %%mm0\n"
		"pmullw    Y_BLUE,  %%mm0\n"
		"paddw     %%mm0,   %%mm7\n"

		"psrlw     $7,      %%mm7\n"  // Y3Y2Y1Y0 -> mm7
		
		"pxor      %%mm0,   %%mm0\n"
		"packuswb  %%mm0,   %%mm7\n"
		"movd      %%mm7,   4(%1)\n"  // Y3Y2Y1Y0 -> lum

		// --------
		
		"movq      %%mm3,   %%mm7\n"
		"pmullw    Y_RED,   %%mm7\n"

		"movq      %%mm2,   %%mm0\n"
		"pmullw    Y_GREEN, %%mm0\n"
		"paddw     %%mm0,   %%mm7\n"

		"movq      %%mm1,   %%mm0\n"
		"pmullw    Y_BLUE,  %%mm0\n"
		"paddw     %%mm0,   %%mm7\n"

		"psrlw     $7,      %%mm7\n"  // Y7Y6Y5Y4 -> mm7
		
		"pxor      %%mm0,   %%mm0\n"
		"packuswb  %%mm0,   %%mm7\n"
		"movd      %%mm7,   (%1)\n"   // Y7Y6Y5Y4 -> lum
		"add       $8,      %1\n"

		// pack RGB
		"packuswb  %%mm4,   %%mm1\n"
		"pand      CLEARX,  %%mm1\n"  // B6B4B2B0 -> mm1
		"packuswb  %%mm5,   %%mm2\n"
		"pand      CLEARX,  %%mm2\n"  // GRG4G2G0 -> mm2
		"packuswb  %%mm6,   %%mm3\n"
 		"pand      CLEARX,  %%mm3\n"  // R6R4R2R0 -> mm3

		// calculate U
		"movq      %%mm3,   %%mm7\n"
		"pmullw    U_RED,   %%mm7\n"

		"movq      %%mm2,   %%mm0\n"
		"pmullw    U_GREEN, %%mm0\n"
		"paddw     %%mm0,   %%mm7\n"

		"movq      %%mm1,   %%mm0\n"
		"pmullw    U_BLUE,  %%mm0\n"
		"paddw     %%mm0,   %%mm7\n"

		"psrlw     $7,      %%mm7\n"  // U3U2U1U0 -> mm7
		"paddw     OFFSETBX,%%mm7\n"
		"pand      CLEARX,  %%mm7\n"
		
		"pxor      %%mm0,   %%mm0\n"
		"packuswb  %%mm0,   %%mm7\n"
		"movd      %%mm7,   (%2)\n"   // U3U2U1U0 -> lum
		"add       $4,      %2\n"

		// calculate V
		"movq      %%mm3,   %%mm7\n"
		"pmullw    V_RED,   %%mm7\n"

		"movq      %%mm2,   %%mm0\n"
		"pmullw    V_GREEN, %%mm0\n"
		"paddw     %%mm0,   %%mm7\n"

		"movq      %%mm1,   %%mm0\n"
		"pmullw    V_BLUE,  %%mm0\n"
		"paddw     %%mm0,   %%mm7\n"

		"psrlw     $7,      %%mm7\n"  // V3V2V1V0 -> mm7
		"paddw     OFFSETBX,%%mm7\n"
		"pand      CLEARX,  %%mm7\n"
		
		"pxor      %%mm0,   %%mm0\n"
		"packuswb  %%mm0,   %%mm7\n"
		"movd      %%mm7,   (%3)\n"   // V3V2V1V0 -> lum
		"add       $4,      %3\n"

		"add       $16,     %0\n"
		
		"sub       $8,      %4\n"
		"jnz       1b\n"

		"emms\n"

		:
		: "r" (rgb), "r" (lum), "r" (cr), "r" (cb), "m" (pixel)
		
		);
}

void rgb2y16bit_mmx_row(unsigned char* rgbSource, 
			unsigned char* lum, int pixel)
{
	unsigned int buf[16];

	// 36%3 = TEMP0
	// 44%3 = TEMPY
	
	__asm__ __volatile__ (
		"1:\n"

		// unpack hicolor ( pixel 1 - 4)
		"movq      (%0),    %%mm0\n"

		"movq      %%mm0,   %%mm1\n"
		"pand      KEEPR,   %%mm1\n"  
  		"psrlq     $8,      %%mm1\n"  // B3B2B1B0 -> mm1
  		"movq      %%mm0,   %%mm2\n"
 		"pand      KEEPG,   %%mm2\n"
		"psrlq     $3,      %%mm2\n"  // G3G2G1G0 -> mm2
  		"movq      %%mm0,   %%mm3\n"
  		"pand      KEEPB,   %%mm3\n" 
		"psllq     $3,      %%mm3\n"  // G3G2G1G0 -> mm3

		"movq      %%mm2,   %%mm0\n"
		"punpcklbw %%mm1,   %%mm2\n" 
		"punpckhbw %%mm1,   %%mm0\n" 

		"pxor      %%mm5,   %%mm5\n"
		"movq      %%mm3,   %%mm4\n"
		"punpcklbw %%mm5,   %%mm3\n" 
		"punpckhbw %%mm5,   %%mm4\n" 
		
		"psllq     $8,      %%mm2\n"
		"por       %%mm2,   %%mm3\n"  // 0B1G1R10B0G0G0 -> mm3
		"psllq     $8,      %%mm0\n"
		"por       %%mm0,   %%mm4\n"  // 0B3G3R30B2G2G2 -> mm4

		"movq      %%mm3,   %3\n"
		"movq      %%mm4,   8%3\n"

		// next 4 pixels ------------------------------

		"movq      8(%0),    %%mm0\n"

		"movq      %%mm0,   %%mm1\n"
		"pand      KEEPR,   %%mm1\n"  
  		"psrlq     $8,      %%mm1\n"  // B3B2B1B0 -> mm1
  		"movq      %%mm0,   %%mm2\n"
 		"pand      KEEPG,   %%mm2\n"
		"psrlq     $3,      %%mm2\n"  // G3G2G1G0 -> mm2
  		"movq      %%mm0,   %%mm3\n"
  		"pand      KEEPB,   %%mm3\n" 
		"psllq     $3,      %%mm3\n"  // G3G2G1G0 -> mm3

		"movq      %%mm2,   %%mm0\n"
		"punpcklbw %%mm1,   %%mm2\n" 
		"punpckhbw %%mm1,   %%mm0\n" 

		"pxor      %%mm5,   %%mm5\n"
		"movq      %%mm3,   %%mm4\n"
		"punpcklbw %%mm5,   %%mm3\n" 
		"punpckhbw %%mm5,   %%mm4\n" 
		
		"psllq     $8,      %%mm2\n"
		"por       %%mm2,   %%mm3\n"  // 0B1G1R10B0G0G0 -> mm3
		"psllq     $8,      %%mm0\n"
		"por       %%mm0,   %%mm4\n"  // 0B3G3R30B2G2G2 -> mm4

		"movq      %%mm3,   16%3\n"
		"movq      %%mm4,   24%3\n"

		"add       $16,     %0\n"

		// standard algorithm --------------------------------------------------

		// pack rgb
		// was:	"movq      (%0), %%mm1\n"  // load G2R2B1G1R1B0G0R0
		// ------------------------------
		// (uses: mm0, mm1)
		"movd      8%3,    %%mm0\n"
		"psllq     $24,      %%mm0\n"  
  		"movd      4%3,    %%mm1\n"
		"por       %%mm1,    %%mm0\n"
  		"psllq     $24,      %%mm0\n"
  		"movd      %3,     %%mm1\n"
		"por       %%mm0,    %%mm1\n"
		// ------------------------------

		"pxor      %%mm6,   %%mm6\n"  // 0 -> mm6
		"movq      %%mm1,   %%mm0\n"  // G2R2B1G1R1B0G0R0 -> mm0
		"psrlq     $16,     %%mm1\n"  // 00G2R2B1G1R1B0 -> mm1
		"punpcklbw ZEROSX,  %%mm0\n"  // R1B0G0R0 -> mm0
		"movq      %%mm1,   %%mm7\n"  // 00G2R2B1G1R1B0 -> mm7
		"punpcklbw ZEROSX,  %%mm1\n"  // B1G1R1B0 -> mm1
		"movq      %%mm0,   %%mm2\n"  // R1B0G0R0 -> mm2
		"pmaddwd   YR0GRX,  %%mm0\n"  // yrR1,ygG0+yrR0 -> mm0
		"movq      %%mm1,   %%mm3\n"  // B1G1R1B0 -> mm3
		"pmaddwd   YBG0BX,  %%mm1\n"  // ybB1+ygG1,ybB0 -> mm1
		"movq      %%mm2,   %%mm4\n"  // R1B0G0R0 -> mm4
		"movq      %%mm3,   %%mm5\n"  // B1G1R1B0 -> mm5
		"punpckhbw %%mm6,   %%mm7\n"  // 00G2R2 -> mm7
		"paddd     %%mm1,   %%mm0\n"  // Y1Y0 -> mm0

		// pack rgb
		// was:	"movq      8(%0),%%mm1\n"     // R5B4G4R4B3G3R3B2 -> mm1
		// ------------------------------
		// (uses: mm1, mm6)
		"movd      20%3,  %%mm1\n"
		"psllq     $24,     %%mm1\n"
		"movd      16%3,  %%mm6\n"
		"por       %%mm6,   %%mm1\n"
		"psllq     $24,     %%mm1\n"
 		"movd      12%3,  %%mm6\n"
		"por       %%mm6,   %%mm1\n"
  		"psllq     $8,      %%mm1\n"	  
   		"movd      8%3,   %%mm6\n"
  		"psrlq     $16,     %%mm6\n"	  
		"por       %%mm6,   %%mm1\n"
		// ------------------------------

		"movq      %%mm1,   %%mm6\n"  // R5B4G4R4B3G3R3B2 -> mm6
		"punpcklbw ZEROSX,  %%mm1\n"  // B3G3R3B2 -> mm1

		//----------------------------------------------------------------------		
		
		"movq      %%mm1,   %%mm5\n"  // B3G3R3B2 -> mm5
		"psllq     $32,     %%mm1\n"  // R3B200 -> mm1
		
		"paddd     %%mm7,   %%mm1\n"  // R3B200+00G2R2=R3B2G2R2->mm1
		
		"punpckhbw ZEROSX,  %%mm6\n"  // R5B4G4R3 -> mm6
 		"movq      %%mm1,   %%mm3\n"  // R3B2G2R2 -> mm3
		
 		"pmaddwd   YR0GRX,  %%mm1\n"  // yrR3,ygG2+yrR2 -> mm1
 		"movq      %%mm5,   %%mm7\n"  // B3G3R3B2 -> mm7
		
		"pmaddwd   YBG0BX,  %%mm5\n"  // ybB3+ygG3,ybB2 -> mm5
 		"psrad     $15,     %%mm0\n"  // 32-bit scaled Y1Y0 -> mm0
		
		"movq      %%mm6,   36%3\n"  // R5B4G4R4 -> TEMP0
		"movq      %%mm3,   %%mm6\n"  // R3B2G2R2 -> mm6
		
		"paddd     %%mm5,   %%mm1\n"  // Y3Y2 -> mm1
		"movq      %%mm7,   %%mm5\n"  // B3G3R3B2 -> mm5
		"psrad     $15,     %%mm1\n"  // 32-bit scaled Y3Y2 -> mm1
		
		"packssdw  %%mm1,   %%mm0\n"  // Y3Y2Y1Y0 -> mm0

		//----------------------------------------------------------------------

		// pack rgb
		// was:	"movq      16(%0), %%mm1\n"     // B7G7R7B6G6R6B5G5 -> mm1
		// ------------------------------
		// (uses: mm1, mm7)
		"movd      28%3,  %%mm1\n"
		"psllq     $24,     %%mm1\n"
		"movd      24%3,  %%mm7\n"
		"por       %%mm7,   %%mm1\n"
		"psllq     $16,     %%mm1\n"
  		"movd      20%3,  %%mm7\n"
		"psrlq     $8,      %%mm7\n"	  
		"por       %%mm7,   %%mm1\n"
		// ------------------------------
		
		"movq      %%mm1,    %%mm7\n"  // B7G7R7B6G6R6B5G5 -> mm1
		
		"psllq     $16,      %%mm7\n"  // R7B6G6R6B5G500 -> mm7
		
		"movq      %%mm7,    %%mm5\n"  // R7B6G6R6B5G500 -> mm5
		
		"movq      %%mm0,    44%3\n"  // 32-bit scaled Y3Y2Y1Y0 -> TEMPY
		
		"movq      36%3,    %%mm0\n"  // R5B4G4R4 -> mm0
		
		"punpcklbw ZEROSX,   %%mm7\n"  // B5G500 -> mm7
		"movq      %%mm0,    %%mm6\n"  // R5B4G4R4 -> mm6
		
		"psrlq     $32,      %%mm0\n"  // 00R5B4 -> mm0
		
		"paddw     %%mm0,    %%mm7\n"  // B5G5R5B4 -> mm7
		"movq      %%mm6,    %%mm2\n"  // B5B4G4R4 -> mm2
		
		"pmaddwd   YR0GRX,   %%mm2\n"  // yrR5,ygG4+yrR4 -> mm2
		
		"pmaddwd   YBG0BX,   %%mm7\n"  // ybB5+ygG5,ybB4 -> mm7

		//----------------------------------------------------------------------
		"paddd     %%mm7,    %%mm2\n"  // Y5Y4 -> mm2
		
		//----------------------------------------------------------------------
		
		"pxor      %%mm7,    %%mm7\n"  // 0 -> mm7
		
		"punpckhbw %%mm7,    %%mm1\n"  // B7G7R7B6 -> mm1
		
		"movq      %%mm1,    %%mm6\n"  // B7G7R7B6 -> mm6
		
		"pmaddwd   YBG0BX,   %%mm6\n"  // ybB7+ygG7,ybB6 -> mm6
		"punpckhbw %%mm7,    %%mm5\n"  // R7B6G6R6 -> mm5
		
		"pmaddwd   YR0GRX,   %%mm5\n"  // yrR7,ygG6+yrR6 -> mm5
		
		//----------------------------------------------------------------------

		"psrad     $15,      %%mm2\n"  // 32-bit scaled Y5Y4 -> mm2
		
		"paddd     %%mm5,    %%mm6\n"  // Y7Y6 -> mm6
		"psrad     $15,      %%mm6\n"  // 32-bit scaled Y7Y6 -> mm6
		
		"packssdw  %%mm6,    %%mm2\n"  // Y7Y6Y5Y4 -> mm2

		//----------------------------------------------------------------------
		
		"movq      44%3,    %%mm6\n"  // 32-bit scaled Y3Y2Y1Y0 -> mm6
  		"packuswb  %%mm2,    %%mm6\n"  // all 8 Y values -> mm6
		
		//----------------------------------------------------------------------

		"movq      %%mm6,  (%1)\n"     // store Y
		
		"add       $8,     %1\n"
		
		"sub       $8,     %2\n"
  		"jnz       1b\n"
		"emms\n"

		:
		: "r" (rgbSource), "r" (lum), "m" (pixel), "m" (buf[0])
		
	);
}

void rgb2y16bit_mmx_row_fast(unsigned char* rgb, unsigned char* lum, int pixel)
{
	__asm__ __volatile__ (
        "1:\n"
		
		// unpack hicolor ( pixel 1 - 4)
		"movq      (%0),    %%mm0\n"
		
		"movq      %%mm0,   %%mm1\n"
		"pand      KEEPR,   %%mm1\n"
		"psrlq     $11,     %%mm1\n"  // B3B2B1B0 -> mm1
		
		"movq      %%mm0,   %%mm2\n"
		"pand      KEEPG,   %%mm2\n"
		"psrlq     $5,      %%mm2\n"  // G3G2G1G0 -> mm2
		"movq      %%mm0,   %%mm3\n"
		"pand      KEEPB,   %%mm3\n"  // R3R2R1R0 -> mm3
		
		// calculate Y
		"movq      %%mm3,   %%mm4\n"
		"pmullw    Y_RED,   %%mm4\n"
		
		"movq      %%mm2,   %%mm5\n"
		"pmullw    Y_GREEN, %%mm5\n"
		"paddw     %%mm5,   %%mm4\n"
		
		"movq      %%mm1,   %%mm6\n"
		"pmullw    Y_BLUE,  %%mm6\n"
		"paddw     %%mm6,   %%mm4\n"
		
		"psrlw     $7,      %%mm4\n"  // Y3Y2Y1Y0 -> mm4
		
		"pxor      %%mm5,   %%mm5\n"
		"packuswb  %%mm5,   %%mm4\n"
		
		"movd      %%mm4,   (%1)\n"
		"add       $4,      %1\n"  
		
		"add       $8,      %0\n"
		
		"sub       $4,      %2\n"
		"jnz       1b\n"
		
		"emms\n"
		
		:
		: "r" (rgb), "r" (lum), "m" (pixel)
    );
}


#endif
// INTEL


