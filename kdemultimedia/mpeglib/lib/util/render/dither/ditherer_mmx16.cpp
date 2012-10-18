
#include "ditherMMX.h"

#include <iostream>

using namespace std;
		
#ifndef INTEL
// nothing
void  ditherBlock(unsigned char *lum, unsigned char *cr, unsigned char *cb,
                  unsigned char *out,
                  int cols, int rows, int screen_width) {
  printf("call to ditherBlock. this should never happen\n");
  printf("check mmx detection routine.\n");
  exit(0);
}
#else	
	

static long long MMX16_0 = 0L;
static unsigned long  MMX16_10w[]         = {0x00100010, 0x00100010};
static unsigned long  MMX16_80w[]         = {0x00800080, 0x00800080};
static unsigned long  MMX16_00FFw[]       = {0x00ff00ff, 0x00ff00ff};
static unsigned short MMX16_Ublucoeff[]   = {0x81, 0x81, 0x81, 0x81};
static unsigned short MMX16_Vredcoeff[]   = {0x66, 0x66, 0x66, 0x66};
static unsigned short MMX16_Ugrncoeff[]   = {0xffe8, 0xffe8, 0xffe8, 0xffe8};
static unsigned short MMX16_Vgrncoeff[]   = {0xffcd, 0xffcd, 0xffcd, 0xffcd};
static unsigned short MMX16_Ycoeff[]      = {0x4a, 0x4a, 0x4a, 0x4a};   
static unsigned short MMX16_redmask[]     = {0xf800, 0xf800, 0xf800, 0xf800}; 
static unsigned short MMX16_grnmask[]     = {0x7e0, 0x7e0, 0x7e0, 0x7e0}; 

void dummy_dithermmx16() {
  cout << "MMX16_0"<<MMX16_0<<endl;
  cout << "MMX16_10w:"<<MMX16_10w<<endl;
  cout << "MMX16_80w:"<<MMX16_80w<<endl;
  cout << "MMX16_Ublucoeff:"<<MMX16_Ublucoeff<<endl;
  cout << "MMX16_Vredcoeff:"<<MMX16_Vredcoeff<<endl;
  cout << "MMX16_Ugrncoeff:"<<MMX16_Ugrncoeff<<endl;
  cout << "MMX16_Vgrncoeff:"<<MMX16_Vgrncoeff<<endl;
  cout << "MMX16_Ycoeff:"<<MMX16_Ycoeff<<endl;
  cout << "MMX16_redmask:"<<MMX16_redmask<<endl;
  cout << "MMX16_grnmask:"<<MMX16_grnmask<<endl;
  cout << "MMX16_00FFw:"<<MMX16_00FFw<<endl;
}


void  ditherBlock(unsigned char *lum, 
		  unsigned char *cr, 
		  unsigned char *cb,
		  unsigned char *out,
		  int rows, 
		  int cols, 
		  int mod) {

    unsigned short *row1;
    unsigned short *row2;
    row1 = (unsigned short* )out;         // 16 bit target

    unsigned char* end = lum +cols*rows;    // Pointer to the end
    int x=cols;
    row2=row1+mod+cols;                   // start of second row 
    mod=2*cols+4*mod;                     // increment for row1 in byte

    // buffer for asm function
    int buf[6];
    buf[0]=(int)(lum+cols);   // lum2 pointer
    buf[1]=(int)end;     
    buf[2]=x;
    buf[3]=mod;     
    buf[4]=0; //tmp0;
    buf[5]=cols;



    __asm__ __volatile__(
         ".align 32\n"
         "1:\n"
         "movd           (%1),                   %%mm0\n"        // 4 Cb         0  0  0  0 u3 u2 u1 u0
         "pxor           %%mm7,                  %%mm7\n"
         "movd           (%0),                   %%mm1\n" // 4 Cr                0  0  0  0 v3 v2 v1 v0
         "punpcklbw      %%mm7,                  %%mm0\n" // 4 W cb   0 u3  0 u2  0 u1  0 u0
         "punpcklbw      %%mm7,                  %%mm1\n" // 4 W cr   0 v3  0 v2  0 v1  0 v0
         "psubw          MMX16_80w,                %%mm0\n"
         "psubw          MMX16_80w,                %%mm1\n"
         "movq           %%mm0,                  %%mm2\n"        // Cb                   0 u3  0 u2  0 u1  0 u0
         "movq           %%mm1,                  %%mm3\n" // Cr
         "pmullw         MMX16_Ugrncoeff,          %%mm2\n" // Cb2green 0 R3  0 R2  0 R1  0 R0
         "movq           (%2),                   %%mm6\n"        // L1      l7 L6 L5 L4 L3 L2 L1 L0
         "pmullw         MMX16_Ublucoeff,          %%mm0\n" // Cb2blue
         "pand           MMX16_00FFw,              %%mm6\n" // L1      00 L6 00 L4 00 L2 00 L0
         "pmullw         MMX16_Vgrncoeff,          %%mm3\n" // Cr2green
         "movq           (%2),                   %%mm7\n" // L2
         "pmullw         MMX16_Vredcoeff,          %%mm1\n" // Cr2red
         //                      "psubw          MMX16_10w,                %%mm6\n"
         "psrlw          $8,                     %%mm7\n"        // L2           00 L7 00 L5 00 L3 00 L1
         "pmullw         MMX16_Ycoeff,             %%mm6\n" // lum1
         //                      "psubw          MMX16_10w,                %%mm7\n" // L2
         "paddw          %%mm3,                  %%mm2\n" // Cb2green + Cr2green == green
         "pmullw         MMX16_Ycoeff,             %%mm7\n"  // lum2

         "movq           %%mm6,                  %%mm4\n"  // lum1
         "paddw          %%mm0,                  %%mm6\n"  // lum1 +blue 00 B6 00 B4 00 B2 00 B0
         "movq           %%mm4,                  %%mm5\n"  // lum1
         "paddw          %%mm1,                  %%mm4\n"  // lum1 +red  00 R6 00 R4 00 R2 00 R0
         "paddw          %%mm2,                  %%mm5\n"  // lum1 +green 00 G6 00 G4 00 G2 00 G0
         "psraw          $6,                     %%mm4\n"  // R1 0 .. 64
         "movq           %%mm7,                  %%mm3\n"  // lum2                       00 L7 00 L5 00 L3 00 L1
         "psraw          $6,                     %%mm5\n"  // G1  - .. +
         "paddw          %%mm0,                  %%mm7\n"  // Lum2 +blue 00 B7 00 B5 00 B3 00 B1
         "psraw          $6,                     %%mm6\n"  // B1         0 .. 64
         "packuswb       %%mm4,                  %%mm4\n"  // R1 R1
         "packuswb       %%mm5,                  %%mm5\n"  // G1 G1
         "packuswb       %%mm6,                  %%mm6\n"  // B1 B1
         "punpcklbw      %%mm4,                  %%mm4\n"
         "punpcklbw      %%mm5,                  %%mm5\n"

         "pand           MMX16_redmask,            %%mm4\n"
         "psllw          $3,                     %%mm5\n"  // GREEN       1
         "punpcklbw      %%mm6,                  %%mm6\n"
         "pand           MMX16_grnmask,            %%mm5\n"
         "pand           MMX16_redmask,            %%mm6\n"
         "por            %%mm5,                  %%mm4\n" //
         "psrlw          $11,                    %%mm6\n"                // BLUE        1
         "movq           %%mm3,                  %%mm5\n" // lum2
         "paddw          %%mm1,                  %%mm3\n"        // lum2 +red      00 R7 00 R5 00 R3 00 R1
         "paddw          %%mm2,                  %%mm5\n" // lum2 +green 00 G7 00 G5 00 G3 00 G1
         "psraw          $6,                     %%mm3\n" // R2
         "por            %%mm6,                  %%mm4\n" // MM4
         "psraw          $6,                     %%mm5\n" // G2

	 "movl %2,16%5\n"               // store register in tmp0
	 "movl %5,%2\n"                 // lum2->register
	 "movq (%2),%%mm6\n"            // 0  0  0  0 L3 L2 L1 L0 (load lum2)


         //"movq           (%2, %5),               %%mm6\n" // L3 load lum2
         "psraw          $6,                     %%mm7\n"
         "packuswb       %%mm3,                  %%mm3\n"
         "packuswb       %%mm5,                  %%mm5\n"
         "packuswb       %%mm7,                  %%mm7\n"
         "pand                   MMX16_00FFw,              %%mm6\n"  // L3
         "punpcklbw      %%mm3,                  %%mm3\n"
         //                              "psubw          MMX16_10w,                        %%mm6\n"  // L3
         "punpcklbw      %%mm5,                  %%mm5\n"
         "pmullw         MMX16_Ycoeff,             %%mm6\n"  // lum3
         "punpcklbw      %%mm7,                  %%mm7\n"
         "psllw          $3,                             %%mm5\n"  // GREEN 2
         "pand                   MMX16_redmask,    %%mm7\n"
         "pand                   MMX16_redmask,    %%mm3\n"
         "psrlw          $11,                            %%mm7\n"  // BLUE  2
         "pand                   MMX16_grnmask,    %%mm5\n"
         "por                    %%mm7,                  %%mm3\n"
	 
         "movq                   (%2),        %%mm7\n"  // L4 load lum2
	 "movl 16%5,%2\n"               // tmp0->register

         "por                    %%mm5,                  %%mm3\n"     //
         "psrlw          $8,                             %%mm7\n"    // L4
         "movq                   %%mm4,                  %%mm5\n"
         //                              "psubw          MMX16_10w,                        %%mm7\n"                // L4
         "punpcklwd      %%mm3,                  %%mm4\n"
         "pmullw         MMX16_Ycoeff,             %%mm7\n"    // lum4
         "punpckhwd      %%mm3,                  %%mm5\n"

         "movq                   %%mm4,                  (%3)\n" // write row1
         "movq                   %%mm5,                  8(%3)\n" // write row1

         "movq                   %%mm6,                  %%mm4\n"        // Lum3
         "paddw          %%mm0,                  %%mm6\n"                // Lum3 +blue

         "movq                   %%mm4,                  %%mm5\n"                        // Lum3
         "paddw          %%mm1,                  %%mm4\n"       // Lum3 +red
         "paddw          %%mm2,                  %%mm5\n"                        // Lum3 +green
         "psraw          $6,                             %%mm4\n"
         "movq                   %%mm7,                  %%mm3\n"                        // Lum4
         "psraw          $6,                             %%mm5\n"
         "paddw          %%mm0,                  %%mm7\n"                   // Lum4 +blue
         "psraw          $6,                             %%mm6\n"                        // Lum3 +blue
         "movq                   %%mm3,                  %%mm0\n"  // Lum4
         "packuswb       %%mm4,                  %%mm4\n"
         "paddw          %%mm1,                  %%mm3\n"  // Lum4 +red
         "packuswb       %%mm5,                  %%mm5\n"
         "paddw          %%mm2,                  %%mm0\n"         // Lum4 +green
         "packuswb       %%mm6,                  %%mm6\n"
         "punpcklbw      %%mm4,                  %%mm4\n"
         "punpcklbw      %%mm5,                  %%mm5\n"
         "punpcklbw      %%mm6,                  %%mm6\n"
         "psllw          $3,                             %%mm5\n" // GREEN 3
         "pand                   MMX16_redmask,    %%mm4\n"
         "psraw          $6,                             %%mm3\n" // psr 6
         "psraw          $6,                             %%mm0\n"
         "pand                   MMX16_redmask,    %%mm6\n" // BLUE
         "pand                   MMX16_grnmask,    %%mm5\n"
         "psrlw          $11,                            %%mm6\n"  // BLUE  3
         "por                    %%mm5,                  %%mm4\n"
         "psraw          $6,                             %%mm7\n"
         "por                    %%mm6,                  %%mm4\n"
         "packuswb       %%mm3,                  %%mm3\n"
         "packuswb       %%mm0,                  %%mm0\n"
         "packuswb       %%mm7,                  %%mm7\n"
         "punpcklbw      %%mm3,                  %%mm3\n"
         "punpcklbw      %%mm0,                  %%mm0\n"
         "punpcklbw      %%mm7,                  %%mm7\n"
         "pand                   MMX16_redmask,    %%mm3\n"
         "pand                   MMX16_redmask,    %%mm7\n" // BLUE
         "psllw          $3,                             %%mm0\n" // GREEN 4
         "psrlw          $11,                            %%mm7\n"
         "pand                   MMX16_grnmask,    %%mm0\n"
         "por                    %%mm7,                  %%mm3\n"
         "por                    %%mm0,                  %%mm3\n"

         "movq                   %%mm4,                  %%mm5\n"

         "punpcklwd      %%mm3,                  %%mm4\n"
         "punpckhwd      %%mm3,                  %%mm5\n"

         "movq                   %%mm4,                  (%4)\n"
	 "movq                   %%mm5,                  8(%4)\n"

         "subl      $8, 8%5\n" // x-=8
	 "addl      $8, %5\n"            // lum2+8
         "addl      $8, %2\n"
         "addl      $4, %0\n"
         "addl      $4, %1\n"
         "cmpl      $0, 8%5\n"
         "leal  16(%3), %3\n"
	 "leal  16(%4), %4\n"        // row2+16


         "jne            1b\n"
	 "addl           20%5,   %2\n" // lum += cols 

	 "movl %2,16%5\n"              // store register in tmp0
	 "movl 20%5,%2\n"              // cols->register

	 "addl           %2,     %5\n" // lum2 += cols 
	 "addl           12%5,   %3\n" // row1+= mod
	 "addl           12%5,   %4\n" // row2+= mod
	 "movl           %2,     8%5\n" // x=cols
	 "movl 16%5,%2\n"              // store tmp0 in register

	 "cmpl           4%5,    %2\n"
	 "jl             1b\n"

         :
         :"r" (cr), "r"(cb),"r"(lum),
	 "r"(row1),"r"(row2),"m"(buf[0])

         );
      __asm__ (
         "emms\n"
         );

   }

#endif
