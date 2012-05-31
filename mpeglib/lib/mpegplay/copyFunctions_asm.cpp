/*
  copyfunctions base class
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "copyFunctions_asm.h"

using namespace std;

CopyFunctions_ASM::CopyFunctions_ASM() {
}

CopyFunctions_ASM::~CopyFunctions_ASM() {
}


int CopyFunctions_ASM::support() {
  return false;
}


void CopyFunctions_ASM::startNOFloatSection() {
}


void CopyFunctions_ASM::endNOFloatSection() {
}

void CopyFunctions_ASM::copy8_byte(unsigned char* ,
				   unsigned char* ,int ) {
  cout << "CopyFunctions_ASM::copy8_byte not implemented"<<endl;
}


void CopyFunctions_ASM::copy8_src1linear_crop(short* ,
					      unsigned char* ,int ) {
  cout << "CopyFunctions_ASM:: not implemented"<<endl;
}




void CopyFunctions_ASM::copy8_div2_nocrop(unsigned char* ,
					  unsigned char* ,
					  unsigned char* ,int ) {
  cout << "CopyFunctions_ASM:: copy8_div2_nocrop not implemented"<<endl;
}


void CopyFunctions_ASM::copy8_div2_destlinear_nocrop(unsigned char* ,
						     unsigned char* ,
						     unsigned char* ,
						     int ) {
  cout << "CopyFunctions_ASM:: copy8_div2_destlinear_nocrop not implemented"
       <<endl;
}


void CopyFunctions_ASM::copy16_div2_destlinear_nocrop(unsigned char* ,
						      unsigned char* ,
						      unsigned char* ,
						      int ) {
  cout << "CopyFunctions_ASM:: copy16_div2_destlinear_nocrop not implemented"
       <<endl;
}


void CopyFunctions_ASM::copy8_src2linear_crop(unsigned char* ,
					      short int* ,
					      unsigned char* ,int ) {
  cout << "CopyFunctions_ASM:: copy8_src2linear_crop not implemented"<<endl;
}


void CopyFunctions_ASM::copy8_div2_src3linear_crop(unsigned char* ,
						   unsigned char* ,
						   short int* ,
						   unsigned char* ,
						   int ) {
  cout << "CopyFunctions_ASM:: copy8_div2_src3linear_crop not implemented"
       <<endl;
}
