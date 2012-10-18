/*
  copyfunctions base class
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __COPYFUNCTIONS_ASM_H
#define __COPYFUNCTIONS_ASM_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>

class CopyFunctions_ASM {

 public:
  CopyFunctions_ASM();
  virtual ~CopyFunctions_ASM();

  virtual int support();

  virtual void startNOFloatSection();
  virtual void endNOFloatSection();

  virtual void copy8_byte(unsigned char* source1,
			  unsigned char* dest,int inc);

  virtual void copy8_src1linear_crop(short* source1,
				     unsigned char* dest,int inc);

  virtual void copy8_div2_nocrop(unsigned char* source1,
				 unsigned char* source2,
				 unsigned char* dest,int inc);

  virtual void copy8_div2_destlinear_nocrop(unsigned char* source1,
					    unsigned char* source2,
					    unsigned char* dest,int inc);

  virtual void copy16_div2_destlinear_nocrop(unsigned char* source1,
					     unsigned char* source2,
					     unsigned char* dest,int inc);

  virtual void copy8_src2linear_crop(unsigned char* source1,
				     short int* source2,
				     unsigned char* dest,int inc);

  virtual void copy8_div2_src3linear_crop(unsigned char* source1,
					  unsigned char* source2,
					  short int* source3,
					  unsigned char* dest,int inc);
};
#endif
