/***************************************************************************
                          rgb2yuv32.cpp  -  description
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

#include "rgb2yuv32.h"
#include <iostream>

void rgb2yuv32(unsigned char* rgb, unsigned char* dest)
{
	dest[0] = Y_RGB(rgb[0], rgb[1], rgb[2]);
	dest[1] = U_RGB(rgb[0], rgb[1], rgb[2]);
	dest[2] = V_RGB(rgb[0], rgb[1], rgb[2]);
}

void rgb2yuv24bit(unsigned char* rgbSource,
		  unsigned char* lum,
		  unsigned char* cr,
		  unsigned char* cb,int height, int width) {
	int width2 = width / 2;
	int height2 = height / 2;
	int r, g, b, row, col;

	for (row=0 ; row<height2 ; row++) {
		for (col=0 ; col<width2 ; col++) {
			r = *rgbSource++;
			g = *rgbSource++;
			b = *rgbSource++;
			
			*lum++ = Y_RGB(r, g, b);
			*cr++  = U_RGB(r, g, b);
			*cb++  = V_RGB(r, g, b);
			
			r = *rgbSource++;
			g = *rgbSource++;
			b = *rgbSource++;
			
			*lum++ = Y_RGB(r, g, b);
		}
		for (col=0 ; col<width ; col++) {
			r = *rgbSource++;
			g = *rgbSource++;
			b = *rgbSource++;
			
			*lum++ = Y_RGB(r, g, b);
		}
	}
}

void rgb2yuv32bit(unsigned char* rgbSource,
		  unsigned char* lum,
		  unsigned char* cr,
		  unsigned char* cb,int height, int width) {

	int width2 = width / 2;
	int height2 = height / 2;
	int r, g, b, row, col;

	for (row=0 ; row<height2 ; row++) {
		for (col=0 ; col<width2 ; col++) {
			r = *rgbSource++;
			g = *rgbSource++;
			b = *rgbSource++;
			rgbSource ++;
			
			*lum++ = Y_RGB(r, g, b);
			*cr++  = U_RGB(r, g, b);
			*cb++  = V_RGB(r, g, b);
			
			r = *rgbSource++;
			g = *rgbSource++;
			b = *rgbSource++;
			rgbSource++;
			
			*lum++ = Y_RGB(r, g, b);
		}
		for (col=0 ; col<width ; col++) {
			r = *rgbSource++;
			g = *rgbSource++;
			b = *rgbSource++;
			rgbSource ++;
			
			*lum++ = Y_RGB(r, g, b);
		}
	}
}

#ifndef INTEL 
void rgb2yuv24bit_mmx(unsigned char* rgbSource,
		      unsigned char* lum,
		      unsigned char* cr,
		      unsigned char* cb,int height, int width) {
  std::cout << "RGB->YUV render not compiled for INTEL"<<std::endl;
  exit(0);
}

void rgb2yuv32bit_mmx(unsigned char* rgbSource,
		      unsigned char* lum,
		      unsigned char* cr,
		      unsigned char* cb,int height, int width) {
  std::cout << "RGB->YUV render not compiled for INTEL"<<std::endl;
  exit(0); 
}

#endif

#ifdef INTEL

void rgb2yuv24bit_mmx(unsigned char* rgbSource,
		      unsigned char* lum,
		      unsigned char* cr,
		      unsigned char* cb,int height, int width) {
	int width2 = width / 2;
	int height2 = height / 2;
	int row;
	int bytesPerLine = width * 3;
		
	for (row=0 ; row<height2 ; row++) {
		rgb2yuv24bit_mmx422_row(rgbSource, lum, cr, cb, width);
		rgbSource += bytesPerLine;
		lum += width;
		cr += width2;
		cb += width2;

		rgb2y24bit_mmx_row(rgbSource, lum, width);
  		rgbSource += bytesPerLine;
  		lum += width;
	}
}

void rgb2yuv32bit_mmx(unsigned char* rgbSource,
		      unsigned char* lum,
		      unsigned char* cr,
		      unsigned char* cb,int height, int width) {


	int width2 = width / 2;
	int height2 = height / 2;
	int bytesPerLine = width * 4;
		
	for (int row=0 ; row<height2 ; row++) {
		rgb2yuv32bit_mmx422_row(rgbSource, lum,cr, cb, width);
		rgbSource += bytesPerLine;
		lum += width;
		cr += width2;
		cb += width2;

		rgb2y32bit_mmx_row(rgbSource, lum, width);
  		rgbSource += bytesPerLine;
  		lum += width;
 	}
}

void rgb2yuv24bit_mmx444_row(unsigned char* rgb, unsigned char* lum, unsigned char* cr,
							 unsigned char* cb, int pixel)
{
	unsigned int buf[8];

	// %5 = TEMP0
	// 8%5 = TEMPY
	// 16%5 = TEMPU
	// 24%5 = TEMPV

	__asm__ __volatile__ (
		"1:\n"

		"movq      (%0), %%mm1\n"  // load G2R2B1G1R1B0G0R0
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

		"movq      8(%0),%%mm1\n"  // load G2R2B1G1R1B0G0R0
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
		
		"movq      %%mm6,   %5\n"  // R5B4G4R4 -> TEMP0
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

		"movq      16(%0), %%mm1\n"  // B7G7R7B6G6R6B5G5 -> mm7
		"paddd     %%mm7,    %%mm6\n"  // U3U2 -> mm6    
		
		"movq      %%mm1,    %%mm7\n"  // B7G7R7B6G6R6B5G5 -> mm1
		"psrad     $15,      %%mm6\n"  // 32-bit scaled U3U2 -> mm6
		
		"paddd     %%mm5,    %%mm3\n"  // V3V2 -> mm3
		"psllq     $16,      %%mm7\n"  // R7B6G6R6B5G500 -> mm7
		
		"movq      %%mm7,    %%mm5\n"  // R7B6G6R6B5G500 -> mm5
		"psrad     $15,      %%mm3\n"  // 32-bit scaled V3V2 -> mm3
		
		"movq      %%mm0,    8%5\n"  // 32-bit scaled Y3Y2Y1Y0 -> TEMPY
		"packssdw  %%mm6,    %%mm2\n"  // 32-bit scaled U3U2U1U0 -> mm2
		
		"movq      %5,    %%mm0\n"  // R5B4G4R4 -> mm0
		
		"punpcklbw ZEROSX,   %%mm7\n"  // B5G500 -> mm7
		"movq      %%mm0,    %%mm6\n"  // R5B4G4R4 -> mm6
		
		"movq      %%mm2,    16%5\n"  // 32-bit scaled U3U2U1U0 -> TEMPU
		"psrlq     $32,      %%mm0\n"  // 00R5B4 -> mm0
		
		"paddw     %%mm0,    %%mm7\n"  // B5G5R5B4 -> mm7
		"movq      %%mm6,    %%mm2\n"  // B5B4G4R4 -> mm2
		
		"pmaddwd   YR0GRX,   %%mm2\n"  // yrR5,ygG4+yrR4 -> mm2
		"movq      %%mm7,    %%mm0\n"  // B5G5R5B4 -> mm0
		
		"pmaddwd   YBG0BX,   %%mm7\n"  // ybB5+ygG5,ybB4 -> mm7
		"packssdw  %%mm3,    %%mm4\n"  // 32-bit scaled V3V2V1V0 -> mm4

		//----------------------------------------------------------------------
		
		"movq      %%mm4,    24%5\n"  // (V3V2V1V0)/256 -> mm4

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
		
		"movq      8%5,    %%mm6\n"  // 32-bit scaled Y3Y2Y1Y0 -> mm6
		"packssdw  %%mm7,    %%mm0\n"  // 32-bit scaled U7U6U5U4 -> mm0
		
		"movq      16%5,    %%mm4\n"  // 32-bit scaled U3U2U1U0 -> mm4
		"packuswb  %%mm2,    %%mm6\n"  // all 8 Y values -> mm6
		
		"movq      OFFSETBX, %%mm7\n"  // 128,128,128,128 -> mm7
		"paddd     %%mm5,    %%mm1\n"  // V7V6 -> mm1
		
		"paddw     %%mm7,    %%mm4\n"  // add offset to U3U2U1U0/256 
		"psrad     $15,      %%mm1\n"  // 32-bit scaled V7V6 -> mm1
		
		//----------------------------------------------------------------------

		"movq      %%mm6,  (%1)\n"     // store Y
		"packuswb  %%mm0,  %%mm4\n"    // all 8 U values -> mm4
		
		"movq      24%5,    %%mm5\n"  // 32-bit scaled V3V2V1V0 -> mm5
		"packssdw  %%mm1,    %%mm3\n"  // V7V6V5V4 -> mm3
		"paddw     %%mm7,    %%mm5\n"  // add offset to  V3V2V1V0
		"paddw     %%mm7,    %%mm3\n"  // add offset to  V7V6V5V4
		
		"movq      %%mm4,    (%2)\n"   // store U
		"packuswb  %%mm3,    %%mm5\n"  // ALL 8 V values -> mm5

		"movq      %%mm5,   (%3)\n"    // store V

		"sub       $8,       %4\n"
  		"jnz       1b\n"
		"emms\n"

		:
		: "r" (rgb), "r" (lum), "r" (cr), "r" (cb), "m" (pixel), "m" (buf[0])
		);
}

void rgb2yuv24bit_mmx422_row(unsigned char* rgb, unsigned char* lum, unsigned char* cr,
							 unsigned char* cb, int pixel)
{
	unsigned int buf[8];

	// %5 = TEMP0
	// 8%5 = TEMPY
	// 16%5 = TEMPU
	// 24%5 = TEMPV

	__asm__ __volatile__ (
		"1:\n"

		"movq      (%0), %%mm1\n"  // load G2R2B1G1R1B0G0R0
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

		"movq      8(%0),%%mm1\n"     // load G2R2B1G1R1B0G0R0
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
		
		"movq      %%mm6,   %5\n"  // R5B4G4R4 -> TEMP0
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

		"movq      16(%0), %%mm1\n"  // B7G7R7B6G6R6B5G5 -> mm7
		"paddd     %%mm7,    %%mm6\n"  // U3U2 -> mm6    
		
		"movq      %%mm1,    %%mm7\n"  // B7G7R7B6G6R6B5G5 -> mm1
		"psrad     $15,      %%mm6\n"  // 32-bit scaled U3U2 -> mm6
		
		"paddd     %%mm5,    %%mm3\n"  // V3V2 -> mm3
		"psllq     $16,      %%mm7\n"  // R7B6G6R6B5G500 -> mm7
		
		"movq      %%mm7,    %%mm5\n"  // R7B6G6R6B5G500 -> mm5
		"psrad     $15,      %%mm3\n"  // 32-bit scaled V3V2 -> mm3
		
		"movq      %%mm0,    8%5\n"  // 32-bit scaled Y3Y2Y1Y0 -> TEMPY
		"packssdw  %%mm6,    %%mm2\n"  // 32-bit scaled U3U2U1U0 -> mm2
		
		"movq      %5,    %%mm0\n"  // R5B4G4R4 -> mm0
		
		"punpcklbw ZEROSX,   %%mm7\n"  // B5G500 -> mm7
		"movq      %%mm0,    %%mm6\n"  // R5B4G4R4 -> mm6
		
		"movq      %%mm2,    16%5\n"  // 32-bit scaled U3U2U1U0 -> TEMPU
		"psrlq     $32,      %%mm0\n"  // 00R5B4 -> mm0
		
		"paddw     %%mm0,    %%mm7\n"  // B5G5R5B4 -> mm7
		"movq      %%mm6,    %%mm2\n"  // B5B4G4R4 -> mm2
		
		"pmaddwd   YR0GRX,   %%mm2\n"  // yrR5,ygG4+yrR4 -> mm2
		"movq      %%mm7,    %%mm0\n"  // B5G5R5B4 -> mm0
		
		"pmaddwd   YBG0BX,   %%mm7\n"  // ybB5+ygG5,ybB4 -> mm7
		"packssdw  %%mm3,    %%mm4\n"  // 32-bit scaled V3V2V1V0 -> mm4

		//----------------------------------------------------------------------
		
		"movq      %%mm4,    24%5\n"  // (V3V2V1V0)/256 -> mm4

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
		
		"movq      8%5,    %%mm6\n"  // 32-bit scaled Y3Y2Y1Y0 -> mm6
		"packssdw  %%mm7,    %%mm0\n"  // 32-bit scaled U7U6U5U4 -> mm0
		
		"movq      16%5,    %%mm4\n"  // 32-bit scaled U3U2U1U0 -> mm4
		"packuswb  %%mm2,    %%mm6\n"  // all 8 Y values -> mm6
		
		"movq      OFFSETBX, %%mm7\n"  // 128,128,128,128 -> mm7
		"paddd     %%mm5,    %%mm1\n"  // V7V6 -> mm1
		
		"paddw     %%mm7,    %%mm4\n"  // add offset to U3U2U1U0/256 
		"psrad     $15,      %%mm1\n"  // 32-bit scaled V7V6 -> mm1
		
		//----------------------------------------------------------------------

		"movq      %%mm6,    (%1)\n"     // store Y
		"packuswb  %%mm0,    %%mm4\n"    // all 8 U values -> mm4
		
		"movq      24%5,    %%mm5\n"  // 32-bit scaled V3V2V1V0 -> mm5
		"packssdw  %%mm1,    %%mm3\n"  // V7V6V5V4 -> mm3
		"paddw     %%mm7,    %%mm5\n"  // add offset to  V3V2V1V0
		"paddw     %%mm7,    %%mm3\n"  // add offset to  V7V6V5V4
		
		"packuswb  %%mm3,    %%mm5\n"  // ALL 8 V values -> mm5

		// pack U and V
		"movq      CLEARX, %%mm2\n"
		"pand      %%mm2, %%mm4\n"
		"pand      %%mm2, %%mm5\n"

		"packuswb  %%mm5, %%mm4\n" 

		"movd      %%mm4,    (%2)\n"
		"psrlq     $32,      %%mm4\n"
		"movd      %%mm4,    (%3)\n"

		"add       $24,      %0\n"
		"add       $8,       %1\n"
		"add       $4,       %2\n"
		"add       $4,       %3\n"
		
		"sub       $8,       %4\n"
  		"jnz       1b\n"
		"emms\n"

		:
		: "r" (rgb), "r" (lum), "r" (cr), "r" (cb), "m" (pixel), "m" (buf[0])
		);
}

void rgb2yuv32bit_mmx422_row(unsigned char* rgb, unsigned char* lum, unsigned char* cr,
							 unsigned char* cb, int pixel)
{
	unsigned int buf[8];

	// %5 = TEMP0
	// 8%5 = TEMPY
	// 16%5 = TEMPU
	// 24%5 = TEMPV

	__asm__ __volatile__ (
		"1:\n"

		// pack rgb
		// was:	"movq      (%0), %%mm1\n"  // load G2R2B1G1R1B0G0R0
		// ------------------------------
		// (uses: mm0, mm1)
		"movd      8(%0),    %%mm0\n"
		"psllq     $24,      %%mm0\n"  
  		"movd      4(%0),    %%mm1\n"
		"por       %%mm1,    %%mm0\n"
  		"psllq     $24,      %%mm0\n"
  		"movd      (%0),     %%mm1\n"
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
		"movd      20(%0),  %%mm1\n"
		"psllq     $24,     %%mm1\n"
		"movd      16(%0),  %%mm6\n"
		"por       %%mm6,   %%mm1\n"
		"psllq     $24,     %%mm1\n"
 		"movd      12(%0),  %%mm6\n"
		"por       %%mm6,   %%mm1\n"
  		"psllq     $8,      %%mm1\n"	  
   		"movd      8(%0),   %%mm6\n"
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
		
		"movq      %%mm6,   %5\n"  // R5B4G4R4 -> TEMP0
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
		"movd      28(%0),  %%mm1\n"
		"psllq     $24,     %%mm1\n"
		"movd      24(%0),  %%mm7\n"
		"por       %%mm7,   %%mm1\n"
		"psllq     $16,     %%mm1\n"
  		"movd      20(%0),  %%mm7\n"
		"psrlq     $8,      %%mm7\n"	  
		"por       %%mm7,   %%mm1\n"
		// ------------------------------

		"movq      %%mm1,    %%mm7\n"  // B7G7R7B6G6R6B5G5 -> mm1
		"psrad     $15,      %%mm6\n"  // 32-bit scaled U3U2 -> mm6

		"paddd     %%mm5,    %%mm3\n"  // V3V2 -> mm3
		"psllq     $16,      %%mm7\n"  // R7B6G6R6B5G500 -> mm7
		
		"movq      %%mm7,    %%mm5\n"  // R7B6G6R6B5G500 -> mm5
		"psrad     $15,      %%mm3\n"  // 32-bit scaled V3V2 -> mm3
		
		"movq      %%mm0,    8%5\n"  // 32-bit scaled Y3Y2Y1Y0 -> TEMPY

		"packssdw  %%mm6,    %%mm2\n"  // 32-bit scaled U3U2U1U0 -> mm2
		
		"movq      %5,    %%mm0\n"  // R5B4G4R4 -> mm0
		
		"punpcklbw ZEROSX,   %%mm7\n"  // B5G500 -> mm7
		"movq      %%mm0,    %%mm6\n"  // R5B4G4R4 -> mm6
		
		"movq      %%mm2,    16%5\n"  // 32-bit scaled U3U2U1U0 -> TEMPU
		"psrlq     $32,      %%mm0\n"  // 00R5B4 -> mm0
		
		"paddw     %%mm0,    %%mm7\n"  // B5G5R5B4 -> mm7
		"movq      %%mm6,    %%mm2\n"  // B5B4G4R4 -> mm2
		
		"pmaddwd   YR0GRX,   %%mm2\n"  // yrR5,ygG4+yrR4 -> mm2
		"movq      %%mm7,    %%mm0\n"  // B5G5R5B4 -> mm0
		
		"pmaddwd   YBG0BX,   %%mm7\n"  // ybB5+ygG5,ybB4 -> mm7
		"packssdw  %%mm3,    %%mm4\n"  // 32-bit scaled V3V2V1V0 -> mm4

		//----------------------------------------------------------------------
		
		"movq      %%mm4,    24%5\n"  // (V3V2V1V0)/256 -> mm4

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
		
		"movq      8%5,    %%mm6\n"  // 32-bit scaled Y3Y2Y1Y0 -> mm6
		"packssdw  %%mm7,    %%mm0\n"  // 32-bit scaled U7U6U5U4 -> mm0
		
		"movq      16%5,    %%mm4\n"  // 32-bit scaled U3U2U1U0 -> mm4
		"packuswb  %%mm2,    %%mm6\n"  // all 8 Y values -> mm6
		
		"movq      OFFSETBX, %%mm7\n"  // 128,128,128,128 -> mm7
		"paddd     %%mm5,    %%mm1\n"  // V7V6 -> mm1
		
		"paddw     %%mm7,    %%mm4\n"  // add offset to U3U2U1U0/256 
		"psrad     $15,      %%mm1\n"  // 32-bit scaled V7V6 -> mm1
		
		//----------------------------------------------------------------------

		"movq      %%mm6,    (%1)\n"     // store Y

		"packuswb  %%mm0,    %%mm4\n"    // all 8 U values -> mm4
		"movq      24%5,    %%mm5\n"  // 32-bit scaled V3V2V1V0 -> mm5

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

		"add       $32,      %0\n"
		"add       $8,       %1\n"
		"add       $4,       %2\n"
		"add       $4,       %3\n"
		
		"sub       $8,       %4\n"
  		"jnz       1b\n"

		"emms\n"

		:
		: "r" (rgb), "r" (lum), "r" (cr), "r" (cb), "m" (pixel), "m" (buf[0])

		);
}

void rgb2y24bit_mmx_row(unsigned char* rgbSource, unsigned char* lum, int pixel)
{
	unsigned int buf[4];

	// %3 = TEMP0
	// 8%3 = TEMPY

	__asm__ __volatile__ (
		"1:\n"

		"movq      (%0), %%mm1\n"  // load G2R2B1G1R1B0G0R0
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

		"movq      8(%0),%%mm1\n"     // load G2R2B1G1R1B0G0R0

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
		
		"movq      %%mm6,   %3\n"  // R5B4G4R4 -> TEMP0
		"movq      %%mm3,   %%mm6\n"  // R3B2G2R2 -> mm6
		
		"paddd     %%mm5,   %%mm1\n"  // Y3Y2 -> mm1
		"movq      %%mm7,   %%mm5\n"  // B3G3R3B2 -> mm5
		"psrad     $15,     %%mm1\n"  // 32-bit scaled Y3Y2 -> mm1
		
		"packssdw  %%mm1,   %%mm0\n"  // Y3Y2Y1Y0 -> mm0

		//----------------------------------------------------------------------

		"movq      16(%0), %%mm1\n"  // B7G7R7B6G6R6B5G5 -> mm7
		
		"movq      %%mm1,    %%mm7\n"  // B7G7R7B6G6R6B5G5 -> mm1
		
		"psllq     $16,      %%mm7\n"  // R7B6G6R6B5G500 -> mm7
		
		"movq      %%mm7,    %%mm5\n"  // R7B6G6R6B5G500 -> mm5
		
		"movq      %%mm0,    8%3\n"  // 32-bit scaled Y3Y2Y1Y0 -> TEMPY
		
		"movq      %3,    %%mm0\n"  // R5B4G4R4 -> mm0
		
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
		
		"movq      8%3,    %%mm6\n"  // 32-bit scaled Y3Y2Y1Y0 -> mm6
  		"packuswb  %%mm2,    %%mm6\n"  // all 8 Y values -> mm6
		
		//----------------------------------------------------------------------

		"movq      %%mm6,  (%1)\n"     // store Y
		
		"add       $24,    %0\n"
		"add       $8,     %1\n"
		
		"sub       $8,     %2\n"
  		"jnz       1b\n"
		"emms\n"

		:
		: "r" (rgbSource), "r" (lum), "m" (pixel), "m" (buf[0])
		
		);
}

void rgb2y32bit_mmx_row(unsigned char* rgbSource, unsigned char* lum, int pixel)
{
	unsigned int buf[4];

	// %3 = TEMP0
	// 8%3 = TEMPY

	__asm__ __volatile__ (
		"1:\n"

		// pack rgb
		// was:	"movq      (%0), %%mm1\n"  // load G2R2B1G1R1B0G0R0
		// ------------------------------
		// (uses: mm0, mm1)
		"movd      8(%0),    %%mm0\n"
		"psllq     $24,      %%mm0\n"  
  		"movd      4(%0),    %%mm1\n"
		"por       %%mm1,    %%mm0\n"
  		"psllq     $24,      %%mm0\n"
  		"movd      (%0),     %%mm1\n"
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
		"movd      20(%0),  %%mm1\n"
		"psllq     $24,     %%mm1\n"
		"movd      16(%0),  %%mm6\n"
		"por       %%mm6,   %%mm1\n"
		"psllq     $24,     %%mm1\n"
 		"movd      12(%0),  %%mm6\n"
		"por       %%mm6,   %%mm1\n"
  		"psllq     $8,      %%mm1\n"	  
   		"movd      8(%0),   %%mm6\n"
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
		
		"movq      %%mm6,   %3\n"  // R5B4G4R4 -> TEMP0
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
		"movd      28(%0),  %%mm1\n"
		"psllq     $24,     %%mm1\n"
		"movd      24(%0),  %%mm7\n"
		"por       %%mm7,   %%mm1\n"
		"psllq     $16,     %%mm1\n"
  		"movd      20(%0),  %%mm7\n"
		"psrlq     $8,      %%mm7\n"	  
		"por       %%mm7,   %%mm1\n"
		// ------------------------------
		
		"movq      %%mm1,    %%mm7\n"  // B7G7R7B6G6R6B5G5 -> mm1
		
		"psllq     $16,      %%mm7\n"  // R7B6G6R6B5G500 -> mm7
		
		"movq      %%mm7,    %%mm5\n"  // R7B6G6R6B5G500 -> mm5
		
		"movq      %%mm0,    8%3\n"  // 32-bit scaled Y3Y2Y1Y0 -> TEMPY
		
		"movq      %3,    %%mm0\n"  // R5B4G4R4 -> mm0
		
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
		
		"movq      8%3,    %%mm6\n"  // 32-bit scaled Y3Y2Y1Y0 -> mm6
  		"packuswb  %%mm2,    %%mm6\n"  // all 8 Y values -> mm6
		
		//----------------------------------------------------------------------

		"movq      %%mm6,  (%1)\n"     // store Y
		
		"add       $32,    %0\n"
		"add       $8,     %1\n"
		
		"sub       $8,     %2\n"
  		"jnz       1b\n"
		"emms\n"

		:
		: "r" (rgbSource), "r" (lum), "r" (pixel), "m" (buf[0])
		
		);
}

#endif
// INTEL

