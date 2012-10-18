/*
  copyfunctions implementation in mmx
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "copyFunctions_mmx.h"

using namespace std;

// mmx goodies
static long ADD_1[]    = {0x01010101, 0x01010101};
static long ADDW_1[]   = {0x00010001, 0x00010001};
static long MASK_AND[] = {0x7f7f7f7f, 0x7f7f7f7f};

void dummyCopyFunctions() {
  cout << "ADD_1:"<<ADD_1<<endl;
  cout << "ADDW_1:"<<ADDW_1<<endl;
  cout << "MASK_AND:"<<MASK_AND<<endl;
}
 
CopyFunctions_MMX::CopyFunctions_MMX() {
#ifdef INTEL
  lmmx=mm_support();
#else
  lmmx=false;
  cout << "no INTEL arch- disable MMX in copyFunctions"<<endl;
#endif
}


CopyFunctions_MMX::~CopyFunctions_MMX() {
}

int CopyFunctions_MMX::support() {
  return lmmx;
}


#if defined (__GNUC__) && defined (INTEL)

void CopyFunctions_MMX::startNOFloatSection() {
}


void CopyFunctions_MMX::endNOFloatSection() {
  emms();
}



void CopyFunctions_MMX::copy8_byte(unsigned char* source1,
				   unsigned char* dest,int inc) {
  int rr=4;

  asm  (
	"1:\n"
	"movq   (%0)    ,%%mm0\n"
	"leal   (%0,%2) ,%0\n"
	"movq   (%0)    ,%%mm1\n"
	"leal   (%0,%2) ,%0\n"
	
	// Write
	"movq   %%mm0   ,(%1)\n"
	"leal   (%1,%2) ,%1\n"
	"movq   %%mm1   ,(%1)\n"
	"leal   (%1,%2) ,%1\n"
	
	"decl       %3\n"
	"jnz        1b\n"		  
	:
	: "r"(source1),"r"(dest),"r"(inc),"r"(rr)
	);
}



void CopyFunctions_MMX::copy8_src1linear_crop(short* source1,
					      unsigned char* dest,int inc) {

  asm (
       "movq             (%1),%%mm0\n"
       "packuswb       8(%1),%%mm0\n"
       "movq            %%mm0,(%0)\n"
       "addl                   %2,%0\n"
       
       "movq            16(%1),%%mm0\n"
       "packuswb       24(%1),%%mm0\n"
       "movq            %%mm0,(%0)\n"
       "addl                   %2,%0\n"
       
       "movq            32(%1),%%mm0\n"
       "packuswb       40(%1),%%mm0\n"
       "movq            %%mm0,(%0)\n"
       "addl                   %2,%0\n"
       
       "movq            48(%1),%%mm0\n"
       "packuswb       56(%1),%%mm0\n"
       "movq            %%mm0,(%0)\n"
       "addl                   %2,%0\n"
       
       "movq            64(%1),%%mm0\n"
       "packuswb       72(%1),%%mm0\n"
       "movq            %%mm0,(%0)\n"
       "addl                   %2,%0\n"
       
       "movq            80(%1),%%mm0\n"
       "packuswb       88(%1),%%mm0\n"
       "movq            %%mm0,(%0)\n"
       "addl                   %2,%0\n"
       
       "movq            96(%1),%%mm0\n"
       "packuswb       104(%1),%%mm0\n"
       "movq            %%mm0,(%0)\n"
       "addl                   %2,%0\n"
       
       "movq            112(%1),%%mm0\n"
       "packuswb       120(%1),%%mm0\n"
       "movq            %%mm0,(%0)\n"
       :
       :"r" (dest), "r" (source1),"r" (inc)
       );
  
}



void CopyFunctions_MMX::copy8_div2_nocrop(unsigned char* source1,
					  unsigned char* source2,
					  unsigned char* dest,int inc) {
  int h=8;
  asm (
       "movq       MASK_AND,  %%mm5\n"
       "movq       ADD_1,     %%mm6\n"
       "1:\t"
       "movq       (%1),   %%mm0\n"      /* 8 s */
       "movq       (%4),   %%mm1\n"      /* 8 s +lx */
       "psrlw      $1,%%mm0\n"
       "psrlw      $1,%%mm1\n"
       "pand       %%mm5,%%mm0\n"
       "pand       %%mm5,%%mm1\n"
       "paddusb    %%mm1,%%mm0\n"
       "addl       %3,%1\n"
       "paddusb    %%mm6,%%mm0\n"
       "addl       %3,%4\n"
       "movq       %%mm0,(%2)\n"
       "decl       %0\n"
       "leal       (%2, %3), %2\n"
       "jnz        1b\n"
       :
       : "c"(h), "r"(source1), "r"(dest), "r"(inc), "r"(source2)
       );
}


void CopyFunctions_MMX::copy8_div2_destlinear_nocrop(unsigned char* source1,
						     unsigned char* source2,
						     unsigned char* dest,
						     int inc) {
  int h=8;
  asm (
       "movq       MASK_AND,  %%mm5\n"
       "1:\t"
       "movq       (%1),   %%mm0\n"      /* 8 s */
       "movq       (%4),   %%mm1\n"      /* 8 s +lx */
       "psrlw      $1,%%mm0\n"
       "psrlw      $1,%%mm1\n"
       "pand       %%mm5,%%mm0\n"
       "pand       %%mm5,%%mm1\n"
       "paddusb    %%mm1,%%mm0\n"
       "addl       %3,%1\n"
       "addl       %3,%4\n"
       "movq       %%mm0,(%2)\n"
       "decl       %0\n"
       "leal       8(%2), %2\n"
       "jnz        1b\n"
       :
       : "c"(h), "r"(source1), "r"(dest), "r"(inc), "r"(source2)
       );
}



void CopyFunctions_MMX::copy16_div2_destlinear_nocrop(unsigned char* source1,
						      unsigned char* source2,
						      unsigned char* dest,
						      int inc) {
  int h=16;
  inc=inc-8;
  asm (
       "movq       MASK_AND,  %%mm5\n"
       "1:\t"
       "movq       (%1),   %%mm0\n"      /* 8 s */
       "movq       (%4),   %%mm1\n"      /* 8 s +lx */
       "psrlw      $1,%%mm0\n"
       "psrlw      $1,%%mm1\n"
       "pand       %%mm5,%%mm0\n"
       "pand       %%mm5,%%mm1\n"
       "paddusb    %%mm1,%%mm0\n"
       "leal       8(%1),%1\n"
       "leal       8(%4),%4\n"
       "movq       %%mm0,(%2)\n"
       "leal       8(%2),%2\n"
       
       "movq       (%1),   %%mm0\n"      /* 8 s */
       "movq       (%4),   %%mm1\n"      /* 8 s +lx */
       "psrlw      $1,%%mm0\n"
       "psrlw      $1,%%mm1\n"
       "pand       %%mm5,%%mm0\n"
       "pand       %%mm5,%%mm1\n"
       "paddusb    %%mm1,%%mm0\n"
       "leal       (%3,%1),%1\n"
       "leal       (%3,%4),%4\n"
       "movq       %%mm0,(%2)\n"
       "leal       8(%2),%2\n"
       
       "decl       %0\n"
       "jnz        1b\n"
       :
       : "c"(h), "r"(source1), "r"(dest), "r"(inc), "r"(source2)
       );
}


void CopyFunctions_MMX::copy8_src2linear_crop(unsigned char* source1,
					      short int* source2,
					      unsigned char* dest,int inc) {
  
  int rr=8;
  // buggy
  
  asm (
       ".align 32\n"
       "pxor      %%mm2 ,%%mm2\n" //    0    0    0    0    0    0    0    0
       "1:\n"
       "movq      (%0)  ,%%mm0\n" // s1_7 s1_6 s1_5 s1_4 s1_3 s1_2 s1_1 s1_0
       "movq      (%0)  ,%%mm4\n" // s1_7 s1_6 s1_5 s1_4 s1_3 s1_2 s1_1 s1_0
       "punpckhbw %%mm2 ,%%mm0\n" //   0 s1_7    0 s1_6    0 s1_5    0  s1_4
       "punpcklbw %%mm2 ,%%mm4\n" //   0 s1_3    0 s1_2    0 s1_1    0  s1_0
       "movq      (%1)  ,%%mm1\n" //   s23h s23l s22h s22l s21l s21h s20h s20l
       "movq      8(%1) ,%%mm5\n" //   s27h s27l s26h s26l s25l s25h s24h s24l
       "paddw    %%mm0 ,%%mm5\n"  //   mm4=mm4 + s3_0..3
       "paddw    %%mm4 ,%%mm1\n"  //   mm0=mm0 + s3_4..7
       "packuswb  %%mm5 ,%%mm1\n" //   cm[...]
       
       "movq      %%mm1 ,(%2)\n"  //   wrote out
       
       "leal      (%0,%3), %0\n"  //   source1+=inc
       "leal      16(%1) , %1\n"  //   source2+=inc
       "leal      (%2,%3), %2\n"  //   dest+=inc
       "decl      %4\n"
       "jnz       1b\n"
       //"emms\n"
       :
       : "r"(source1), "r"(source2), "r"(dest),"r"(inc),"r"(rr)
       );
}


void CopyFunctions_MMX::copy8_div2_src3linear_crop(unsigned char* source1,
						   unsigned char* source2,
						   short int* source3,
						   unsigned char* dest,
						   int inc){
    // buggy
    int rr=8;

    asm (
	 "pxor      %%mm2 ,%%mm2\n" //   0    0    0    0    0    0    0    0
	 "pxor      %%mm3 ,%%mm3\n" //   0    0    0    0    0    0    0    0
	 "movq      ADDW_1,%%mm6\n" //   0    1    0    1    0    1    0    1
	 "1:\n"		  
	 "movq      (%0)  ,%%mm0\n" // s1_7 s1_6 s1_5 s1_4 s1_3 s1_2 s1_1 s1_0
	 "movq      (%1)  ,%%mm1\n" // s2_7 s2_6 s2_5 s2_4 s2_3 s2_2 s2_1 s2_0
	 "movq      %%mm0 ,%%mm4\n" // s1_7 s1_6 s1_5 s1_4 s1_3 s1_2 s1_1 s1_0
	 "movq      %%mm1 ,%%mm5\n" // s2_7 s2_6 s2_5 s2_4 s2_3 s2_2 s2_1 s2_0
	 "punpckhbw %%mm2 ,%%mm0\n" //   0 s1_7    0 s1_6    0 s1_5    0  s1_4
	 "punpckhbw %%mm3 ,%%mm1\n" //   0 s2_7    0 s2_6    0 s2_5    0  s2_4
	 "punpcklbw %%mm2 ,%%mm4\n" //   0 s1_3    0 s1_2    0 s1_1    0  s1_0
	 "punpcklbw %%mm3 ,%%mm5\n" //   0 s2_3    0 s2_2    0 s2_1    0  s2_0
	 "paddusw   %%mm4 ,%%mm5\n" //   mm5=s1_0..3 + s2_0..3
	 "paddusw   %%mm0 ,%%mm1\n" //   mm1=s1_4..7 + s2_4..7
	 "paddusw   %%mm6 ,%%mm5\n" //   mm5=mm5 + 1
	 "paddusw   %%mm6 ,%%mm1\n" //   mm1=mm1 + 1
	 "psraw     $1    ,%%mm1\n" //   mm1=mm1/2
	 "psraw     $1    ,%%mm5\n" //   mm5=mm5/2
	 "movq      (%2)  ,%%mm0\n" // s33h s33l s32h s32l s31l s31h s30h s30l
	 "movq      8(%2) ,%%mm4\n" // s37h s37l s36h s36l s35l s35h s34h s34l
	 "paddw     %%mm0 ,%%mm5\n" //   mm5=mm5 + s3_0..3
	 "paddw     %%mm4 ,%%mm1\n" //   mm1=mm1 + s3_4..7
	 "packuswb  %%mm1 ,%%mm5\n" //   cm[...]
	 "movq      %%mm5 ,(%3)\n"  //   wrote out
	 "leal      (%0,%4), %0\n"  //   source1+=inc
	 "leal      (%1,%4), %1\n"  //   source2+=inc
	 "addl      $16    ,%2 \n"  //   source3+8
	 "leal      (%3,%4), %3\n"  //   dest+=inc
	 "decl      %5\n"
	 "jnz       1b\n"
	 :
	 : "r"(source1), "r"(source2), "r"(source3),
	 "r"(dest),"r"(inc),"m"(rr)
	 );
}


#endif
