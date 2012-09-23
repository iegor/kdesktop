/*
  colorTables for 8 Bit depth
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __COLORTABLE8BIT_H
#define __COLORTABLE8BIT_H

#include "ditherDef.h"


class ColorTable8Bit {
  
  // Arrays holding quantized value ranged for lum, cr, and cb. 
  // (used for 8 Bit)
  
  int* lum_values;
  int* cr_values;
  int* cb_values;
  



  TABTYPE *L_tab;
  TABTYPE *Cr_r_tab;
  TABTYPE *Cr_g_tab;
  TABTYPE *Cb_g_tab;
  TABTYPE *Cb_b_tab;
  TABTYPE *colortab;
 

 public:
  ColorTable8Bit();
  ~ColorTable8Bit();

  inline int* getLumValues() { return lum_values; }
  inline int* getCrValues()  { return cr_values; }
  inline int* getCbValues() { return cb_values; }
  
  void ConvertColor(unsigned int l, unsigned int cr, unsigned int cb,
		    unsigned char* r, unsigned char* g, unsigned char* b);


 private:
  void init8BitColor();


};
#endif
