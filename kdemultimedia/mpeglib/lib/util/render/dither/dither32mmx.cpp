/*
  MMX ditherer for 32 bit displays
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "ditherMMX.h"

#include <iostream>

using namespace std;


#ifndef INTEL
  void dither32_mmx(unsigned char* lum,
		  unsigned char* cr,
		  unsigned char* cb,
		  unsigned char* out,
		  int rows,
		  int cols,
		  int mod) {
  printf("urgs! dither32_mmx \n");
  printf("never should happen!\n");
  exit(0);
}

#else


static unsigned long  MMX32_80w[]         = {0x00800080, 0x00800080};
static unsigned long  MMX32_10w[]         = {0x00100010, 0x00100010};   
static unsigned long  MMX32_00FFw[]       = {0x00ff00ff, 0x00ff00ff}; 
static unsigned long  MMX32_FF00w[]       = {0xff00ff00, 0xff00ff00}; 
static unsigned short MMX32_Ycoeff[]      = {0x4a, 0x4a, 0x4a, 0x4a}; 
static unsigned short MMX32_Vredcoeff[]   = {0x59, 0x59, 0x59, 0x59};  
static unsigned short MMX32_Ubluecoeff[]  = {0x72, 0x72, 0x72, 0x72};    
static unsigned short MMX32_Ugrncoeff[]   = {0xffea,0xffea,0xffea,0xffea}; 
static unsigned short MMX32_Vgrncoeff[]   = {0xffd2,0xffd2,0xffd2,0xffd2};  

void dummy_dithermmx32() {
  cout << "MMX32_10w:"<<MMX32_10w<<endl;
  cout << "MMX32_80w:"<<MMX32_80w<<endl;
  cout << "MMX32_Ubluecoeff:"<<MMX32_Ubluecoeff<<endl;
  cout << "MMX32_Vredcoeff:"<<MMX32_Vredcoeff<<endl;
  cout << "MMX32_Ugrncoeff:"<<MMX32_Ugrncoeff<<endl;
  cout << "MMX32_Vgrncoeff:"<<MMX32_Vgrncoeff<<endl;
  cout << "MMX32_Ycoeff:"<<MMX32_Ycoeff<<endl;
  cout << "MMX32_00FFw:"<<MMX32_00FFw<<endl;
  cout << "MMX32_FF00w:"<<MMX32_FF00w<<endl;
}


/**
   This MMX assembler is my first assembler/MMX program ever.
   Thus it maybe buggy.
   Send patches to:
   mvogt@rhrk.uni-kl.de

   After it worked fine I have "obfuscated" the code a bit to have
   more parallism in the MMX units. This means I moved
   initilisation around and delayed other instruction.
   Performance measurement did not show that this brought any advantage
   but in theory it _should_ be faster this way.

   The overall performanve gain to the C based dither was 30%-40%.
   The MMX routine calculates 256bit=8RGB values in each cycle
   (4 for row1 & 4 for row2)

   The red/green/blue.. coefficents are taken from the mpeg_play 
   player. They look nice, but I dont know if you can have
   better values, to avoid integer rounding errors.
   

   IMPORTANT:
   ==========

   It is a requirement that the cr/cb/lum are 8 byte aligned and
   the out are 16byte aligned or you will/may get segfaults

*/

void dither32_mmx(unsigned char* lum,
		  unsigned char* cr,
		  unsigned char* cb,
		  unsigned char* out,
		  int rows,
		  int cols,
		  int mod) {


    
    unsigned int *row1;
    unsigned int *row2;
    row1 = (unsigned int *)out;           // 32 bit target

    unsigned char* end = lum +cols*rows;    // Pointer to the end
    int x=cols;
    row2=row1+cols+mod;                   // start of second row 
    mod=4*cols+8*mod;                     // increment for row1 in byte

    // buffer for asm function
    int buf[6];
    buf[0]=(int)(lum+cols);   // lum2 pointer
    buf[1]=(int)end;
    buf[2]=x;
    buf[3]=mod;     
    buf[4]=0; //tmp0;
    buf[5]=cols;


    __asm__ __volatile__ (
	         ".align 32\n"
		 "1:\n"
		
		 // create Cr (result in mm1)
		 "movd (%0), %%mm1\n"      //         0  0  0  0  v3 v2 v1 v0
		 "pxor %%mm7,%%mm7\n"      //         00 00 00 00 00 00 00 00
		 "movd (%2), %%mm2\n"           //    0  0  0  0 l3 l2 l1 l0
		 "punpcklbw %%mm7,%%mm1\n" //         0  v3 0  v2 00 v1 00 v0
		 "punpckldq %%mm1,%%mm1\n" //         00 v1 00 v0 00 v1 00 v0
		 "psubw MMX32_80w,%%mm1\n"   // mm1-128:r1 r1 r0 r0 r1 r1 r0 r0 

		 // create Cr_g (result in mm0)
		 "movq %%mm1,%%mm0\n"           // r1 r1 r0 r0 r1 r1 r0 r0
		 "pmullw MMX32_Vgrncoeff,%%mm0\n" // red*-46dec=0.7136*64
		 "pmullw MMX32_Vredcoeff,%%mm1\n" // red*89dec=1.4013*64
		 "psraw  $6, %%mm0\n"           // red=red/64
		 "psraw  $6, %%mm1\n"           // red=red/64

		 
		 // create L1 L2 (result in mm2,mm4)
		 // L2=lum2
		 "movl %2,16%5\n"               // store register in tmp0
		 "movl %5,%2\n"                 // lum2->register
		 "movd (%2),%%mm3\n"            //    0  0  0  0 L3 L2 L1 L0
		 "movl 16%5,%2\n"               // tmp0->register
		 "punpckldq %%mm3,%%mm2\n"      //   L3 L2 L1 L0 l3 l2 l1 l0
		 "movq %%mm2,%%mm4\n"           //   L3 L2 L1 L0 l3 l2 l1 l0
		 "pand MMX32_FF00w, %%mm2\n"      //   L3 0  L1  0 l3  0 l1  0
		 "pand MMX32_00FFw, %%mm4\n"      //   0  L2  0 L0  0 l2  0 l0
		 "psrlw $8,%%mm2\n"             //   0  L3  0 L1  0 l3  0 l1



		 // create R (result in mm6)
		 "movq %%mm2,%%mm5\n"           //   0 L3  0 L1  0 l3  0 l1
		 "movq %%mm4,%%mm6\n"           //   0 L2  0 L0  0 l2  0 l0
		 "paddsw  %%mm1, %%mm5\n"       // lum1+red:x R3 x R1 x r3 x r1
		 "paddsw  %%mm1, %%mm6\n"       // lum1+red:x R2 x R0 x r2 x r0
		 "packuswb %%mm5,%%mm5\n"       //  R3 R1 r3 r1 R3 R1 r3 r1
		 "packuswb %%mm6,%%mm6\n"       //  R2 R0 r2 r0 R2 R0 r2 r0
		 "pxor %%mm7,%%mm7\n"      //         00 00 00 00 00 00 00 00
		 "punpcklbw %%mm5,%%mm6\n"      //  R3 R2 R1 R0 r3 r2 r1 r0


		 // create Cb (result in mm1)
		 "movd (%1), %%mm1\n"      //         0  0  0  0  u3 u2 u1 u0
		 "punpcklbw %%mm7,%%mm1\n" //         0  u3 0  u2 00 u1 00 u0
		 "punpckldq %%mm1,%%mm1\n" //         00 u1 00 u0 00 u1 00 u0
		 "psubw MMX32_80w,%%mm1\n"   // mm1-128:u1 u1 u0 u0 u1 u1 u0 u0 
		 // create Cb_g (result in mm5)
		 "movq %%mm1,%%mm5\n"            // u1 u1 u0 u0 u1 u1 u0 u0
		 "pmullw MMX32_Ugrncoeff,%%mm5\n"  // blue*-109dec=1.7129*64
		 "pmullw MMX32_Ubluecoeff,%%mm1\n" // blue*114dec=1.78125*64
		 "psraw  $6, %%mm5\n"            // blue=red/64
		 "psraw  $6, %%mm1\n"            // blue=blue/64


		 // create G (result in mm7)
		 "movq %%mm2,%%mm3\n"      //   0  L3  0 L1  0 l3  0 l1
		 "movq %%mm4,%%mm7\n"      //   0  L2  0 L0  0 l2  0 l1
		 "paddsw  %%mm5, %%mm3\n"  // lum1+Cb_g:x G3t x G1t x g3t x g1t
		 "paddsw  %%mm5, %%mm7\n"  // lum1+Cb_g:x G2t x G0t x g2t x g0t
		 "paddsw  %%mm0, %%mm3\n"  // lum1+Cr_g:x G3  x G1  x g3  x g1
		 "paddsw  %%mm0, %%mm7\n"  // lum1+blue:x G2  x G0  x g2  x g0
		 "packuswb %%mm3,%%mm3\n"  // G3 G1 g3 g1 G3 G1 g3 g1
		 "packuswb %%mm7,%%mm7\n"  // G2 G0 g2 g0 G2 G0 g2 g0
		 "punpcklbw %%mm3,%%mm7\n" // G3 G2 G1 G0 g3 g2 g1 g0
		 

		 // create B (result in mm5)
		 "movq %%mm2,%%mm3\n"         //   0  L3  0 L1  0 l3  0 l1
		 "movq %%mm4,%%mm5\n"         //   0  L2  0 L0  0 l2  0 l1
		 "paddsw  %%mm1, %%mm3\n"     // lum1+blue:x B3 x B1 x b3 x b1
		 "paddsw  %%mm1, %%mm5\n"     // lum1+blue:x B2 x B0 x b2 x b0
		 "packuswb %%mm3,%%mm3\n"     // B3 B1 b3 b1 B3 B1 b3 b1
		 "packuswb %%mm5,%%mm5\n"     // B2 B0 b2 b0 B2 B0 b2 b0
		 "punpcklbw %%mm3,%%mm5\n"    // B3 B2 B1 B0 b3 b2 b1 b0


		 // fill destination row1 (needed are mm6=Rr,mm7=Gg,mm5=Bb)

		 "pxor %%mm2,%%mm2\n"           //  0  0  0  0  0  0  0  0
		 "pxor %%mm4,%%mm4\n"           //  0  0  0  0  0  0  0  0
		 "movq %%mm6,%%mm1\n"           // R3 R2 R1 R0 r3 r2 r1 r0
		 "movq %%mm5,%%mm3\n"           // B3 B2 B1 B0 b3 b2 b1 b0
		 // process lower lum
		 "punpcklbw %%mm4,%%mm1\n"      //  0 r3  0 r2  0 r1  0 r0
		 "punpcklbw %%mm4,%%mm3\n"      //  0 b3  0 b2  0 b1  0 b0
		 "movq %%mm1,%%mm2\n"           //  0 r3  0 r2  0 r1  0 r0
		 "movq %%mm3,%%mm0\n"           //  0 b3  0 b2  0 b1  0 b0
		 "punpcklwd %%mm1,%%mm3\n"      //  0 r1  0 b1  0 r0  0 b0
		 "punpckhwd %%mm2,%%mm0\n"      //  0 r3  0 b3  0 r2  0 b2

		 "pxor %%mm2,%%mm2\n"           //  0  0  0  0  0  0  0  0
		 "movq %%mm7,%%mm1\n"           // G3 G2 G1 G0 g3 g2 g1 g0
		 "punpcklbw %%mm1,%%mm2\n"      // g3  0 g2  0 g1  0 g0  0
		 "punpcklwd %%mm4,%%mm2\n"      //  0  0 g1  0  0  0 g0  0 
		 "por  %%mm3, %%mm2\n"      //  0 r1 g1 b1  0 r0 g0 b0
		 "movq   %%mm2,(%3)\n"          // wrote out ! row1

		 "pxor %%mm2,%%mm2\n"           //  0  0  0  0  0  0  0  0
		 "punpcklbw %%mm1,%%mm4\n"      // g3  0 g2  0 g1  0 g0  0
		 "punpckhwd %%mm2,%%mm4\n"      //  0  0 g3  0  0  0 g2  0 
		 "por  %%mm0, %%mm4\n"      //  0 r3 g3 b3  0 r2 g2 b2
		 "movq   %%mm4,8(%3)\n"         // wrote out ! row1
		 
		 // fill destination row2 (needed are mm6=Rr,mm7=Gg,mm5=Bb)
		 // this can be done "destructive"
		 "pxor %%mm2,%%mm2\n"           //  0  0  0  0  0  0  0  0
		 "punpckhbw %%mm2,%%mm6\n"      //  0 R3  0 R2  0 R1  0 R0
		 "punpckhbw %%mm1,%%mm5\n"      // G3 B3 G2 B2 G1 B1 G0 B0
		 "movq %%mm5,%%mm1\n"           // G3 B3 G2 B2 G1 B1 G0 B0
		 "punpcklwd %%mm6,%%mm1\n"      //  0 R1 G1 B1  0 R0 G0 B0
		 "movq   %%mm1,(%4)\n"          // wrote out ! row2
		 "punpckhwd %%mm6,%%mm5\n"      //  0 R3 G3 B3  0 R2 G2 B2
		 "movq   %%mm5,8(%4)\n"         // wrote out ! row2
		 
		 "addl  $4,%2\n"            // lum+4
		 "addl  $4,%5\n"            // lum2+4
		 "leal  16(%3),%3\n"        // row1+16
		 "leal  16(%4),%4\n"        // row2+16
		 "addl  $2, %0\n"           // cr+2
		 "addl  $2, %1\n"           // cb+2

		 "subl  $4,8%5\n"           // x+4 x is buf[2]
		 "cmpl  $0,8%5\n"

		 "jne   1b\n"
		 "addl           20%5,   %2\n" // lum  += cols 
		 "movl %2,16%5\n"              // store register in tmp0
		 "movl 20%5,%2\n"              // cols->register

		 "addl           %2,     %5\n" // lum2 += cols 
		 "addl           12%5,   %3\n" // row1+= mod is buf[0]
		 "addl           12%5,   %4\n" // row2+= mod is buf[0]

		 "movl %2, 8%5\n"              // x=cols
		 "movl 16%5,%2\n"              // store tmp0 in register

		 "cmpl           4%5,    %2\n"  // buf[1] is end
		 "jl             1b\n"
		 "emms\n"
		 :
		 : "r" (cr), "r"(cb),"r"(lum),
		 "r"(row1),"r"(row2),"m"(buf[0])
		 );



}


#endif
