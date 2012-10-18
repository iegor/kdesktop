/*
  class for decoders
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __DECODERCLASS_H
#define __DECODERCLASS_H


 
#include "videoDecoder.h"
#include "mpegVideoStream.h"
#include "decoderTables.h"
#include "slice.h"
#include "proto.h"
#include "../util/mmx/mmx.h"
#include "mmxidct.h"
#include "picture.h"

/* Special values for DCT Coefficients */
#define END_OF_BLOCK 62
#define ESCAPE 61


/* DCT coeff tables. */
#define RUN_MASK 0xfc00
#define LEVEL_MASK 0x03f0
#define NUM_MASK 0x000f
#define RUN_SHIFT 10
#define LEVEL_SHIFT 4
#define RUNTYPE char

class DecoderClass {

  int lmmx;
  int zigzag_direct[256];

  /* Block structure. */

  short int dct_recon[8][8];             /* Reconstructed dct coeff matrix. */
  int dct_dc_y_past;                     /* Past lum. dc dct coefficient.   */
  int dct_dc_cr_past;                    /* Past cr dc dct coefficient.     */
  int dct_dc_cb_past;                    /* Past cb dc dct coefficient.     */
  short int *reconptr;                   /* reconptr = dct_recon[0];        */

  class VideoDecoder* vid_stream;
  class MpegVideoStream* mpegVideoStream;
 public:
  DecoderClass(class VideoDecoder* vid_stream,
	       class MpegVideoStream* mpegVideoStream);
  ~DecoderClass();
  int decodeDCTDCSizeLum();
  int decodeDCTDCSizeChrom();
  int decodeMotionVectors();
  int decodeCBP();

  inline short int* getDCT() { return ((short int*) &(dct_recon[0][0]));}
  void resetDCT();

  void decodeMBTypeB(int& quant,int& motion_fwd,
		     int& motion_bwd,int& pat,int& intra);

  void decodeMBTypeI(int& quant,int& motion_fwd,
		     int& motion_bwd,int& pat,int& intra);
  
  void decodeMBTypeP(int& quant,int& motion_fwd,
		     int& motion_bwd,int& pat,int& intra);

  void ParseReconBlock(int& n,int& mb_intra,unsigned int& quant_scale,
		       int& lflag,
		       unsigned int* iqmatrixptr,
		       unsigned int* niqmatrixptr);
  int decodeMBAddrInc();


  void print();

 private:


  inline void decodeDCTCoeff(unsigned short int* dct_coeff_tbl,
			     unsigned RUNTYPE& run,
			     int& level);

 

};
#endif
