/*
  colorTables for 16,32 Bit depth
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __COLORTABLEHIGHBIT_H
#define __COLORTABLEHIGHBIT_H

#include "ditherDef.h"




class ColorTableHighBit {

  TABTYPE *L_tab;
  TABTYPE *Cr_r_tab;
  TABTYPE *Cr_g_tab;
  TABTYPE *Cb_g_tab;
  TABTYPE *Cb_b_tab;
  TABTYPE *colortab;
  
  
  PIXVAL *r_2_pix;
  PIXVAL *g_2_pix;
  PIXVAL *b_2_pix;
  PIXVAL *rgb_2_pix;

  PIXVAL *r_2_pix_alloc;
  PIXVAL *g_2_pix_alloc;
  PIXVAL *b_2_pix_alloc;



  // init stuff
  int bpp;
  // colorMask
  unsigned int redMask;
  unsigned int greenMask;
  unsigned int blueMask;

 public:
  ColorTableHighBit(int bpp,unsigned int redMask,
		    unsigned int greenMask,unsigned int blueMask);
  ~ColorTableHighBit();

  inline TABTYPE* getL_tab()        { return L_tab     ; }
  inline TABTYPE* getCr_r_tab()     { return Cr_r_tab  ; }
  inline TABTYPE* getCr_g_tab()     { return Cr_g_tab  ; }
  inline TABTYPE* getCb_g_tab()     { return Cb_g_tab  ; }
  inline TABTYPE* getCb_b_tab()     { return Cb_b_tab  ; }

  
  inline PIXVAL* getr_2_pix()       { return r_2_pix   ; }
  inline PIXVAL* getg_2_pix()       { return g_2_pix   ; }
  inline PIXVAL* getb_2_pix()       { return b_2_pix   ; }



 private:
  void initHighColor(int thirty2,unsigned int redMask,
		     unsigned int greenMask,unsigned int blueMask);

};
#endif
