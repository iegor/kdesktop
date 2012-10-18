/*
  stores heavily used copy functions (makes mmx support easier)
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "copyFunctions.h"


/*
 * We use a lookup table to make sure values stay in the 0..255 range.
 * Since this is cropping (ie, x = (x < 0)?0:(x>255)?255:x; ), wee call this
 * table the "crop table".
 * MAX_NEG_CROP is the maximum neg/pos value we can handle.
 */
/*
 * We use a lookup table to make sure values stay in the 0..255 range.
 * Since this is cropping (ie, x = (x < 0)?0:(x>255)?255:x; ), wee call this
 * table the "crop table".
 * MAX_NEG_CROP is the maximum neg/pos value we can handle.
 */

// Compiler cannot allocate too big arrays.




CopyFunctions::CopyFunctions() {
  /* Initialize crop table. */
  cropTbl=new unsigned char[NUM_CROP_ENTRIES];

  int i;

  for (i = (-MAX_NEG_CROP); i < NUM_CROP_ENTRIES - MAX_NEG_CROP; i++) {
    if (i <= 0) {
      cropTbl[i + MAX_NEG_CROP] = 0;
    } else if (i >= 255) {
      cropTbl[i + MAX_NEG_CROP] = 255;
    } else {
      cropTbl[i + MAX_NEG_CROP] = i;
    }
  }
  cm=cropTbl + MAX_NEG_CROP;

  copyFunctions_asm = new CopyFunctions_MMX();
  lmmx=copyFunctions_asm->support();
  


}


CopyFunctions::~CopyFunctions() {
  delete cropTbl;
}

void CopyFunctions::startNOFloatSection() {
  // nothing
  copyFunctions_asm->startNOFloatSection();
}


void CopyFunctions::endNOFloatSection() {
  copyFunctions_asm->endNOFloatSection();

}


void CopyFunctions::copy8_byte(unsigned char* source1,
			       unsigned char* dest,int inc) {
  if (lmmx == false) {
    int rr;
    
    for (rr = 0; rr < 8; rr++) {
      memcpy(dest,source1,sizeof(char)*8);
      source1+=inc;
      dest+=inc;
    }    

  } else {
    copyFunctions_asm->copy8_byte(source1,dest,inc);
  }
      

}

void CopyFunctions::copy8_word(unsigned short* source1,
			       unsigned short* dest,int inc) {
  int rr;

  // Optimisation is slower, leave it in C
  for (rr = 0; rr < 8; rr++) {
    memcpy(dest,source1,sizeof(short)*8);
    source1+=inc;
    dest+=inc;
  }    

}


 
void CopyFunctions::copy8_src1linear_crop(short* source1,
					  unsigned char* dest,int inc) {

  if (lmmx == false) {
    int rr;
    
    for (rr = 0; rr < 8; rr++) {
      
      dest[0] = cm[source1[0]];
      dest[1] = cm[source1[1]];
      dest[2] = cm[source1[2]];
      dest[3] = cm[source1[3]];
      dest[4] = cm[source1[4]];
      dest[5] = cm[source1[5]];
      dest[6] = cm[source1[6]];
      dest[7] = cm[source1[7]];

      
      dest += inc;
      source1 += 8;
      
    }
  } else {
    copyFunctions_asm->copy8_src1linear_crop(source1,dest,inc);
  }

}

void CopyFunctions::copy8_div2_nocrop(unsigned char* source1,
				      unsigned char* source2,
				      unsigned char* dest,int inc) {
  if (lmmx == false) {
    int rr;
    for (rr = 0; rr < 8; rr++) {
      
      dest[0] = (int) (source1[0] + source2[0]+1) >> 1;
      dest[1] = (int) (source1[1] + source2[1]+1) >> 1;
      dest[2] = (int) (source1[2] + source2[2]+1) >> 1;
      dest[3] = (int) (source1[3] + source2[3]+1) >> 1;
      dest[4] = (int) (source1[4] + source2[4]+1) >> 1;
      dest[5] = (int) (source1[5] + source2[5]+1) >> 1;
      dest[6] = (int) (source1[6] + source2[6]+1) >> 1;
      dest[7] = (int) (source1[7] + source2[7]+1) >> 1;
      dest += inc;
      source1 += inc;
      source2 += inc;
    }
  } else {
    copyFunctions_asm->copy8_div2_nocrop(source1,source2, dest, inc);
  }

}

void CopyFunctions::copy8_div2_destlinear_nocrop(unsigned char* source1,
						 unsigned char* source2,
						 unsigned char* dest,int inc) {
  
  if (lmmx == false) {
    int rr;
    for (rr = 0; rr < 8; rr++) {
      dest[0] = (int) (source1[0] + source2[0]) >> 1;
      dest[1] = (int) (source1[1] + source2[1]) >> 1;
      dest[2] = (int) (source1[2] + source2[2]) >> 1;
      dest[3] = (int) (source1[3] + source2[3]) >> 1;
      dest[4] = (int) (source1[4] + source2[4]) >> 1;
      dest[5] = (int) (source1[5] + source2[5]) >> 1;
      dest[6] = (int) (source1[6] + source2[6]) >> 1;
      dest[7] = (int) (source1[7] + source2[7]) >> 1;
      dest += 8;
      source1 += inc;
      source2 += inc;
    }
  } else {
    copyFunctions_asm->copy8_div2_destlinear_nocrop(source1,source2,dest,inc);
  }
}
  

void CopyFunctions::copy16_div2_destlinear_nocrop(unsigned char* source1,
						  unsigned char* source2,
						  unsigned char* dest,int inc){

  if (lmmx == false) {
    int rr;
    for (rr = 0; rr < 16; rr++) {
      dest[0] = (int) (source1[0] + source2[0]) >> 1;
      dest[1] = (int) (source1[1] + source2[1]) >> 1;
      dest[2] = (int) (source1[2] + source2[2]) >> 1;
      dest[3] = (int) (source1[3] + source2[3]) >> 1;
      dest[4] = (int) (source1[4] + source2[4]) >> 1;
      dest[5] = (int) (source1[5] + source2[5]) >> 1;
      dest[6] = (int) (source1[6] + source2[6]) >> 1;
      dest[7] = (int) (source1[7] + source2[7]) >> 1;
      dest[8] = (int) (source1[8] + source2[8]) >> 1;
      dest[9] = (int) (source1[9] + source2[9]) >> 1;
      dest[10] = (int) (source1[10] + source2[10]) >> 1;
      dest[11] = (int) (source1[11] + source2[11]) >> 1;
      dest[12] = (int) (source1[12] + source2[12]) >> 1;
      dest[13] = (int) (source1[13] + source2[13]) >> 1;
      dest[14] = (int) (source1[14] + source2[14]) >> 1;
      dest[15] = (int) (source1[15] + source2[15]) >> 1;
      dest += 16;
      source1 += inc;
      source2 += inc;
    }
  } else {
    copyFunctions_asm->copy16_div2_destlinear_nocrop(source1,source2,dest,inc);
  }   

}
  


void CopyFunctions::copy8_div4_nocrop(unsigned char* source1,
				      unsigned char* source2,
				      unsigned char* source3,
				      unsigned char* source4,
				      unsigned char* dest,int inc) {
  int rr;

  for (rr = 0; rr < 8; rr++) {
    dest[0]=(int) (source1[0]+source2[0]+source3[0]+source4[0] + 2) >> 2;
    dest[1]=(int) (source1[1]+source2[1]+source3[1]+source4[1] + 2) >> 2;
    dest[2]=(int) (source1[2]+source2[2]+source3[2]+source4[2] + 2) >> 2;
    dest[3]=(int) (source1[3]+source2[3]+source3[3]+source4[3] + 2) >> 2;
    dest[4]=(int) (source1[4]+source2[4]+source3[4]+source4[4] + 2) >> 2;
    dest[5]=(int) (source1[5]+source2[5]+source3[5]+source4[5] + 2) >> 2;
    dest[6]=(int) (source1[6]+source2[6]+source3[6]+source4[6] + 2) >> 2;
    dest[7]=(int) (source1[7]+source2[7]+source3[7]+source4[7] + 2) >> 2;
    dest += inc;
    source1 += inc;
    source2 += inc;
    source3 += inc;
    source4 += inc;
  }
}

// Optimize me!
// should be mmx perfomance analysis shows: 8 % overall time

void CopyFunctions::copy8_src2linear_crop(unsigned char* source1,
					  short int* source2,
					  unsigned char* dest,int inc) {
  int rr;
  if (lmmx == false) {
    for (rr = 0; rr < 8; rr++) {
      dest[0] = cm[(int) source1[0] + (int) source2[0]];
      dest[1] = cm[(int) source1[1] + (int) source2[1]];
      dest[2] = cm[(int) source1[2] + (int) source2[2]];
      dest[3] = cm[(int) source1[3] + (int) source2[3]];
      dest[4] = cm[(int) source1[4] + (int) source2[4]];
      dest[5] = cm[(int) source1[5] + (int) source2[5]];
      dest[6] = cm[(int) source1[6] + (int) source2[6]];
      dest[7] = cm[(int) source1[7] + (int) source2[7]];
      dest += inc;
      source1 += inc;
      source2 += 8;
    }
  } else {
    copyFunctions_asm->copy8_src2linear_crop(source1,source2,dest,inc);
  }

}

// Optimize me!
// should be mmx perfomance analysis shows: 13 % overall time
void CopyFunctions::copy8_div2_src3linear_crop(unsigned char* source1,
					       unsigned char* source2,
					       short int* source3,
					       unsigned char* dest,int inc) {
  int rr;
  if (lmmx==false) {
    for (rr = 0; rr < 8; rr++) {
      dest[0] = cm[((int) (source1[0] + source2[0]+1) >> 1) + source3[0]];
      dest[1] = cm[((int) (source1[1] + source2[1]+1) >> 1) + source3[1]];
      dest[2] = cm[((int) (source1[2] + source2[2]+1) >> 1) + source3[2]];
      dest[3] = cm[((int) (source1[3] + source2[3]+1) >> 1) + source3[3]];
      dest[4] = cm[((int) (source1[4] + source2[4]+1) >> 1) + source3[4]];
      dest[5] = cm[((int) (source1[5] + source2[5]+1) >> 1) + source3[5]];
      dest[6] = cm[((int) (source1[6] + source2[6]+1) >> 1) + source3[6]];
      dest[7] = cm[((int) (source1[7] + source2[7]+1) >> 1) + source3[7]];
      dest += inc;
      source1 += inc;
      source2 += inc;
      source3 += 8;
      
    }
  } else {
    copyFunctions_asm->copy8_div2_src3linear_crop(source1,source2,source3,
						  dest,inc);
  }


}


void CopyFunctions::copy8_div4_src5linear_crop(unsigned char* source1,
					       unsigned char* source2,
					       unsigned char* source3,
					       unsigned char* source4,
					       short int* source5,
					       unsigned char* dest,int inc) {

  int rr;

  for (rr = 0; rr < 8; rr++) {
    dest[0]=cm[((int) (source1[0]+source2[0]+source3[0]+source4[0]+2) >> 2) + source5[0]];
    dest[1]=cm[((int) (source1[1]+source2[1]+source3[1]+source4[1]+2) >> 2) + source5[1]];
    dest[2]=cm[((int) (source1[2]+source2[2]+source3[2]+source4[2]+2) >> 2) + source5[2]];
    dest[3]=cm[((int) (source1[3]+source2[3]+source3[3]+source4[3]+2) >> 2) + source5[3]];
    dest[4]=cm[((int) (source1[4]+source2[4]+source3[4]+source4[4]+2) >> 2) + source5[4]];
    dest[5]=cm[((int) (source1[5]+source2[5]+source3[5]+source4[5]+2) >> 2) + source5[5]];
    dest[6]=cm[((int) (source1[6]+source2[6]+source3[6]+source4[6]+2) >> 2) + source5[6]];
    dest[7]=cm[((int) (source1[7]+source2[7]+source3[7]+source4[7]+2) >> 2) + source5[7]];
    dest +=inc;
    source1 += inc;
    source2 += inc;
    source3 += inc;
    source4 += inc;
    source5 += 8;
  }
}
