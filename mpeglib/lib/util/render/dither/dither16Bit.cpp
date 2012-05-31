/*
  dither 16 bit depth yuv images
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "dither16Bit.h"


Dither16Bit::Dither16Bit(unsigned int redMask,
			 unsigned int greenMask,unsigned int blueMask) {


  colorTableHighBit=new ColorTableHighBit(16,redMask,greenMask,blueMask);
  L_tab=colorTableHighBit->getL_tab();
  Cr_r_tab=colorTableHighBit->getCr_r_tab();
  Cr_g_tab=colorTableHighBit->getCr_g_tab();
  Cb_g_tab=colorTableHighBit->getCb_g_tab();
  Cb_b_tab=colorTableHighBit->getCb_b_tab();
  
  r_2_pix=colorTableHighBit->getr_2_pix();
  g_2_pix=colorTableHighBit->getg_2_pix();
  b_2_pix=colorTableHighBit->getb_2_pix();

}


Dither16Bit::~Dither16Bit() {
  delete colorTableHighBit;
}


/*
 *--------------------------------------------------------------
 *
 * Color16DitherImage --
 *
 *	Converts image into 16 bit color.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

void Dither16Bit::ditherImageColor16(unsigned char* lum, 
				     unsigned char* cr, 
				     unsigned char* cb,
				     unsigned char* out,
				     int rows,
				     int cols,
				     int offset) {
  
  int L, CR, CB;
  unsigned short *row1, *row2;
  unsigned char *lum2;
  int x, y;
  int cr_r;
  int cr_g;
  int cb_g;
  int cb_b;
  int cols_2 = cols/2;
  
  row1 = (unsigned short *)out;
  row2=row1+cols_2+cols_2+offset;                   // start of second row 

  offset=2*offset+cols_2+cols_2;

  lum2 = lum + cols_2 + cols_2;

  
  for (y=0; y<rows; y+=2) {
    for (x=0; x<cols_2; x++) {
      int R, G, B;
      
      CR = *cr++;
      CB = *cb++;
      cr_r = Cr_r_tab[CR];
      cr_g = Cr_g_tab[CR];
      cb_g = Cb_g_tab[CB];
      cb_b = Cb_b_tab[CB];
      
      L = L_tab[(int) *lum++];
      
      R = L + cr_r;
      G = L + cr_g + cb_g;
      B = L + cb_b;
      
      *row1++ = (r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);
      
      
#ifdef INTERPOLATE
      if(x != cols_2 - 1) {
	CR = (CR + *cr) >> 1;
	CB = (CB + *cb) >> 1;
	cr_r = Cr_r_tab[CR];
	cr_g = Cr_g_tab[CR];
	cb_g = Cb_g_tab[CB];
	cb_b = Cb_b_tab[CB];
      }
#endif
      
      L = L_tab[(int) *lum++];
      
      R = L + cr_r;
      G = L + cr_g + cb_g;
      B = L + cb_b;
      
      *row1++ = (r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);
      
      /*
       * Now, do second row.
       */
#ifdef INTERPOLATE
      if(y != rows - 2) {
	CR = (CR + *(cr + cols_2 - 1)) >> 1;
	CB = (CB + *(cb + cols_2 - 1)) >> 1;
	cr_r = Cr_r_tab[CR];
	cr_g = Cr_g_tab[CR];
	cb_g = Cb_g_tab[CB];
	cb_b = Cb_b_tab[CB];
      }
#endif
      
      L = L_tab[(int) *lum2++];
      R = L + cr_r;
      G = L + cr_g + cb_g;
      B = L + cb_b;
      
      *row2++ = (r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);
      
      L = L_tab[(int) *lum2++];
      R = L + cr_r;
      G = L + cr_g + cb_g;
      B = L + cb_b;
      
      *row2++ = (r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);
    }
    /*
     * These values are at the start of the next line, (due
     * to the ++'s above),but they need to be at the start
     * of the line after that.
     */
    lum += cols_2 + cols_2;
    lum2 += cols_2 + cols_2;
    row1 += offset;
    row2 += offset;
  }
}


/*
 * Erik Corry's pixel doubling routines for 15/16/24/32 bit screens.
 */


/*
 *--------------------------------------------------------------
 *
 * Twox2Color16DitherImage --
 *
 *	Converts image into 16 bit color at double size.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

/*
 * In this function I make use of a nasty trick. The tables have the lower
 * 16 bits replicated in the upper 16. This means I can write ints and get
 * the horisontal doubling for free (almost).
 */

void Dither16Bit::ditherImageTwox2Color16(unsigned char* lum,
					  unsigned char* cr,
					  unsigned char* cb,
					  unsigned char* out,
					  int rows,
					  int cols,
					  int mod) {
  int L, CR, CB;
  unsigned int *row1 = (unsigned int *)out;
  unsigned int *row2 = row1 + cols + mod/2;
  unsigned int *row3 = row2 + cols + mod/2;
  unsigned int *row4 = row3 + cols + mod/2;
  unsigned char *lum2;
  int x, y;
  int cr_r;
  int cr_g;
  int cb_g;
  int cb_b;
  int cols_2 = cols/2;
  
  lum2 = lum + cols_2 + cols_2;
  for (y=0; y<rows; y+=2) {
    for (x=0; x<cols_2; x++) {
      int R, G, B;
      int t;
      
      CR = *cr++;
      CB = *cb++;
      cr_r = Cr_r_tab[CR];
      cr_g = Cr_g_tab[CR];
      cb_g = Cb_g_tab[CB];
      cb_b = Cb_b_tab[CB];
      
      L = L_tab[(int) *lum++];
      
      R = L + cr_r;
      G = L + cr_g + cb_g;
      B = L + cb_b;
      
      t = (r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);
      row1[0] = t;
      row1++;
      row2[0] = t;
      row2++;
      
      // INTERPOLATE
      if(x != cols_2 - 1) {
	CR = (CR + *cr) >> 1;
	CB = (CB + *cb) >> 1;
	cr_r = Cr_r_tab[CR];
	cr_g = Cr_g_tab[CR];
	cb_g = Cb_g_tab[CB];
	cb_b = Cb_b_tab[CB];
      }
      // end
      
      L = L_tab[(int) *lum++];
      
      R = L + cr_r;
      G = L + cr_g + cb_g;
      B = L + cb_b;
      
      t = (r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);
      row1[0] = t;
      row1++;
      row2[0] = t;
      row2++;
      
      /*
       * Now, do second row.
       */
      // INTERPOLATE
      if(y != rows - 2) {
	CR = (CR + *(cr + cols_2 - 1)) >> 1;
	CB = (CB + *(cb + cols_2 - 1)) >> 1;
	cr_r = Cr_r_tab[CR];
	cr_g = Cr_g_tab[CR];
	cb_g = Cb_g_tab[CB];
	cb_b = Cb_b_tab[CB];
      }
      // end
      
      L = L_tab[(int) *lum2++];
      R = L + cr_r;
      G = L + cr_g + cb_g;
      B = L + cb_b;
      
      t = (r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);
      row3[0] = t;
      row3++;
      row4[0] = t;
      row4++;
      
      L = L_tab[(int) *lum2++];
      R = L + cr_r;
      G = L + cr_g + cb_g;
      B = L + cb_b;
      
      t = (r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);
      row3[0] = t;
      row3++;
      row4[0] = t;
      row4++;
    }
    lum += cols_2 + cols_2;
    lum2 += cols_2 + cols_2;
    row1 += 6 * cols_2 + 2*mod;
    row3 += 6 * cols_2 + 2*mod;
    row2 += 6 * cols_2 + 2*mod;
    row4 += 6 * cols_2 + 2*mod;
  }
}
