/*
  dither 32 bit depth yuv images
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "dither32Bit.h"


#define doRow(row,Lum)  *row++=(local_r_2_pix[Lum] | \
 local_g_2_pix[Lum] | local_b_2_pix[Lum])


Dither32Bit::Dither32Bit(unsigned int redMask,
			 unsigned int greenMask,unsigned int blueMask) {


  colorTableHighBit=new ColorTableHighBit(32,redMask,greenMask,blueMask);
  L_tab=colorTableHighBit->getL_tab();
  Cr_r_tab=colorTableHighBit->getCr_r_tab();
  Cr_g_tab=colorTableHighBit->getCr_g_tab();
  Cb_g_tab=colorTableHighBit->getCb_g_tab();
  Cb_b_tab=colorTableHighBit->getCb_b_tab();
  
  r_2_pix=colorTableHighBit->getr_2_pix();
  g_2_pix=colorTableHighBit->getg_2_pix();
  b_2_pix=colorTableHighBit->getb_2_pix();

}


Dither32Bit::~Dither32Bit() {
  delete colorTableHighBit;
}


void Dither32Bit::ditherImageColor32(unsigned char* lum, 
				     unsigned char* cr, 
				     unsigned char* cb,
				     unsigned char* out,
				     int rows,
				     int cols,
				     int mod) {
  
  int L;
  int n;
  int rowWork;
  int colWork;
  
  unsigned int *row1, *row2;
  unsigned char *lum2;
  PIXVAL* local_r_2_pix;
  PIXVAL* local_g_2_pix;
  PIXVAL* local_b_2_pix;
  
  row1 = (unsigned int *)out;
  
  row2 = row1+cols+mod;
  lum2 = lum+cols;
  
  // because the width/height are a multiply of a macroblocksize
  // cols/rows always are even
  colWork=cols>>1;
  rowWork=rows>>1;
  mod=cols+2*mod;
  
  while(rowWork--) {
    n=colWork;
    while(n--) {
      
      local_r_2_pix=r_2_pix+Cr_r_tab[*cr];
      local_g_2_pix=g_2_pix+Cr_g_tab[*cr++] + Cb_g_tab[*cb];
      local_b_2_pix=b_2_pix+Cb_b_tab[*cb++];
      
      L = L_tab[*lum++];
      doRow(row1,L);
	
      L = L_tab[*lum++];
      doRow(row1,L);
	
      L = L_tab [*lum2++];
      doRow(row2,L);
	
      L = L_tab [*lum2++];
      doRow(row2,L);
	
      
    }
    row2 += mod;
    lum += cols;
    lum2 += cols;
    row1 += mod;
    
  }

}

/*
 *--------------------------------------------------------------
 *
 * Twox2Color32 --
 *
 *	Converts image into 24/32 bit color.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

void Dither32Bit::ditherImageTwox2Color32(unsigned char* lum,
					  unsigned char* cr,
					  unsigned char* cb,
					  unsigned char* out,
					  int rows,
					  int cols,
					  int mod) {
  int L, CR, CB;
  unsigned PIXVAL *row1 = (unsigned PIXVAL *)out;
  unsigned PIXVAL *row2 = row1 + cols * ONE_TWO + mod;
  unsigned PIXVAL *row3 = row2 + cols * ONE_TWO + mod;
  unsigned PIXVAL *row4 = row3 + cols * ONE_TWO + mod;
  unsigned char *lum2;
  int x, y;
  int cr_r;
  int cr_g;
  int cb_g;
  int cb_b;
  int cols_2 = cols/2;
  int loffset = ONE_TWO * 6 *cols_2 + 4*mod ;
  
  lum2 = lum + cols_2 + cols_2;
  for (y=0; y<rows; y+=2) {
    for (x=0; x<cols_2; x++) {
      int R, G, B;
      PIXVAL t; 
      
      CR = *cr++;
      CB = *cb++;
      cr_r = Cr_r_tab[CR];
      cr_g = Cr_g_tab[CR];
      cb_g = Cb_g_tab[CB];
      cb_b = Cb_b_tab[CB];
      
      L = L_tab[ (int) *lum++];
      
      R = L + cr_r;
      G = L + cr_g + cb_g;
      B = L + cb_b;
      
      t = (r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);
      row1[0] = t;
      row2[0] = t;
#ifndef SIXTYFOUR_BIT
      row1[1] = t;
      row2[1] = t;
#endif
      row1 += ONE_TWO;
      row2 += ONE_TWO;
      
      /* INTERPOLATE is now standard */
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
      /* end INTERPOLATE */
      
      L = L_tab[ (int) *lum++];
      
      R = L + cr_r;
      G = L + cr_g + cb_g;
      B = L + cb_b;
      
      t = (r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);
      row1[0] = t;
      row2[0] = t;
#ifndef SIXTYFOUR_BIT
      row1[1] = t;
      row2[1] = t;
#endif
      row1 += ONE_TWO;
      row2 += ONE_TWO;
      
      /*
       * Now, do second row.
       */
      /* INTERPOLATE is now standard */
      // INTERPOLATE
      if(y != rows - 2) {
	CR = (unsigned int) (CR + *(cr + cols_2 - 1)) >> 1;
	CB = (unsigned int) (CB + *(cb + cols_2 - 1)) >> 1;
	cr_r = Cr_r_tab[CR];
	cr_g = Cr_g_tab[CR];
	cb_g = Cb_g_tab[CB];
	cb_b = Cb_b_tab[CB];
      }
      // end
      /* endif */
      L = L_tab[ (int) *lum2++];
      R = L + cr_r;
      G = L + cr_g + cb_g;
      B = L + cb_b;
      
      t = (r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);
      row3[0] = t;
      row4[0] = t;
#ifndef SIXTYFOUR_BIT
      row3[1] = t;
      row4[1] = t;
#endif
      row3 += ONE_TWO;
      row4 += ONE_TWO;
      
      L = L_tab[(int) *lum2++];
      R = L + cr_r;
      G = L + cr_g + cb_g;
      B = L + cb_b;
      
      t = (r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);
      row3[0] = t;
      row4[0] = t;
#ifndef SIXTYFOUR_BIT
      row3[1] = t;
      row4[1] = t;
#endif
      row3 += ONE_TWO;
      row4 += ONE_TWO;
    }
    lum += cols_2 + cols_2;
    lum2 += cols_2 + cols_2;
    
    row1 += loffset;
    row3 += loffset;
    row2 += loffset;
    row4 += loffset;
  }
}
