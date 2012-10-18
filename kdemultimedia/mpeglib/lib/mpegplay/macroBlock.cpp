/*
  stores macroblock infos
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "macroBlock.h"

#define DEBUG_MACROBLOCK(x)
//#define DEBUG_MACROBLOCK(x) x



MacroBlock::MacroBlock(VideoDecoder* vid_stream) {
  this->vid_stream=vid_stream;
  copyFunctions=new CopyFunctions();
}


MacroBlock::~MacroBlock() {
  delete copyFunctions;
}


/*
 *--------------------------------------------------------------
 *
 * ParseMacroBlock --
 *
 *      Parseoff macroblock. Reconstructs DCT values. Applies
 *      inverse DCT, reconstructs motion vectors, calculates and
 *      set pixel values for macroblock in current pict image
 *      structure.
 *
 * Results:
 *      Here's where everything really happens. Welcome to the
 *      heart of darkness.
 *
 * Side effects:
 *      Bit stream irreversibly parsed off.
 *
 *--------------------------------------------------------------
 */

int MacroBlock::processMacroBlock(PictureArray* pictureArray) {
    unsigned int data;
  int recon_right_for, recon_down_for, recon_right_back,
      recon_down_back;
  int mb_quant = 0, mb_motion_forw = 0, mb_motion_back = 0, 
    mb_pattern = 0;

  int addr_incr;
  MpegVideoStream* mpegVideoStream=vid_stream->mpegVideoStream;
  DecoderClass* decoderClass=vid_stream->decoderClass;

  /*
   * Parse off macroblock address increment and add to macroblock address.
   */
  do {
    addr_incr=decoderClass->decodeMBAddrInc();
    if (addr_incr==MB_ESCAPE) {
      mb_address += 33;
      addr_incr=MB_STUFFING;
    }

  } while (addr_incr == MB_STUFFING);
  mb_address+=addr_incr;
    
      
  
  if (mb_address > (vid_stream->mpegVideoHeader)->getMB_Size()) {

    DEBUG_MACROBLOCK(cout <<"ParseMacroBlock: SKIP_TO_START_CODE"<<endl;)
    DEBUG_MACROBLOCK(cout <<"mb_address "<<mb_address<<endl;)
    
    int h=(vid_stream->mpegVideoHeader)->getMB_Height();
    int w=(vid_stream->mpegVideoHeader)->getMB_Width();
    

    DEBUG_MACROBLOCK(cout <<"mb_height*mb_width-1:"<<(h*w - 1)<<endl;)
    return false;
  }
  
  /*
   * If macroblocks have been skipped, process skipped macroblocks.
   */

  int code_type=(vid_stream->picture)->getCodeType();

  if (mb_address - past_mb_addr > 1) {

    processSkippedPictures(pictureArray,code_type,
			   (vid_stream->mpegVideoHeader)->getMB_Width());

  }


  /* Set past macroblock address to current macroblock address. */
  past_mb_addr = mb_address;
  /* Based on picture type decode macroblock type. */


  switch (code_type) {
  case I_TYPE:
    decoderClass->decodeMBTypeI(mb_quant, mb_motion_forw, 
				 mb_motion_back, mb_pattern,
				 mb_intra);
    break;

  case P_TYPE:
    decoderClass->decodeMBTypeP(mb_quant, mb_motion_forw, 
				mb_motion_back, mb_pattern,
				mb_intra);
    break;

  case B_TYPE:
    decoderClass->decodeMBTypeB(mb_quant, mb_motion_forw, 
				mb_motion_back, mb_pattern,
				mb_intra);
    break;
  case D_TYPE:
    DEBUG_MACROBLOCK(cout <<"ERROR:  MPEG-1 Streams with D-frames are not supported"<<endl;)
    return false;

  }
  /* If quantization flag set, parse off new quantization scale. */
  if (mb_quant == true) {
    data=mpegVideoStream->getBits(5);
    (vid_stream->slice)->setQuantScale(data);
  }
  /* If forward motion vectors exist... */

  if (mb_motion_forw == true) {
    // Parse off and decode horizontal forward motion vector. 
    motion_h_forw_code=decoderClass->decodeMotionVectors();

    // If horiz. forward r data exists, parse off. 

    if ((vid_stream->picture->getForw_f() != 1) &&
	(motion_h_forw_code != 0)) {
      data=vid_stream->picture->geth_forw_r(mpegVideoStream);
      motion_h_forw_r = data;
    }
    // Parse off and decode vertical forward motion vector. 
    
    motion_v_forw_code=decoderClass->decodeMotionVectors();

    // If vert. forw. r data exists, parse off. 

    if ((vid_stream->picture->getForw_f() != 1) &&
	(motion_v_forw_code != 0)) {

      data=vid_stream->picture->getv_forw_r(mpegVideoStream);
      motion_v_forw_r = data;
    }
  }

  /* If back motion vectors exist... */

  if (mb_motion_back == true) {
    // Parse off and decode horiz. back motion vector. 
    motion_h_back_code=decoderClass->decodeMotionVectors();

    // If horiz. back r data exists, parse off. 

    if ((vid_stream->picture->getBack_f() != 1) &&
	(motion_h_back_code != 0)) {
      data=vid_stream->picture->geth_back_r(mpegVideoStream);
      motion_h_back_r = data;
    }
    // Parse off and decode vert. back motion vector. 
    motion_v_back_code=decoderClass->decodeMotionVectors();

    // If vert. back r data exists, parse off. 

    if ((vid_stream->picture->getBack_f() != 1) &&
	(motion_v_back_code != 0)) {
      data=vid_stream->picture->getv_back_r(mpegVideoStream);
      motion_v_back_r = data;
    }
  }

  /* If mblock pattern flag set, parse and decode CBP (code block pattern). */
  if (mb_pattern == true) {
    cbp=decoderClass->decodeCBP();
  }
  /* Otherwise, set CBP to zero. */
  else
    cbp = 0;



  /* Reconstruct motion vectors depending on picture type. */
  if (code_type == P_TYPE) {

    /*
     * If no forw motion vectors, reset previous and current vectors to 0.
     */
    if (!mb_motion_forw) {
      recon_right_for = 0;
      recon_down_for = 0;
      recon_right_for_prev = 0;
      recon_down_for_prev = 0;
    }
    /*
     * Otherwise, compute new forw motion vectors. Reset previous vectors to
     * current vectors.
     */

    else {
      computeForwVector(&recon_right_for, &recon_down_for);

    }
  }
  if (code_type == B_TYPE) {

    /* Reset prev. and current vectors to zero if mblock is intracoded. */
    if (mb_intra) {
      recon_right_for_prev = 0;
      recon_down_for_prev = 0;
      recon_right_back_prev = 0;
      recon_down_back_prev = 0;
    } else {
      
     /* If no forw vectors, current vectors equal prev. vectors. */
      
      if (!mb_motion_forw) {
	recon_right_for = recon_right_for_prev;
	recon_down_for = recon_down_for_prev;
      }
      /*
       * Otherwise compute forw. vectors. Reset prev vectors to new values.
       */
      
      else {
	computeForwVector(&recon_right_for, &recon_down_for);

     }
      
      /* If no back vectors, set back vectors to prev back vectors. */
      
      if (!mb_motion_back) {
        recon_right_back = recon_right_back_prev;
        recon_down_back = recon_down_back_prev;
      }
      /* Otherwise compute new vectors and reset prev. back vectors. */

      else {
        computeBackVector(&recon_right_back,&recon_down_back);

      }
  
      /*
       * Store vector existence flags in structure for possible skipped
       * macroblocks to follow.
       */

      bpict_past_forw = mb_motion_forw;
      bpict_past_back = mb_motion_back;
    }
  }
  int back;
  back=reconstruct(recon_right_for,
		   recon_down_for,
		   recon_right_back,
		   recon_down_back,
		   mb_motion_forw,
		   mb_motion_back,
		   pictureArray);
  

  /* If D Type picture, flush marker bit. */
  if (code_type == D_TYPE) {
    mpegVideoStream->flushBits(1);
  }

  /* If macroblock was intracoded, set macroblock past intra address. */
  if (mb_intra) {
    past_intra_addr=mb_address;
  }
  if (back == false) {
    return false;
  }
  return true;
}
  

int MacroBlock::resetMacroBlock() {
  /* Reset past intrablock address. */
  past_intra_addr = -2;

  /* Reset previous recon motion vectors. */
  

  recon_right_for_prev = 0;
  recon_down_for_prev = 0;
  recon_right_back_prev = 0;
  recon_down_back_prev = 0;

  /* Reset macroblock address. */
  mb_address = (((vid_stream->slice)->getVertPos()-1) *
		(vid_stream->mpegVideoHeader)->getMB_Width()) - 1;
  return true;

}


int MacroBlock::resetPastMacroBlock() {
  /* Reset past macroblock address field. */

  past_mb_addr = -1;
  return true;
}




int MacroBlock::reconstruct(int& recon_right_for,
			    int& recon_down_for,
			    int& recon_right_back,
			    int& recon_down_back,
			    int& mb_motion_forw,
			    int& mb_motion_back,
			    PictureArray* pictureArray) {
  int mask, i;
  int zero_block_flag;
  int mb_row;
  int mb_col;
  int mb_width=(vid_stream->mpegVideoHeader)->getMB_Width();
  int row_size=pictureArray->getWidth();
  short int* dct_start=(vid_stream->decoderClass)->getDCT();
  unsigned int qscale=(vid_stream->slice)->getQuantScale();
  int codeType=(vid_stream->picture)->getCodeType();
  DecoderClass* decoderClass=vid_stream->decoderClass;
  int lflag=false;
  Recon* recon=vid_stream->recon;
  unsigned int* iqmatrixptr=
    (vid_stream->mpegVideoHeader)->getIntra_quant_matrix();
  unsigned int* niqmatrixptr=
    (vid_stream->mpegVideoHeader)->getNon_intra_quant_matrix();





  if (mb_address-past_intra_addr > 1) {
    lflag=true;
  }

  if (mb_width <= 0) {
     DEBUG_MACROBLOCK(cout << "mb_width <= 0"<<endl;)
     return false;
  }
  /* Calculate macroblock row and column from address. */
  mb_row=mb_address / mb_width;
  mb_col=mb_address % mb_width;

  copyFunctions->startNOFloatSection();

  for (mask = 32, i = 0; i < 6; mask >>= 1, i++) {

     
    /* If block exists... */
    if ((mb_intra) || (cbp & mask)) {
      zero_block_flag = 0;
      //copyFunctions->endNOFloatSection();
      decoderClass->ParseReconBlock(i,mb_intra,
				    qscale,lflag,iqmatrixptr,niqmatrixptr);
      //copyFunctions->endNOFloatSection();
    } else {
      zero_block_flag = 1;
    }


    // If macroblock is intra coded... 


    if (mb_intra) {
      recon->ReconIMBlock(i,mb_row,mb_col,row_size,
			  dct_start,pictureArray);
      //copyFunctions->endNOFloatSection();
    } else if (mb_motion_forw && mb_motion_back) {
      recon->ReconBiMBlock(i,recon_right_for,
			   recon_down_for,recon_right_back,
			   recon_down_back,zero_block_flag,
			   mb_row,mb_col,row_size,dct_start,
			   pictureArray);
      //copyFunctions->endNOFloatSection();
    } else if (mb_motion_forw || (codeType ==P_TYPE)){
      recon->ReconPMBlock(i,recon_right_for,
			  recon_down_for,zero_block_flag,
			  mb_row,mb_col,row_size,dct_start,
			  pictureArray, codeType);
      //copyFunctions->endNOFloatSection();
    } else if (mb_motion_back) {
      recon->ReconBMBlock(i,recon_right_back,
			  recon_down_back,zero_block_flag,
			  mb_row,mb_col,row_size,dct_start,
			  pictureArray);
      //copyFunctions->endNOFloatSection();
      
    } else {
      //DEBUG_MACROBLOCK(cout << "nothing"<<endl;)
    }
    
  }
  copyFunctions->endNOFloatSection();
  return true;
}

/*
 *--------------------------------------------------------------
 *
 * ComputeForwVector --
 *
 *	Computes forward motion vector by calling ComputeVector
 *      with appropriate parameters.
 *
 * Results:
 *	Reconstructed motion vector placed in recon_right_for_ptr and
 *      recon_down_for_ptr.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */

void MacroBlock::computeForwVector(int* recon_right_for_ptr,
				   int* recon_down_for_ptr) {
  
  Picture *picture;


  picture = vid_stream->picture;


  unsigned int forw_f=picture->getForw_f();
  unsigned int full_pel_forw_vector=picture->getFull_pel_forw_vector();
  vid_stream->motionVector->computeVector(recon_right_for_ptr, 
					  recon_down_for_ptr,
					  recon_right_for_prev, 
					  recon_down_for_prev,
					  forw_f,
					  full_pel_forw_vector,
					  motion_h_forw_code, 
					  motion_v_forw_code,
					  motion_h_forw_r,
					  motion_v_forw_r); 
  picture->setForw_f(forw_f);
  picture->setFull_pel_forw_vector(full_pel_forw_vector);
}


/*
 *--------------------------------------------------------------
 *
 * ComputeBackVector --
 *
 *	Computes backward motion vector by calling ComputeVector
 *      with appropriate parameters.
 *
 * Results:
 *	Reconstructed motion vector placed in recon_right_back_ptr and
 *      recon_down_back_ptr.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */

void MacroBlock::computeBackVector(int* recon_right_back_ptr,
				   int* recon_down_back_ptr) {

  Picture *picture;


  picture = vid_stream->picture;


  unsigned int back_f=picture->getBack_f();
  unsigned int full_pel_back_vector=picture->getFull_pel_back_vector();
  
  vid_stream->motionVector->computeVector(recon_right_back_ptr, 
					  recon_down_back_ptr,
					  recon_right_back_prev, 
					  recon_down_back_prev,
					  back_f, 
					  full_pel_back_vector,
					  motion_h_back_code, 
					  motion_v_back_code,
					  motion_h_back_r,
					  motion_v_back_r); 
  picture->setBack_f(back_f);
  picture->setFull_pel_back_vector(full_pel_back_vector);
}





int MacroBlock::processSkippedPictures(PictureArray* pictureArray,
				       int code_type,
				       int mb_width) {
  copyFunctions->startNOFloatSection();
 
  if (code_type == P_TYPE) {
    
    ProcessSkippedPFrameMBlocks(pictureArray->getCurrent(),
				pictureArray->getFuture(),
				mb_width);
    
  } else {
    if (code_type == B_TYPE) {
      ProcessSkippedBFrameMBlocks(vid_stream->picture,
				  pictureArray->getPast(),
				  pictureArray->getCurrent(),
				  pictureArray->getFuture(),
				  mb_width);
    }
  }

  copyFunctions->endNOFloatSection();

  return true;
}

/*
 *--------------------------------------------------------------
 *
 * ProcessSkippedPFrameMBlocks --
 *
 *	Processes skipped macroblocks in P frames.
 *
 * Results:
 *	Calculates pixel values for luminance, Cr, and Cb planes
 *      in current pict image for skipped macroblocks.
 *
 * Side effects:
 *	Pixel values in pict image changed.
 *
 *--------------------------------------------------------------
 */
void MacroBlock::ProcessSkippedPFrameMBlocks(YUVPicture* current,
					     YUVPicture* future,
					     int mb_width) {

  int row_size, half_row, mb_row, mb_col, row, col, rr;
  int addr, row_incr, half_row_incr, crow, ccol;
  int *dest, *src, *dest1, *src1;

  /* For each row in macroblock luminance plane... */
  if (mb_width == 0) {
    DEBUG_MACROBLOCK(cout << "mb_width in skipped is 0"<<endl;)
    return;
  }


  /* Calculate row sizes for luminance and Cr/Cb macroblock areas. */

  row_size = mb_width << 4;
  half_row = (row_size >> 1);
  row_incr = row_size >> 2;
  half_row_incr = half_row >> 2;

  /* For each skipped macroblock, do... */
  int lumEnd=current->getLumLength();
  int colorEnd=current->getColorLength();
  
  unsigned char *picDest;
  unsigned char *picSrc;
 
  unsigned char *picDestStart;
  unsigned char *picSrcStart;



  for (addr = past_mb_addr + 1; addr < mb_address; addr++) {

    /* Calculate macroblock row and col. */

    mb_row = addr / mb_width;
    mb_col = addr % mb_width;

    /* Calculate upper left pixel row,col for luminance plane. */

    row = mb_row << 4;
    col = mb_col << 4;
    
    picDest=current->getLuminancePtr();
    picSrc=future->getLuminancePtr();

    picDestStart=(picDest+(row*row_size)+col);
    picSrcStart=(picSrc+(row*row_size)+col);
   
    if ((picDestStart+7*row_size+7 >= picDest+lumEnd) ||
	(picDestStart < picDest)) {
      DEBUG_MACROBLOCK(cout << "urg! last resort caught before sigsev skipped -1"<<endl;)
      break;
    }
    if ((picSrcStart+7*row_size+7 >= picSrc+lumEnd) ||
	(picSrcStart < picSrc)) {
      DEBUG_MACROBLOCK(cout << "urg! last resort caught before sigsev skipped -2"<<endl;)
      break;
    }

    dest=(int*)picDestStart;
    src=(int*)picSrcStart;



    for (rr = 0; rr < 8; rr++) {

      /* Copy pixel values from last I or P picture. */
      memcpy(dest,src,sizeof(int)*4);

      dest += row_incr;
      src += row_incr;
      memcpy(dest,src,sizeof(int)*4);

      dest += row_incr;
      src += row_incr;
    }

    /*
     * Divide row,col to get upper left pixel of macroblock in Cr and Cb
     * planes.
     */

    crow = row >> 1;
    ccol = col >> 1;

    /* For each row in Cr, and Cb planes... */
    picDest=current->getCrPtr();
    picDestStart=(picDest+(crow*half_row)+ccol);
    if ((picDestStart+7*half_row_incr+7 >= picDest+colorEnd) ||
	(picDestStart < picDest)) {
      DEBUG_MACROBLOCK(cout << "urg! last resort caught before sigsev skipped -3"<<endl;)
      break;
    }


    dest=(int*)(current->getCrPtr()+(crow*half_row)+ccol);
    src=(int*)(future->getCrPtr()+(crow*half_row)+ccol);
    dest1=(int*)(current->getCbPtr()+(crow*half_row)+ccol);
    src1=(int*)(future->getCbPtr()+(crow*half_row)+ccol);

    for (rr = 0; rr < 4; rr++) {

      /* Copy pixel values from last I or P picture. */
      memcpy(dest,src,sizeof(int)*2);
      memcpy(dest1,src1,sizeof(int)*2);


      dest += half_row_incr;
      src += half_row_incr;
      dest1 += half_row_incr;
      src1 += half_row_incr;

      memcpy(dest,src,sizeof(int)*2);
      memcpy(dest1,src1,sizeof(int)*2);

      dest += half_row_incr;
      src += half_row_incr;
      dest1 += half_row_incr;
      src1 += half_row_incr;
    }

  }

  recon_right_for_prev = 0;
  recon_down_for_prev = 0;

}
  




/*
 *--------------------------------------------------------------
 *
 * ProcessSkippedBFrameMBlocks --
 *
 *	Processes skipped macroblocks in B frames.
 *
 * Results:
 *	Calculates pixel values for luminance, Cr, and Cb planes
 *      in current pict image for skipped macroblocks.
 *
 * Side effects:
 *	Pixel values in pict image changed.
 *
 *--------------------------------------------------------------
 */

void MacroBlock::ProcessSkippedBFrameMBlocks(Picture* picture,
					     YUVPicture* past,
					     YUVPicture* current,
					     YUVPicture* future,
					     int mb_width) {
  int row_size, half_row, mb_row, mb_col, row, col, rr;
  int right_half_for = 0, down_half_for = 0;
  int c_right_half_for = 0, c_down_half_for = 0;
  int right_half_back = 0, down_half_back = 0;
  int c_right_half_back = 0, c_down_half_back = 0;
  int addr, right_for = 0, down_for = 0;
  int recon_right_for, recon_down_for;
  int recon_right_back, recon_down_back;
  int right_back = 0, down_back = 0;
  int c_right_for = 0, c_down_for = 0;
  int c_right_back = 0, c_down_back = 0;
  unsigned char forw_lum[256];
  unsigned char forw_cr[64], forw_cb[64];
  unsigned char back_lum[256], back_cr[64], back_cb[64];
  int row_incr, half_row_incr;
  int ccol, crow;


  /* Calculate row sizes for luminance and Cr/Cb macroblock areas. */

  if (mb_width == 0) {
    DEBUG_MACROBLOCK(cout << "mb_width in skipped is 0 (2)"<<endl;)
    return;
  }

  row_size = mb_width << 4;
  half_row = (row_size >> 1);
  row_incr = row_size >> 2;
  half_row_incr =  half_row >> 2;

  /* Establish motion vector codes based on full pixel flag. */

  if (picture->getFull_pel_forw_vector()) {
    recon_right_for = recon_right_for_prev << 1;
    recon_down_for = recon_down_for_prev << 1;
  } else {
    recon_right_for = recon_right_for_prev;
    recon_down_for = recon_down_for_prev;
  }

  if (picture->getFull_pel_back_vector()) {
    recon_right_back = recon_right_back_prev << 1;
    recon_down_back = recon_down_back_prev << 1;
  } else {
    recon_right_back = recon_right_back_prev;
    recon_down_back = recon_down_back_prev;
  }


  /* If only one motion vector, do display copy, else do full
     calculation. 
  */

  /* Calculate motion vectors. */
  
  if (bpict_past_forw) {
    right_for = recon_right_for >> 1;
    down_for = recon_down_for >> 1;
    right_half_for = recon_right_for & 0x1;
    down_half_for = recon_down_for & 0x1;
    
    recon_right_for /= 2;
    recon_down_for /= 2;
    c_right_for = recon_right_for >> 1;
    c_down_for = recon_down_for >> 1;
    c_right_half_for = recon_right_for & 0x1;
    c_down_half_for = recon_down_for & 0x1;
    
  }
  if (bpict_past_back) {
    right_back = recon_right_back >> 1;
    down_back = recon_down_back >> 1;
    right_half_back = recon_right_back & 0x1;
    down_half_back = recon_down_back & 0x1;
    
    recon_right_back /= 2;
    recon_down_back /= 2;
    c_right_back = recon_right_back >> 1;
    c_down_back = recon_down_back >> 1;
    c_right_half_back = recon_right_back & 0x1;
    c_down_half_back = recon_down_back & 0x1;
    
  }
  /* For each skipped macroblock, do... */
  
  for (addr = past_mb_addr + 1;
       addr < mb_address; addr++) {
    
    /* Calculate macroblock row and col. */
    
    mb_row = addr / mb_width;
    mb_col = addr % mb_width;
    
    /* Calculate upper left pixel row,col for luminance plane. */
    
    row = mb_row << 4;
    col = mb_col << 4;
    crow = row / 2;
    ccol = col / 2;
    
    /* If forward predicted, calculate prediction values. */
    if (bpict_past_forw) {
      int lumEnd=current->getLumLength();
      int colorEnd=current->getColorLength();
      ReconSkippedBlock(past->getLuminancePtr(),
			forw_lum,row,col,row_size,
			right_for,down_for,
			right_half_for,
			down_half_for,16,lumEnd);
      ReconSkippedBlock(past->getCrPtr(),
			forw_cr,crow,ccol, half_row,
			c_right_for,c_down_for,
			c_right_half_for,
			c_down_half_for,8,colorEnd);
      ReconSkippedBlock(past->getCbPtr(),
			forw_cb,crow,ccol,half_row,
			c_right_for,c_down_for,
			c_right_half_for,
			c_down_half_for,8,colorEnd);
    }
    /* If back predicted, calculate prediction values. */
    
    if (bpict_past_back) {
      int lumEnd=current->getLumLength();
      int colorEnd=current->getColorLength();
      ReconSkippedBlock(future->getLuminancePtr(),
			back_lum,row,col,row_size,
			right_back,down_back,
			right_half_back,down_half_back,
			16,lumEnd);
      ReconSkippedBlock(future->getCrPtr(),
			back_cr,crow,ccol,
			half_row,c_right_back,
			c_down_back,c_right_half_back,
			c_down_half_back,8,colorEnd);
      ReconSkippedBlock(future->getCbPtr(),
			back_cb,crow,ccol,half_row,
			c_right_back,c_down_back,
			c_right_half_back,
			c_down_half_back,8,colorEnd);
    }
    unsigned char* picDest=current->getLuminancePtr();
    int lumEnd=current->getLumLength();
    int colorEnd=current->getColorLength();
 
    unsigned char* picDestStart=(picDest+(row*row_size)+col);

   
    if ((picDestStart+7*row_size+7 >= picDest+lumEnd) ||
	(picDestStart < picDest)) {
      DEBUG_MACROBLOCK(cout << "urg! last resort caught before sigsev skipped -4"<<endl;)
      return;
    }

    picDest=current->getCrPtr();
    picDestStart=(picDest+(crow*half_row)+ccol);
    if ((picDestStart+7*half_row_incr+7 >= picDest+colorEnd) ||
	(picDestStart < picDest)) {
      DEBUG_MACROBLOCK(cout << "urg! last resort caught before sigsev skipped -5"<<endl;)
      exit(0);
    }
    

    if (bpict_past_forw && !bpict_past_back) {
      
      int *dest, *dest1;
      int *src, *src1;
      dest=(int*)(current->getLuminancePtr()+(row*row_size)+col);
      src=(int*)forw_lum;
      
      for (rr = 0; rr < 16; rr++) {
	
	/* memcpy(dest, forw_lum+(rr<<4), 16);  */
	
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
	dest[3] = src[3];
	dest += row_incr;
	src += 4;
      }
      
      dest = (int*)(current->getCrPtr()+(crow*half_row)+ccol);
      dest1 = (int*)(current->getCbPtr()+(crow*half_row)+ccol);
      src = (int*)forw_cr;
      src1 = (int*)forw_cb;
      
      for (rr = 0; rr < 8; rr++) {
	/*
	 * memcpy(dest, forw_cr+(rr<<3), 8); memcpy(dest1, forw_cb+(rr<<3),
	 * 8);
	 */
	
	dest[0] = src[0];
	dest[1] = src[1];
	
	dest1[0] = src1[0];
	dest1[1] = src1[1];
	
	dest += half_row_incr;
	dest1 += half_row_incr;
	src += 2;
	src1 += 2;
      }
    } else if (bpict_past_back && !bpict_past_forw) {
      
      int *src, *src1;
      int *dest, *dest1;
      dest=(int*)(current->getLuminancePtr()+(row*row_size)+col);
      src = (int*)back_lum;
      
      for (rr = 0; rr < 16; rr++) {
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
	dest[3] = src[3];
	dest += row_incr;
	src += 4;
      }
      
      
      dest = (int *)(current->getCrPtr()+(crow*half_row)+ccol);
      dest1 = (int *)(current->getCbPtr()+(crow*half_row)+ccol);
      src = (int *)back_cr;
      src1 = (int *)back_cb;
      
      for (rr = 0; rr < 8; rr++) {
	/*
	 * memcpy(dest, back_cr+(rr<<3), 8); memcpy(dest1, back_cb+(rr<<3),
	 * 8);
	 */
	
	dest[0] = src[0];
	dest[1] = src[1];
	
	dest1[0] = src1[0];
	dest1[1] = src1[1];
	
	dest += half_row_incr;
	dest1 += half_row_incr;
	src += 2;
	src1 += 2;
      }
    } else {
      
      unsigned char *src1, *src2, *src1a, *src2a;
      unsigned char *dest, *dest1;
      dest = current->getLuminancePtr()+(row*row_size)+col;
      src1 = forw_lum;
      src2 = back_lum;
      
      for (rr = 0; rr < 16; rr++) {
        dest[0] = (int) (src1[0] + src2[0]) >> 1;
        dest[1] = (int) (src1[1] + src2[1]) >> 1;
        dest[2] = (int) (src1[2] + src2[2]) >> 1;
        dest[3] = (int) (src1[3] + src2[3]) >> 1;
        dest[4] = (int) (src1[4] + src2[4]) >> 1;
        dest[5] = (int) (src1[5] + src2[5]) >> 1;
        dest[6] = (int) (src1[6] + src2[6]) >> 1;
        dest[7] = (int) (src1[7] + src2[7]) >> 1;
        dest[8] = (int) (src1[8] + src2[8]) >> 1;
        dest[9] = (int) (src1[9] + src2[9]) >> 1;
        dest[10] = (int) (src1[10] + src2[10]) >> 1;
        dest[11] = (int) (src1[11] + src2[11]) >> 1;
        dest[12] = (int) (src1[12] + src2[12]) >> 1;
        dest[13] = (int) (src1[13] + src2[13]) >> 1;
        dest[14] = (int) (src1[14] + src2[14]) >> 1;
        dest[15] = (int) (src1[15] + src2[15]) >> 1;
        dest += row_size;
        src1 += 16;
        src2 += 16;
      }
      
      
      dest = current->getCrPtr() + (crow * half_row) + ccol;
      dest1 = current->getCbPtr() + (crow * half_row) + ccol;
      src1 = forw_cr;
      src2 = back_cr;
      src1a = forw_cb;
      src2a = back_cb;
      
      for (rr = 0; rr < 8; rr++) {
        dest[0] = (int) (src1[0] + src2[0]) >> 1;
        dest[1] = (int) (src1[1] + src2[1]) >> 1;
        dest[2] = (int) (src1[2] + src2[2]) >> 1;
        dest[3] = (int) (src1[3] + src2[3]) >> 1;
        dest[4] = (int) (src1[4] + src2[4]) >> 1;
        dest[5] = (int) (src1[5] + src2[5]) >> 1;
        dest[6] = (int) (src1[6] + src2[6]) >> 1;
        dest[7] = (int) (src1[7] + src2[7]) >> 1;
        dest += half_row;
        src1 += 8;
        src2 += 8;
	
        dest1[0] = (int) (src1a[0] + src2a[0]) >> 1;
        dest1[1] = (int) (src1a[1] + src2a[1]) >> 1;
        dest1[2] = (int) (src1a[2] + src2a[2]) >> 1;
        dest1[3] = (int) (src1a[3] + src2a[3]) >> 1;
        dest1[4] = (int) (src1a[4] + src2a[4]) >> 1;
        dest1[5] = (int) (src1a[5] + src2a[5]) >> 1;
        dest1[6] = (int) (src1a[6] + src2a[6]) >> 1;
        dest1[7] = (int) (src1a[7] + src2a[7]) >> 1;
        dest1 += half_row;
        src1a += 8;
        src2a += 8;
      }
    }
    
  }
}







/*
 *--------------------------------------------------------------
 *
 * ReconSkippedBlock --
 *
 *	Reconstructs predictive block for skipped macroblocks
 *      in B Frames.
 *
 * Results:
 *	No return values.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */
void MacroBlock::ReconSkippedBlock(unsigned char* source,
				   unsigned char* dest,
				   int row,
				   int col,
				   int row_size,
				   int right,
				   int down,
				   int right_half,
				   int down_half,
				   int width,int maxLen) {
  int rr;
  unsigned char *source2;
  unsigned char *tmp;

  tmp = source+((row + down) * row_size) + col + right;


  if ((tmp+7*row_size+7 >= source+maxLen) ||
      (tmp < source)) {
    DEBUG_MACROBLOCK(cout << "urg! last resort caught before sigsev skipped -6"<<endl;)
    return;
  } 

  source=tmp;
  if (width == 16) {
    if ((!right_half) && (!down_half)) {
	if (right & 0x1) {
	  /* No alignment, use bye copy */
	  for (rr = 0; rr < 16; rr++) {

	    memcpy(dest,source,sizeof(char)*16);

	    dest += 16;
	    source += row_size;
	  }
	} else if (right & 0x2) {
	  /* Half-word bit aligned, use 16 bit copy */
	  short *src = (short *)source;
	  short *d = (short *)dest;
	  row_size >>= 1;
	  for (rr = 0; rr < 16; rr++) {

	    memcpy(d,src,sizeof(short)*8);
    
	    d += 8;
	    src += row_size;
	  }
	} else {
	  /* Word aligned, use 32 bit copy */
	  int *src = (int *)source;
	  int *d = (int *)dest;
	  row_size >>= 2;
	  for (rr = 0; rr < 16; rr++) {
	    d[0] = src[0];
	    d[1] = src[1];
	    d[2] = src[2];
	    d[3] = src[3];
	    d += 4;
	    src += row_size;
	  }
	}
    } else {
      source2 = source + right_half + (row_size * down_half);
      copyFunctions->copy16_div2_destlinear_nocrop(source,source2,dest,
						   row_size);


    }
  } else {			/* (width == 8) */
    assert(width == 8);
    if ((!right_half) && (!down_half)) {
      if (right & 0x1) {
	for (rr = 0; rr < width; rr++) {

	  memcpy(dest,source,sizeof(char)*8);

	  dest += 8;
	  source += row_size;
	}
      } else if (right & 0x02) {
	short *d = (short *)dest;
	short *src = (short *)source;
	row_size >>= 1;
	for (rr = 0; rr < width; rr++) {
	  d[0] = src[0];
	  d[1] = src[1];
	  d[2] = src[2];
	  d[3] = src[3];
	  d += 4;
	  src += row_size;
	}
      } else {
	int *d = (int *)dest;
	int *src = (int *)source;
	row_size >>= 2;
	for (rr = 0; rr < width; rr++) {
	  d[0] = src[0];
	  d[1] = src[1];
	  d += 2;
	  src += row_size;
	}
      }
    } else {
      source2 = source + right_half + (row_size * down_half);
      copyFunctions->copy8_div2_destlinear_nocrop(source,source2,
						  dest,row_size);
    }
  }
}

