/*
  stores macroblock infos
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __MACROBLOCK_H
#define __MACROBLOCK_H

#include "videoDecoder.h"
#include "decoderClass.h"
#include "motionVector.h"
#include "recon.h"
#include "copyFunctions.h"

class MacroBlock {
  
  /* Macroblock structure. */

  int mb_address;                        /* Macroblock address.              */
  int past_mb_addr;                      /* Previous mblock address.         */
  int motion_h_forw_code;                /* Forw. horiz. motion vector code. */
  unsigned int motion_h_forw_r;          /* Used in decoding vectors.        */
  int motion_v_forw_code;                /* Forw. vert. motion vector code.  */
  unsigned int motion_v_forw_r;          /* Used in decdoinge vectors.       */
  int motion_h_back_code;                /* Back horiz. motion vector code.  */
  unsigned int motion_h_back_r;          /* Used in decoding vectors.        */
  int motion_v_back_code;                /* Back vert. motion vector code.   */
  unsigned int motion_v_back_r;          /* Used in decoding vectors.        */
  unsigned int cbp;                      /* Coded block pattern.             */
  int mb_intra;                          /* Intracoded mblock flag.          */
  int bpict_past_forw;                   /* Past B frame forw. vector flag.  */
  int bpict_past_back;                   /* Past B frame back vector flag.   */
  int past_intra_addr;                   /* Addr of last intracoded mblock.  */
  int recon_right_for_prev;              /* Past right forw. vector.         */
  int recon_down_for_prev;               /* Past down forw. vector.          */
  int recon_right_back_prev;             /* Past right back vector.          */
  int recon_down_back_prev;              /* Past down back vector.           */

  class VideoDecoder* vid_stream;
  CopyFunctions* copyFunctions;

 public:
  MacroBlock(class VideoDecoder* vid_stream);
  ~MacroBlock();
  int processMacroBlock(PictureArray* pictureArray);
  int resetMacroBlock();
  int resetPastMacroBlock();

 private:
  int reconstruct(int& recon_right_for,
		  int& recon_down_for,
		  int& recon_right_back,
		  int& recon_down_back,
		  int& mb_motion_forw,
		  int& mb_motion_back,
		  PictureArray* pictureArray);

  void computeForwVector(int* recon_right_for_ptr,
			 int* recon_down_for_ptr);

  void computeBackVector(int* recon_right_back_ptr,
			 int* recon_down_back_ptr);

  int processSkippedPictures(PictureArray* pictureArray,
			     int code_type,
			     int mb_width);

  void ProcessSkippedPFrameMBlocks(YUVPicture* current,
				   YUVPicture* future,
				   int mb_width);
  

  void ProcessSkippedBFrameMBlocks(Picture* picture,
				   YUVPicture* past,
				   YUVPicture* current,
				   YUVPicture* future,
				   int mb_width);


  void ReconSkippedBlock(unsigned char* source,unsigned char* dest,
			 int row, int col,int row_size,
			 int right,int down,
			 int right_half,int down_half,int width,
			 int maxLen);
  
    
};
#endif
