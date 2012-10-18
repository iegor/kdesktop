/*
  copyfunctions implementation in mmx
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __COPYFUNCTIONS_MMX_H
#define __COPYFUNCTIONS_MMX_H

#include "copyFunctions_asm.h"
#include "../util/mmx/mmx.h"


class CopyFunctions_MMX : public CopyFunctions_ASM {
  
  int lmmx;

 public:
  CopyFunctions_MMX();
  ~CopyFunctions_MMX();

  int support();

#if defined (__GNUC__) && defined (INTEL)

  void startNOFloatSection();
  void endNOFloatSection();


  void copy8_byte(unsigned char* source1,
		  unsigned char* dest,int inc);

  void copy8_src1linear_crop(short* source1,
			     unsigned char* dest,int inc);

  void copy8_div2_nocrop(unsigned char* source1,
			 unsigned char* source2,
			 unsigned char* dest,int inc);

  void copy8_div2_destlinear_nocrop(unsigned char* source1,
				    unsigned char* source2,
				    unsigned char* dest,int inc);
  
  void copy16_div2_destlinear_nocrop(unsigned char* source1,
				     unsigned char* source2,
				     unsigned char* dest,int inc);

  void copy8_src2linear_crop(unsigned char* source1,
			     short int* source2,
			     unsigned char* dest,int inc);

  void copy8_div2_src3linear_crop(unsigned char* source1,
				  unsigned char* source2,
				  short int* source3,
				  unsigned char* dest,int inc);
  
#endif

};
#endif

