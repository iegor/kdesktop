/*
  colorTables for 8 Bit depth
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "colorTable8Bit.h"


ColorTable8Bit::ColorTable8Bit() {

  lum_values = new int[LUM_RANGE];
  cr_values = new int[CR_RANGE];
  cb_values = new int[CB_RANGE];


  /* We can exploit cache by allocating contiguous blocks */
  
  colortab = new TABTYPE[5*256];
  
  Cr_r_tab = &colortab[0*256];
  Cr_g_tab = &colortab[1*256];
  Cb_g_tab = &colortab[2*256];
  Cb_b_tab = &colortab[3*256];
  L_tab    = &colortab[4*256];

  init8BitColor();
} 


ColorTable8Bit::~ColorTable8Bit() {
  delete lum_values;
  delete cr_values;
  delete cb_values;
  delete colortab;
}


void ColorTable8Bit::init8BitColor() {
  int i;



  for (i=0; i<LUM_RANGE; i++) {
    lum_values[i]  = ((i * 256) / (LUM_RANGE)) + (256/(LUM_RANGE*2));
    L_tab[i] = lum_values[i];
    if (gammaCorrectFlag) {
      L_tab[i] = GAMMA_CORRECTION(L_tab[i]);
    }

  }

  
  for (i=0; i<CR_RANGE; i++) {
    register double tmp;
    if (chromaCorrectFlag) {
      tmp = ((i * 256) / (CR_RANGE)) + (256/(CR_RANGE*2));
      Cr_r_tab[i]=(TABTYPE) ((0.419/0.299)*CHROMA_CORRECTION128D(tmp-128.0));
      Cr_g_tab[i]=(TABTYPE) (-(0.299/0.419)*CHROMA_CORRECTION128D(tmp-128.0));
      cr_values[i] = CHROMA_CORRECTION256(tmp);
    } else {
      tmp = ((i * 256) / (CR_RANGE)) + (256/(CR_RANGE*2));
      Cr_r_tab[i] = (TABTYPE)  ((0.419/0.299) * (tmp - 128.0));
      Cr_g_tab[i] = (TABTYPE) (-(0.299/0.419) * (tmp - 128.0));
      cr_values[i] = (int) tmp;
    }
  }

  
  for (i=0; i<CB_RANGE; i++) {
    register double tmp;
    if (chromaCorrectFlag) {
      tmp = ((i * 256) / (CB_RANGE)) + (256/(CB_RANGE*2));
      Cb_g_tab[i]=(TABTYPE) (-(0.114/0.331)*CHROMA_CORRECTION128D(tmp-128.0));
      Cb_b_tab[i]=(TABTYPE) ((0.587/0.331)*CHROMA_CORRECTION128D(tmp-128.0));
      cb_values[i] = CHROMA_CORRECTION256(tmp);
    } else {
      tmp = ((i * 256) / (CB_RANGE)) + (256/(CB_RANGE*2));
      Cb_g_tab[i] = (TABTYPE) (-(0.114/0.331) * (tmp - 128.0));
      Cb_b_tab[i] = (TABTYPE) ((0.587/0.331) * (tmp - 128.0));
      cb_values[i] = (int) tmp;
    }
  }
}



/*
 *--------------------------------------------------------------
 *
 * ConvertColor --
 *
 *      Given a l, cr, cb tuple, converts it to r,g,b.
 *
 * Results:
 *      r,g,b values returned in pointers passed as parameters.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */
void ColorTable8Bit::ConvertColor(unsigned int l, unsigned int cr, 
				  unsigned int cb, unsigned char* r, 
				  unsigned char* g, unsigned char* b) {

  double fl, fr, fg, fb;

  /*
   * Old method w/o lookup table
   *
   * fl = 1.164*(((double) l)-16.0);
   * fcr =  ((double) cr) - 128.0;
   * fcb =  ((double) cb) - 128.0;
   *
   * fr = fl + (1.366 * fcr);
   * fg = fl - (0.700 * fcr) - (0.334 * fcb);
   * fb = fl + (1.732 * fcb);
   */
  
  fl = L_tab[l];

  fr = fl + Cr_r_tab[cr];
  fg = fl + Cr_g_tab[cr] + Cb_g_tab[cb];
  fb = fl + Cb_b_tab[cb];

  if (fr < 0.0) fr = 0.0;
  else if (fr > 255.0) fr = 255.0;

  if (fg < 0.0) fg = 0.0;
  else if (fg > 255.0) fg = 255.0;

  if (fb < 0.0) fb = 0.0;
  else if (fb > 255.0) fb = 255.0;

  *r = (unsigned char) fr;
  *g = (unsigned char) fg;
  *b = (unsigned char) fb;

}
