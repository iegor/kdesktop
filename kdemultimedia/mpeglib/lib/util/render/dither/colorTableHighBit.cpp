/*
  colorTables for 16,32 Bit depth
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "colorTableHighBit.h"

//#define INTERPOLATE


/*
 * Erik Corry's multi-byte dither routines.
 *
 * The basic idea is that the Init generates all the necessary tables.
 * The tables incorporate the information about the layout of pixels
 * in the XImage, so that it should be able to cope with 15-bit, 16-bit
 * 24-bit (non-packed) and 32-bit (10-11 bits per color!) screens.
 * At present it cannot cope with 24-bit packed mode, since this involves
 * getting down to byte level again. It is assumed that the bits for each
 * color are contiguous in the longword.
 * 
 * Writing to memory is done in shorts or ints. (Unfortunately, short is not
 * very fast on Alpha, so there is room for improvement here). There is no
 * dither time check for overflow - instead the tables have slack at
 * each end. This is likely to be faster than an 'if' test as many modern
 * architectures are really bad at ifs. Potentially, each '&&' causes a 
 * pipeline flush!
 *
 * There is no shifting and fixed point arithmetic, as I really doubt you
 * can see the difference, and it costs. This may be just my bias, since I
 * heard that Intel is really bad at shifting.
 */


/*
 * How many 1 bits are there in the PIXVALword.
 * Low performance, do not call often.
 */
static int number_of_bits_set(unsigned PIXVAL a) {
    if(!a) return 0;
    if(a & 1) return 1 + number_of_bits_set(a >> 1);
    return(number_of_bits_set(a >> 1));
}



/*
 * How many 0 bits are there at most significant end of PIXVALword.
 * Low performance, do not call often.
 */
static int free_bits_at_top(unsigned PIXVAL a) {
      /* assume char is 8 bits */
    if(!a) return sizeof(unsigned PIXVAL) * 8;
        /* assume twos complement */
    if(((PIXVAL)a) < 0l) return 0;
    return 1 + free_bits_at_top ( a << 1);
}

/*
 * How many 0 bits are there at least significant end of PIXVALword.
 * Low performance, do not call often.
 */
static int free_bits_at_bottom(unsigned PIXVAL a) {
      /* assume char is 8 bits */
    if(!a) return sizeof(unsigned PIXVAL) * 8;
    if(((PIXVAL)a) & 1l) return 0;
    return 1 + free_bits_at_bottom ( a >> 1);
}



ColorTableHighBit::ColorTableHighBit(int bpp,unsigned int redMask,
				     unsigned int greenMask,
				     unsigned int blueMask) {
  this->bpp=bpp;
  this->redMask=redMask;
  this->greenMask=greenMask;
  this->blueMask=blueMask;

  colortab = new TABTYPE[5*256];
  
  Cr_r_tab = &colortab[0*256];
  Cr_g_tab = &colortab[1*256];
  Cb_g_tab = &colortab[2*256];
  Cb_b_tab = &colortab[3*256];
  L_tab    = &colortab[4*256];
  
  rgb_2_pix = new PIXVAL [3*768];

  r_2_pix_alloc = &rgb_2_pix[0*768];
  g_2_pix_alloc = &rgb_2_pix[1*768];
  b_2_pix_alloc = &rgb_2_pix[2*768];
  
  initHighColor(bpp>=24,redMask,greenMask,blueMask);
  
}


ColorTableHighBit::~ColorTableHighBit() {
  delete colortab;
  delete rgb_2_pix;
}

/*
 *--------------------------------------------------------------
 *
 * InitColor16Dither --
 *
 *	To get rid of the multiply and other conversions in color
 *	dither, we use a lookup table.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The lookup tables are initialized.
 *
 *--------------------------------------------------------------
 */

void ColorTableHighBit::initHighColor(int thirty2,unsigned int redMask,
				      unsigned int greenMask,
				      unsigned int blueMask) {
  
  unsigned PIXVAL red_mask = redMask;
  unsigned PIXVAL green_mask =greenMask;
  unsigned PIXVAL blue_mask = blueMask;

  int CR, CB, i;
    

  for (i=0; i<256; i++) {
    L_tab[i] = i;
    if (gammaCorrectFlag) {
      L_tab[i] = (TABTYPE)GAMMA_CORRECTION(i);
    }
    
    CB = CR = i;
    
    if (chromaCorrectFlag) {
      CB -= 128; 
      CB = CHROMA_CORRECTION128(CB);
      CR -= 128;
      CR = CHROMA_CORRECTION128(CR);
    } else {
      CB -= 128; CR -= 128;
    }
/* was
      Cr_r_tab[i] =  1.596 * CR;
      Cr_g_tab[i] = -0.813 * CR;
      Cb_g_tab[i] = -0.391 * CB;   
      Cb_b_tab[i] =  2.018 * CB;
  but they were just messed up.
  Then was (_Video Deymstified_):
      Cr_r_tab[i] =  1.366 * CR;
      Cr_g_tab[i] = -0.700 * CR;
      Cb_g_tab[i] = -0.334 * CB;   
      Cb_b_tab[i] =  1.732 * CB;
  but really should be:
   (from ITU-R BT.470-2 System B, G and SMPTE 170M )
*/
      Cr_r_tab[i] = (TABTYPE) ( (0.419/0.299) * CR  );
      Cr_g_tab[i] = (TABTYPE) ( -(0.299/0.419) * CR );
      Cb_g_tab[i] = (TABTYPE) ( -(0.114/0.331) * CB ); 
      Cb_b_tab[i] = (TABTYPE) (  (0.587/0.331) * CB );

/*
  though you could argue for:
    SMPTE 240M
      Cr_r_tab[i] =  (0.445/0.212) * CR;
      Cr_g_tab[i] = -(0.212/0.445) * CR;
      Cb_g_tab[i] = -(0.087/0.384) * CB; 
      Cb_b_tab[i] =  (0.701/0.384) * CB;
    FCC 
      Cr_r_tab[i] =  (0.421/0.30) * CR;
      Cr_g_tab[i] = -(0.30/0.421) * CR;
      Cb_g_tab[i] = -(0.11/0.331) * CB; 
      Cb_b_tab[i] =  (0.59/0.331) * CB;
    ITU-R BT.709 
      Cr_r_tab[i] =  (0.454/0.2125) * CR;
      Cr_g_tab[i] = -(0.2125/0.454) * CR;
      Cb_g_tab[i] = -(0.0721/0.386) * CB; 
      Cb_b_tab[i] =  (0.7154/0.386) * CB;
*/
    }

    /* 
     * Set up entries 0-255 in rgb-to-pixel value tables.
     */
    for (i = 0; i < 256; i++) {
      r_2_pix_alloc[i + 256] = i >> (8 - number_of_bits_set(red_mask));
      r_2_pix_alloc[i + 256] <<= free_bits_at_bottom(red_mask);
      g_2_pix_alloc[i + 256] = i >> (8 - number_of_bits_set(green_mask));
      g_2_pix_alloc[i + 256] <<= free_bits_at_bottom(green_mask);
      b_2_pix_alloc[i + 256] = i >> (8 - number_of_bits_set(blue_mask));
      b_2_pix_alloc[i + 256] <<= free_bits_at_bottom(blue_mask);
      /*
       * If we have 16-bit output depth, then we double the value
       * in the top word. This means that we can write out both
       * pixels in the pixel doubling mode with one op. It is 
       * harmless in the normal case as storing a 32-bit value
       * through a short pointer will lose the top bits anyway.
       * A similar optimisation for Alpha for 64 bit has been
       * prepared for, but is not yet implemented.
       */
      if(!thirty2) {
	r_2_pix_alloc[i + 256] |= (r_2_pix_alloc[i + 256]) << 16;
	g_2_pix_alloc[i + 256] |= (g_2_pix_alloc[i + 256]) << 16;
	b_2_pix_alloc[i + 256] |= (b_2_pix_alloc[i + 256]) << 16;

      }
#ifdef SIXTYFOUR_BIT
      if(thirty2) {

	r_2_pix_alloc[i + 256] |= (r_2_pix_alloc[i + 256]) << 32;
	g_2_pix_alloc[i + 256] |= (g_2_pix_alloc[i + 256]) << 32;
	b_2_pix_alloc[i + 256] |= (b_2_pix_alloc[i + 256]) << 32;

      }
#endif
    }

    /*
     * Spread out the values we have to the rest of the array so that
     * we do not need to check for overflow.
     */
    for (i = 0; i < 256; i++) {
      r_2_pix_alloc[i] = r_2_pix_alloc[256];
      r_2_pix_alloc[i+ 512] = r_2_pix_alloc[511];
      g_2_pix_alloc[i] = g_2_pix_alloc[256];
      g_2_pix_alloc[i+ 512] = g_2_pix_alloc[511];
      b_2_pix_alloc[i] = b_2_pix_alloc[256];
      b_2_pix_alloc[i+ 512] = b_2_pix_alloc[511];
    }

    r_2_pix = r_2_pix_alloc + 256;
    g_2_pix = g_2_pix_alloc + 256;
    b_2_pix = b_2_pix_alloc + 256;
}
