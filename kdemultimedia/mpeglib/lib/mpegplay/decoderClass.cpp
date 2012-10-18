/*
  class for decoders
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "decoderClass.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif



#ifndef NDEBUG
#define NDEBUG
#endif
#include <assert.h>


#define DEBUG_DECODERCLASS(x)
//#define DEBUG_DECODERCLASS(x) x

/* Array mapping zigzag to array pointer offset. */


static const int zigzag_direct_nommx[64] = {
  0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12,
  19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28, 35,
  42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
  58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63};


static const int zigzag_direct_mmx[64] = {

    0*8+0/* 0*/, 1*8+0/* 1*/, 0*8+1/* 8*/, 0*8+2/*16*/, 1*8+1/* 9*/, 2*8+0/* 2*/, 3*8+0/* 3*/, 2*8+1/*10*/,
    1*8+2/*17*/, 0*8+3/*24*/, 0*8+4/*32*/, 1*8+3/*25*/, 2*8+2/*18*/, 3*8+1/*11*/, 4*8+0/* 4*/, 5*8+0/* 5*/,
    4*8+1/*12*/, 5*8+2/*19*/, 2*8+3/*26*/, 1*8+4/*33*/, 0*8+5/*40*/, 0*8+6/*48*/, 1*8+5/*41*/, 2*8+4/*34*/,
    3*8+3/*27*/, 4*8+2/*20*/, 5*8+1/*13*/, 6*8+0/* 6*/, 7*8+0/* 7*/, 6*8+1/*14*/, 5*8+2/*21*/, 4*8+3/*28*/,
    3*8+4/*35*/, 2*8+5/*42*/, 1*8+6/*49*/, 0*8+7/*56*/, 1*8+7/*57*/, 2*8+6/*50*/, 3*8+5/*43*/, 4*8+4/*36*/,
    5*8+3/*29*/, 6*8+2/*22*/, 7*8+1/*15*/, 7*8+2/*23*/, 6*8+3/*30*/, 5*8+4/*37*/, 4*8+5/*44*/, 3*8+6/*51*/,
    2*8+7/*58*/, 3*8+7/*59*/, 4*8+6/*52*/, 5*8+5/*45*/, 6*8+4/*38*/, 7*8+3/*31*/, 7*8+4/*39*/, 6*8+5/*46*/,
    7*8+6/*53*/, 4*8+7/*60*/, 5*8+7/*61*/, 6*8+6/*54*/, 7*8+5/*47*/, 7*8+6/*55*/, 6*8+7/*62*/, 7*8+7/*63*/
};





/* Bit masks used by bit i/o operations. */



static unsigned int bitMask[] = {0xffffffff,0x7fffffff,0x3fffffff,0x1fffffff, 
			    0x0fffffff, 0x07ffffff, 0x03ffffff, 0x01ffffff,
			    0x00ffffff, 0x007fffff, 0x003fffff, 0x001fffff,
			    0x000fffff, 0x0007ffff, 0x0003ffff, 0x0001ffff,
			    0x0000ffff, 0x00007fff, 0x00003fff, 0x00001fff,
			    0x00000fff, 0x000007ff, 0x000003ff, 0x000001ff,
			    0x000000ff, 0x0000007f, 0x0000003f, 0x0000001f,
			    0x0000000f, 0x00000007, 0x00000003, 0x00000001};

static unsigned int rBitMask[] = {0xffffffff,0xfffffffe,0xfffffffc,0xfffffff8, 
			    0xfffffff0, 0xffffffe0, 0xffffffc0, 0xffffff80, 
			    0xffffff00, 0xfffffe00, 0xfffffc00, 0xfffff800, 
			    0xfffff000, 0xffffe000, 0xffffc000, 0xffff8000, 
			    0xffff0000, 0xfffe0000, 0xfffc0000, 0xfff80000, 
			    0xfff00000, 0xffe00000, 0xffc00000, 0xff800000, 
			    0xff000000, 0xfe000000, 0xfc000000, 0xf8000000, 
			    0xf0000000, 0xe0000000, 0xc0000000, 0x80000000};

static unsigned int bitTest[] = { 0x80000000,0x40000000,0x20000000,0x10000000, 
			    0x08000000, 0x04000000, 0x02000000, 0x01000000,
			    0x00800000, 0x00400000, 0x00200000, 0x00100000,
			    0x00080000, 0x00040000, 0x00020000, 0x00010000,
			    0x00008000, 0x00004000, 0x00002000, 0x00001000,
			    0x00000800, 0x00000400, 0x00000200, 0x00000100,
			    0x00000080, 0x00000040, 0x00000020, 0x00000010,
			    0x00000008, 0x00000004, 0x00000002, 0x00000001};





DecoderClass::DecoderClass(VideoDecoder* vid_stream,
			   MpegVideoStream* mpegVideoStream) {

  this->vid_stream=vid_stream;
  this->mpegVideoStream=mpegVideoStream;


#ifdef INTEL
  lmmx=mm_support();
#else
  lmmx=false;
  DEBUG_DECODERCLASS(cout << "no INTEL arch- disable MMX in decoderClass"<<endl;)

#endif

  if (lmmx==true) {
    lmmx=4;
  }
  
  int i;
  for(i=0;i<64;i++) {
    zigzag_direct[i]=zigzag_direct_nommx[i];
  }  
  if (lmmx) {
    for(i=0;i<64;i++) {
      // they are different !!!
      zigzag_direct[i]=zigzag_direct_mmx[i];
    }
  } 
  for(i=64;i<256;i++) {
      zigzag_direct[i]=0;
  }
  resetDCT();
  reconptr = dct_recon[0];

}
 

DecoderClass::~DecoderClass() {
}
 


int DecoderClass::decodeDCTDCSizeLum() {
  unsigned int macro_val;
  unsigned int index;
  index=mpegVideoStream->showBits(5);
  if (index < 31) {
    macro_val = dct_dc_size_luminance[index].value;
    mpegVideoStream->flushBits(dct_dc_size_luminance[index].num_bits);
  } else {
    index=mpegVideoStream->showBits(9);	
    index -= 0x1f0;
    macro_val = dct_dc_size_luminance1[index].value;
    mpegVideoStream->flushBits(dct_dc_size_luminance1[index].num_bits);
  }	
  return macro_val;
}


int DecoderClass::decodeDCTDCSizeChrom() {
  unsigned int macro_val;
  unsigned int index;
  index=mpegVideoStream->showBits(5);
  if (index < 31) {
    macro_val = dct_dc_size_chrominance[index].value;
    mpegVideoStream->flushBits(dct_dc_size_chrominance[index].num_bits);
  }else {
    index=mpegVideoStream->showBits(10);
    index -= 0x3e0;
    macro_val = dct_dc_size_chrominance1[index].value;
    mpegVideoStream->flushBits(dct_dc_size_chrominance1[index].num_bits);
  }
  return macro_val;
}

/*
 *--------------------------------------------------------------
 *
 * DecodeMBAddrInc --
 *
 *      Huffman DecoderClass for macro_block_address_increment; the location
 *      in which the result will be placed is being passed as argument.
 *      The decoded value is obtained by doing a table lookup on
 *      mb_addr_inc.
 *
 * Results:
 *      The decoded value for macro_block_address_increment or MPGDECODE_ERROR
 *      for unbound values will be placed in the location specified.
 *
 * Side effects:
 *      Bit stream is irreversibly parsed.
 *
 *--------------------------------------------------------------
 */
int DecoderClass::decodeMBAddrInc() {
  unsigned int index;
  int val;
  index=mpegVideoStream->showBits(11);
  val = mb_addr_inc[index].value;
  mpegVideoStream->flushBits(mb_addr_inc[index].num_bits);
  if (mb_addr_inc[index].num_bits==0) {
    DEBUG_DECODERCLASS(cout<<"num_bits==0"<<endl;)
    val=1;
  }

  if (val == -1) {
    DEBUG_DECODERCLASS(cout <<"EROR: decodeMBAddrInc"<<endl;)
    val=MB_STUFFING;
  }
  return val;

}


/*
 *--------------------------------------------------------------
 *
 * DecodeMotionVectors --
 *
 *      Huffman DecoderClass for the various motion vectors, including
 *      motion_horizontal_forward_code, motion_vertical_forward_code,
 *      motion_horizontal_backward_code, motion_vertical_backward_code.
 *      Location where the decoded result will be placed is being passed
 *      as argument. The decoded values are obtained by doing a table
 *      lookup on motion_vectors.
 *
 * Results:
 *      The decoded value for the motion vector or MPGDECODE_ERROR for unbound
 *      values will be placed in the location specified.
 *
 * Side effects:
 *      Bit stream is irreversibly parsed.
 *
 *--------------------------------------------------------------
 */
int DecoderClass::decodeMotionVectors() {
  unsigned int index;
  int value;
  index=mpegVideoStream->showBits(11);
  value = motion_vectors[index].code;

  mpegVideoStream->flushBits(motion_vectors[index].num_bits);
  return value;
}


/*
 *--------------------------------------------------------------
 *
 * DecodeCBP --
 *
 *      Huffman DecoderClass for coded_block_pattern; location in which the
 *      decoded result will be placed is being passed as argument. The
 *      decoded values are obtained by doing a table lookup on
 *      coded_block_pattern.
 *
 * Results:
 *      The decoded value for coded_block_pattern or MPGDECODE_ERROR for unbound
 *      values will be placed in the location specified.
 *
 * Side effects:
 *      Bit stream is irreversibly parsed.
 *
 *--------------------------------------------------------------
 */
int DecoderClass::decodeCBP() {
  unsigned int index;
  unsigned int coded_bp;
  index=mpegVideoStream->showBits(9);
  coded_bp = coded_block_pattern[index].cbp;
  mpegVideoStream->flushBits(coded_block_pattern[index].num_bits);
  return coded_bp;
}







/*
 *--------------------------------------------------------------
 *
 * DecodeMBTypeB --
 *
 *      Huffman Decoder for macro_block_type in bidirectionally-coded
 *      pictures;locations in which the decoded results: macroblock_quant,
 *      macroblock_motion_forward, macro_block_motion_backward,
 *      macroblock_pattern, macro_block_intra, will be placed are
 *      being passed as argument. The decoded values are obtained by
 *      doing a table lookup on mb_type_B.
 *
 * Results:
 *      The various decoded values for macro_block_type in
 *      bidirectionally-coded pictures or MPGDECODE_ERROR for unbound values will
 *      be placed in the locations specified.
 *
 * Side effects:
 *      Bit stream is irreversibly parsed.
 *
 *--------------------------------------------------------------
 */
void DecoderClass::decodeMBTypeB(int& quant,int& motion_fwd,
				 int& motion_bwd,int& pat,int& intra){
  unsigned int index;							
									
  index=mpegVideoStream->showBits(6);
									
  quant = mb_type_B[index].mb_quant;					
  motion_fwd = mb_type_B[index].mb_motion_forward;			
  motion_bwd = mb_type_B[index].mb_motion_backward;			
  pat = mb_type_B[index].mb_pattern;					
  intra = mb_type_B[index].mb_intra;					
  if (index == 0) {
    DEBUG_DECODERCLASS(cout << "error in decodeMBTypeB"<<endl;)
  }
  mpegVideoStream->flushBits(mb_type_B[index].num_bits);	
}


/*
 *--------------------------------------------------------------
 *
 * DecodeMBTypeI --
 *
 *      Huffman Decoder for macro_block_type in intra-coded pictures;
 *      locations in which the decoded results: macroblock_quant,
 *      macroblock_motion_forward, macro_block_motion_backward,
 *      macroblock_pattern, macro_block_intra, will be placed are
 *      being passed as argument.
 *
 * Results:
 *      The various decoded values for macro_block_type in intra-coded
 *      pictures or MPGDECODE_ERROR for unbound values will be placed in the
 *      locations specified.
 *
 * Side effects:
 *      Bit stream is irreversibly parsed.
 *
 *--------------------------------------------------------------
 */
void DecoderClass::decodeMBTypeI(int& quant,int& motion_fwd,
				 int& motion_bwd,int& pat,int& intra) {

  unsigned int index;							
  static int quantTbl[4] = {MPGDECODE_ERROR, 1, 0, 0};				
									
  index=mpegVideoStream->showBits(2);
									
  motion_fwd = 0;							
  motion_bwd = 0;							
  pat = 0;								
  intra = 1;								
  quant = quantTbl[index];						
  if (quant == MPGDECODE_ERROR) {
    DEBUG_DECODERCLASS(cout << "decodeMBTypeI Error"<<endl;)
  }
  if (index) {								
    mpegVideoStream->flushBits (1 + quant);
  }									

}



/*
 *--------------------------------------------------------------
 *
 * DecodeMBTypeP --
 *
 *      Huffman Decoder for macro_block_type in predictive-coded pictures;
 *      locations in which the decoded results: macroblock_quant,
 *      macroblock_motion_forward, macro_block_motion_backward,
 *      macroblock_pattern, macro_block_intra, will be placed are
 *      being passed as argument. The decoded values are obtained by
 *      doing a table lookup on mb_type_P.
 * 
 * Results:
 *      The various decoded values for macro_block_type in
 *      predictive-coded pictures or MPGDECODE_ERROR for unbound values will be
 *      placed in the locations specified.
 *
 * Side effects:
 *      Bit stream is irreversibly parsed.
 *
 *--------------------------------------------------------------
 */
void DecoderClass::decodeMBTypeP(int& quant,int& motion_fwd,
				 int& motion_bwd,int& pat,int& intra) {

  unsigned int index;							
  
  index=mpegVideoStream->showBits(6);
  
  quant = mb_type_P[index].mb_quant;					
  motion_fwd = mb_type_P[index].mb_motion_forward;			
  motion_bwd = mb_type_P[index].mb_motion_backward;			
  pat = mb_type_P[index].mb_pattern;					
  intra = mb_type_P[index].mb_intra;					
  if (index == 0) {
    DEBUG_DECODERCLASS(cout << "error in decodeMBTypeP"<<endl;)
  }
  mpegVideoStream->flushBits(mb_type_P[index].num_bits);	
}

/*
 *--------------------------------------------------------------
 *
 * decodeDCTCoeff --
 *
 *      Huffman Decoder for dct_coeff_first and dct_coeff_next;
 *      locations where the results of decoding: run and level, are to
 *      be placed and also the type of DCT coefficients, either
 *      dct_coeff_first or dct_coeff_next, are being passed as argument.
 *      
 *      The decoder first examines the next 8 bits in the input stream,
 *      and perform according to the following cases:
 *      
 *      '0000 0000' - examine 8 more bits (i.e. 16 bits total) and
 *                    perform a table lookup on dct_coeff_tbl_0.
 *                    One more bit is then examined to determine the sign
 *                    of level.
 *
 *      '0000 0001' - examine 4 more bits (i.e. 12 bits total) and 
 *                    perform a table lookup on dct_coeff_tbl_1.
 *                    One more bit is then examined to determine the sign
 *                    of level.
 *      
 *      '0000 0010' - examine 2 more bits (i.e. 10 bits total) and
 *                    perform a table lookup on dct_coeff_tbl_2.
 *                    One more bit is then examined to determine the sign
 *                    of level.
 *
 *      '0000 0011' - examine 2 more bits (i.e. 10 bits total) and 
 *                    perform a table lookup on dct_coeff_tbl_3.
 *                    One more bit is then examined to determine the sign
 *                    of level.
 *
 *      otherwise   - perform a table lookup on dct_coeff_tbl. If the
 *                    value of run is not ESCAPE, extract one more bit
 *                    to determine the sign of level; otherwise 6 more
 *                    bits will be extracted to obtain the actual value 
 *                    of run , and then 8 or 16 bits to get the value of level.
 *                    
 *      
 *
 * Results:
 *      The decoded values of run and level or MPGDECODE_ERROR 
 *      for unbound values
 *      are placed in the locations specified.
 *
 * Side effects:
 *      Bit stream is irreversibly parsed.
 *
 *--------------------------------------------------------------
 */

void DecoderClass::decodeDCTCoeff(unsigned short int* dct_coeff_tbl,
				  unsigned RUNTYPE& run,int& level) {


  unsigned int temp, index;
  unsigned int value, next32bits, flushed;

    /*
   * Grab the next 32 bits and use it to improve performance of
   * getting the bits to parse. Thus, calls are translated as:
   *
   *	show_bitsX  <-->   next32bits >> (32-X)
   *	get_bitsX   <-->   val = next32bits >> (32-flushed-X);
   *			   flushed += X;
   *			   next32bits &= bitMask[flushed];
   *	flush_bitsX <-->   flushed += X;
   *			   next32bits &= bitMask[flushed];
   *
   */
  next32bits=mpegVideoStream->showBits32();

  flushed = 0;

  /* show_bits8(index); */
  index = next32bits >> 24;

  if (index > 3) {
    value = dct_coeff_tbl[index];
    run = (value & RUN_MASK) >> RUN_SHIFT;
    if (run == END_OF_BLOCK) {
      level = END_OF_BLOCK;
    }
    else {
      /* num_bits = (value & NUM_MASK) + 1; */
      /* flush_bits(num_bits); */
      flushed = (value & NUM_MASK) + 1;
      next32bits &= bitMask[flushed];
      if (run != ESCAPE) {
         level = (value & LEVEL_MASK) >> LEVEL_SHIFT;
	 /* get_bits1(value); */
	 /* if (value) *level = -*level; */
	 if (next32bits >> (31-flushed)) level = -level;
	 flushed++;
	 /* next32bits &= bitMask[flushed];  last op before update */
       }
       else {    /* *run == ESCAPE */
         /* get_bits14(temp); */
	 temp = next32bits >> (18-flushed);
	 flushed += 14;
	 next32bits &= bitMask[flushed];
	 run = temp >> 8;
	 temp &= 0xff;
	 if (temp == 0) {
            /* get_bits8(*level); */
	    level = next32bits >> (24-flushed);
	    flushed += 8;
	    /* next32bits &= bitMask[flushed];  last op before update */
 	    assert(level >= 128);
	 } else if (temp != 128) {
	    /* Grab sign bit */
	    level = ((int) (temp << 24)) >> 24;
	 } else {
            /* get_bits8(*level); */
	    level = next32bits >> (24-flushed);
	    flushed += 8;
	    /* next32bits &= bitMask[flushed];  last op before update */
	    level = level - 256;
	    assert(level <= -128 && level >= -255);
	 }
       }
       /* Update bitstream... */
       mpegVideoStream->flushBitsDirect(flushed);
    }
  }
  else {
    if (index == 2) { 
      /* show_bits10(index); */
      index = next32bits >> 22;
      value = dct_coeff_tbl_2[index & 3];
    }
    else if (index == 3) { 
      /* show_bits10(index); */
      index = next32bits >> 22;
      value = dct_coeff_tbl_3[index & 3];
    }
    else if (index) {	/* index == 1 */
      /* show_bits12(index); */
      index = next32bits >> 20;
      value = dct_coeff_tbl_1[index & 15];
    }
    else {   /* index == 0 */
      /* show_bits16(index); */
      index = next32bits >> 16;
      value = dct_coeff_tbl_0[index & 255];
    }
    run = (value & RUN_MASK) >> RUN_SHIFT;
    level = (value & LEVEL_MASK) >> LEVEL_SHIFT;

    /*
     * Fold these operations together to make it fast...
     */
    /* num_bits = (value & NUM_MASK) + 1; */
    /* flush_bits(num_bits); */
    /* get_bits1(value); */
    /* if (value) *level = -*level; */

    flushed = (value & NUM_MASK) + 2;
    if ((next32bits >> (32-flushed)) & 0x1) level = -level;

    /* Update bitstream ... */
    mpegVideoStream->flushBitsDirect(flushed);

  }

}


void DecoderClass::resetDCT() {
  /* Reset past dct dc y, cr, and cb values. */

  dct_dc_y_past = 1024 << 3;
  dct_dc_cr_past = 1024 << 3;
  dct_dc_cb_past = 1024 << 3;
  
}



/*
 *--------------------------------------------------------------
 *
 * ParseReconBlock --
 *
 *    Parse values for block structure from bitstream.
 *      n is an indication of the position of the block within
 *      the macroblock (i.e. 0-5) and indicates the type of 
 *      block (i.e. luminance or chrominance). Reconstructs
 *      coefficients from values parsed and puts in 
 *      block.dct_recon array in vid stream structure.
 *      sparseFlag is set when the block contains only one
 *      coeffictient and is used by the IDCT.
 *
 * Results:
 *    
 *
 * Side effects:
 *      Bit stream irreversibly parsed.
 *
 *--------------------------------------------------------------
 */


void DecoderClass::ParseReconBlock(int& n,int& mb_intra,
				   unsigned int& qscale,
				   int& lflag,
				   unsigned int* iqmatrixptr,
				   unsigned int* niqmatrixptr) {

   


  int coeffCount=0;
  if (mpegVideoStream->hasBytes(512) == false) {
    DEBUG_DECODERCLASS(cout << "cannot get 512 raw bytes"<<endl;)
    return;
  }

  {
    /*
     * Copy the VideoDecoder fields curBits, bitOffset, and bitBuffer
     * into local variables with the same names, so the macros use the
     * local variables instead.  This allows register allocation and
     * can provide 1-2 fps speedup.  On machines with not so many registers,
     * don't do this.
     */
    int size,  pos, coeff;
    int level;
    unsigned RUNTYPE run;
    unsigned RUNTYPE i;


    memset((char *) dct_recon, 0, 64*sizeof(short int));

    if (mb_intra) {
          
      if (n < 4) {
	
	/*
	 * Get the luminance bits.  This code has been hand optimized to
	 * get by the normal bit parsing routines.  We get some speedup
	 * by grabbing the next 16 bits and parsing things locally.
	 * Thus, calls are translated as:
	 *
	 *    show_bitsX  <-->   next16bits >> (16-X)
	 *    get_bitsX   <-->   val = next16bits >> (16-flushed-X);
	 *               flushed += X;
	 *               next16bits &= bitMask[flushed];
	 *    flush_bitsX <-->   flushed += X;
	 *               next16bits &= bitMask[flushed];
	 *
	 * I've streamlined the code a lot, so that we don't have to mask
	 * out the low order bits and a few of the extra adds are removed.
	 *    bsmith
	 */
	unsigned int next16bits, index, flushed;
        next16bits=mpegVideoStream->showBits16();

        index = next16bits >> (16-5);
        if (index < 31) {
          size = dct_dc_size_luminance[index].value;
          flushed = dct_dc_size_luminance[index].num_bits;
        } else {
          index = next16bits >> (16-9);
          index -= 0x1f0;
          size = dct_dc_size_luminance1[index].value;
          flushed = dct_dc_size_luminance1[index].num_bits;
        }
        next16bits &= bitMask[(16+flushed)&0x1f];
        if (size != 0) {
          flushed += size;
          coeff = next16bits >> (16-flushed);
          if (!(coeff & bitTest[32-size])) {
	    coeff++;
            coeff|= rBitMask[size&0x1f];
          }
	  coeff <<= 3;
        } else {
          coeff = 0;
        }
        mpegVideoStream->flushBitsDirect(flushed);
	
        if ( (n == 0) && (lflag) ) {
	  coeff += 1024;
	} else {
	  coeff += dct_dc_y_past;
	}
	dct_dc_y_past = coeff;

      } else { /* n = 4 or 5 */
	/*
	 * Get the chrominance bits.  This code has been hand optimized to
	 * as described above
	 */
	
	unsigned int next16bits, index, flushed;
        next16bits=mpegVideoStream->showBits16();
	
        index = next16bits >> (16-5);
        if (index < 31) {
          size = dct_dc_size_chrominance[index].value;
          flushed = dct_dc_size_chrominance[index].num_bits;
        } else {
          index = next16bits >> (16-10);
          index -= 0x3e0;
          size = dct_dc_size_chrominance1[index].value;
          flushed = dct_dc_size_chrominance1[index].num_bits;
        }
        next16bits &= bitMask[(16+flushed)&0x1f];

        if (size != 0) {
          flushed += size;
          coeff = next16bits >> (16-flushed);
          if (!(coeff & bitTest[32-size])) {
	    coeff++;
            coeff|=rBitMask[size&0x1f];
          }
	  coeff <<= 3;
        } else {
          coeff = 0;
        }
        mpegVideoStream->flushBitsDirect(flushed);
	
	/* We test 5 first; a result of the mixup of Cr and Cb */

        if (n == 5) {
	  if (lflag) {
	    coeff += 1024;
	  } else {
	    coeff += dct_dc_cr_past;
	  }
	  dct_dc_cr_past = coeff;
	} else {
          if (lflag) {
            coeff += 1024;
          } else {
            coeff += dct_dc_cb_past;
          }
          dct_dc_cb_past = coeff;
        }
      }

      coeff <<= lmmx;
      reconptr[0] = coeff;

      pos=0;
      i = 0; 
      coeffCount = (coeff != 0);


      // we never have d_types so there is no check
      

      while(1) {
	decodeDCTCoeff(dct_coeff_next,run,level);
	
	if (run >=END_OF_BLOCK) {
	  break;
	}
	i++;
	i+=run;
	pos = zigzag_direct[i&0x3f];


	coeff = (level * qscale * iqmatrixptr[pos]) >> 3 ;
	
	if (level < 0) {
	  coeff += (1 - (coeff & 1));
	} else {
	  coeff -= (1 - (coeff & 1));
	}	

	coeff <<= lmmx;

	reconptr[pos] = coeff;
	coeffCount++;
	
      }
      mpegVideoStream->flushBitsDirect(2);
      
      goto end;
    
    } else { /* non-intra-coded macroblock */

      decodeDCTCoeff(dct_coeff_first,run,level);
      i = run;

      pos = zigzag_direct[i&0x3f];
      
      /* quantizes and oddifies each coefficient */
      if (level < 0) {
        coeff = ((level - 1) * qscale * niqmatrixptr[pos]) >>3;
        if ((coeff & 1) == 0) {coeff++;}
      } else {    
        coeff = ((level + 1) * qscale * niqmatrixptr[pos]) >>3;
        coeff = (coeff-1) | 1; 
      }

      coeff <<= lmmx;

      reconptr[pos] = coeff;
      coeffCount = (coeff!=0);

      // we never have d_type pictures here, we do not support them
      
      while(1) {
	decodeDCTCoeff(dct_coeff_next,run,level);
	
	if (run >= END_OF_BLOCK) {
	  break;
	}
	i++;
	i+=run;
	pos = zigzag_direct[i&0x3f];

	if (level < 0) {
	  coeff = ((level - 1) * qscale * niqmatrixptr[pos]) >>3;
	  if ((coeff & 1) == 0) {coeff++;}
	} else {
	  coeff = ((level + 1) * qscale * niqmatrixptr[pos]) >> 3;
	  coeff = (coeff-1) | 1; 
	}

	coeff <<= lmmx;

	reconptr[pos] = coeff;
	coeffCount++;
      } /* end while */
      
      mpegVideoStream->flushBitsDirect(2);
      goto end;
    }
  
  end:
    if (coeffCount == 1) {
      if (lmmx) {
	emms();
	reconptr[pos]>>=lmmx;
	j_rev_dct_sparse (reconptr, pos);
	//IDCT_mmx(reconptr);
      } else {
	j_rev_dct_sparse (reconptr, pos);
      }

    } else {

      if (lmmx) {
	IDCT_mmx(reconptr);
      } else {
	j_rev_dct(reconptr);
      }

    }
  }

  // we call this at the edn of the "critical sections"
  /*
  if (lmmx) {
    emms();
  }
  */

  return;

}

    



void DecoderClass::print() {
  int i;
  for(i=0;i<64;i++) {
    printf(" %d ",zigzag_direct[i]);
  }  
  printf("\n");
}


















