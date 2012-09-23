/*
  stores heavily used copy functions (makes mmx support easier)
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __COPYFUNCTIONS_H
#define __COPYFUNCTIONS_H

#define MAX_NEG_CROP 32768
#define NUM_CROP_ENTRIES (2048+2*MAX_NEG_CROP)

#include <iostream>
#include "../util/mmx/mmx.h"

extern "C" {
#include <string.h>
}

#include "copyFunctions_mmx.h"


class CopyFunctions {

  unsigned char *cm;
  int lmmx;
  unsigned char* cropTbl;
  CopyFunctions_ASM* copyFunctions_asm;

 public:
  CopyFunctions();
  ~CopyFunctions();

  /**
     We make sure, that during the whole construcion block
     we never (!!!) do float operations, thus we move
     the time consumin emms call really at the end of
     the whole reconstrucion/motion compensation
  */

  void startNOFloatSection();
  void endNOFloatSection();

  void copy8_byte(unsigned char* source1,
		  unsigned char* dest,int inc);

  void copy8_word(unsigned short* source1,
		  unsigned short* dest,int inc);


  void copy8_div2_nocrop(unsigned char* source1,
			 unsigned char* source2,
			 unsigned char* dest,int inc);

  void copy8_div2_destlinear_nocrop(unsigned char* source1,
				    unsigned char* source2,
				    unsigned char* dest,int inc);
  
  void copy16_div2_destlinear_nocrop(unsigned char* source1,
				     unsigned char* source2,
				     unsigned char* dest,int inc);
  

  void copy8_div4_nocrop(unsigned char* source1,
			 unsigned char* source2,
			 unsigned char* source3,
			 unsigned char* source4,
			 unsigned char* dest,int inc);

  void copy8_src1linear_crop(short* source1,unsigned char* dest,int inc);

  void copy8_src2linear_crop(unsigned char* source1,
			     short int* source2,
			     unsigned char* dest,int inc);
  void copy8_div2_src3linear_crop(unsigned char* source1,
				  unsigned char* source2,
				  short int* source3,
				  unsigned char* dest,int inc);

  void copy8_div4_src5linear_crop(unsigned char* source1,
				  unsigned char* source2,
				  unsigned char* source3,
				  unsigned char* source4,
				  short int* source5,
				  unsigned char* dest,int inc);

};
#endif
