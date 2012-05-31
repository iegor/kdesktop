/*
  class for reconstruction
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "recon.h"


#define DEBUG_RECON(x)
//#define DEBUG_RECON(x) x



Recon::Recon() {
  copyFunctions=new CopyFunctions();
}


Recon::~Recon() {
  delete copyFunctions;
}


/*
 *--------------------------------------------------------------
 *
 * ReconIMBlock --
 *
 *	Reconstructs intra coded macroblock.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

int Recon::ReconIMBlock(int bnum,int mb_row,int mb_col,int row_size,
			short int* dct_start,PictureArray* pictureArray) {
  int row, col;
  unsigned char *dest;
  unsigned char *picDest;
  int lumLength=(pictureArray->getCurrent())->getLumLength();
  int colorLength=(pictureArray->getCurrent())->getColorLength();
  int endDest=0;



  /* If block is luminance block... */

  if (bnum < 4) {

    /* Calculate row and col values for upper left pixel of block. */

    row = mb_row << 4;
    col = mb_col << 4;
    if (bnum > 1)
      row += 8;
    if (bnum % 2)
      col += 8;

    /* Set dest to luminance plane of current pict image. */

    picDest = (pictureArray->getCurrent())->getLuminancePtr();
    endDest=lumLength;

  }
  /* Otherwise if block is Cr block... */
  /* Cr first because of the earlier mixup */

  else if (bnum == 5) {

    /* Set dest to Cr plane of current pict image. */

    picDest = (pictureArray->getCurrent())->getCrPtr();
    endDest=colorLength;

    /* Establish row size. yuv color is half of whole size*/
    row_size >>=1;

    /* Calculate row,col for upper left pixel of block. */

    row = mb_row << 3;
    col = mb_col << 3;
  }
  /* Otherwise block is Cb block, and ... */

  else {

    /* Set dest to Cb plane of current pict image. */

    picDest = (pictureArray->getCurrent())->getCbPtr();
    endDest=colorLength;

    
    /* Establish row size. yuv color is half of whole size*/

    row_size /=2;

    /* Calculate row,col for upper left pixel value of block. */

    row = mb_row << 3;
    col = mb_col << 3;
  }

  
  /*
   * For each pixel in block, set to cropped reconstructed value from inverse
   * dct.
   */

  dest = picDest+row * row_size + col;
  
  
  if ((dest+7*row_size+7 >= picDest+endDest) || (dest < picDest)) {
    DEBUG_RECON(cout << "urg! last resort caught before sigsev -4"<<endl;)
    return false;
  }
  
  
  copyFunctions->copy8_src1linear_crop(dct_start,dest,row_size);


  return true;
}



/*
 *--------------------------------------------------------------
 *
 * ReconPMBlock --
 *
 *	Reconstructs forward predicted macroblocks.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */

int Recon::ReconPMBlock(int bnum,
			int recon_right_for,
			int recon_down_for,
			int zflag,
			int mb_row,int mb_col,
			int row_size,short int* dct_start,
			PictureArray* pictureArray,int codeType) {
  int row, col, rr;
  unsigned char *picDest, *past;
  unsigned char *rindex1, *rindex2, *rindex3, *rindex4;
  unsigned char *index;
  int lumLength=(pictureArray->getCurrent())->getLumLength();
  int colorLength=(pictureArray->getCurrent())->getColorLength();
  int endPast=0;
  int endDest=0;


  int right_for;
  int down_for;
  int right_half_for;
  int down_half_for;



  picDest=0;

  if (bnum < 4) {
    
    
    
    /* Set dest to luminance plane of current pict image. */

    picDest = (pictureArray->getCurrent())->getLuminancePtr();
    endDest=lumLength;

    if (codeType == B_TYPE) {
       past = (pictureArray->getPast())->getLuminancePtr();
    } else {

      /* Set predictive frame to current future frame. */

      past = (pictureArray->getFuture())->getLuminancePtr();
    }
    endPast=lumLength;


    /* Calculate row,col of upper left pixel in block. */

    row = mb_row << 4;
    col = mb_col << 4;
    if (bnum > 1)
      row += 8;
    if (bnum % 2)
      col += 8;

  /* Otherwise, block is NOT luminance block, ... */

  } else {
    
    /* Construct motion vectors. */

    recon_right_for >>= 1;
    recon_down_for >>= 1;
    
    /* Establish row size. yuv color is half of whole size*/

    row_size /= 2;

    
    /* Calculate row,col of upper left pixel in block. */
    
    row = mb_row << 3;
    col = mb_col << 3;
    
    /* If block is Cr block... */
    /* 5 first because order was mixed up in earlier versions */
    
    if (bnum == 5) {
      
      /* Set dest to Cr plane of current pict image. */
      
     picDest = (pictureArray->getCurrent())->getCrPtr();
     
     if (codeType == B_TYPE) {
       
       past = (pictureArray->getPast())->getCrPtr();
     } else {
       past = (pictureArray->getFuture())->getCrPtr();
     }
    }
    /* Otherwise, block is Cb block... */
    
    else {
      
      /* Set dest to Cb plane of current pict image. */
      
      picDest = (pictureArray->getCurrent())->getCbPtr();
      
      if (codeType == B_TYPE) {
	past = (pictureArray->getPast())->getCbPtr();
      } else {
	past = (pictureArray->getFuture())->getCbPtr();
      }
    }
    endPast=colorLength;
    endDest=colorLength;
    
  }

  /* Calculate right_back, down_back motion vectors. */
  right_for = recon_right_for >> 1;
  down_for = recon_down_for >> 1;
  right_half_for = recon_right_for & 0x1;
  down_half_for = recon_down_for & 0x1;

  
  /* For each pixel in block... */

  index = picDest + (row * row_size) + col;
  rindex1 = past + (row + down_for) * row_size + col + right_for;




  if ((rindex1+7*row_size+7 >= past+endPast) || (rindex1 < past)) {
    DEBUG_RECON(cout << "urg! last resort caught before sigsev -1"<<endl;)
    return false;
  }
  if ((index+7*row_size+7 >= picDest+endDest) || (index < picDest)) {
    DEBUG_RECON(cout << "urg! last resort caught before sigsev -2"<<endl;)
    return false;
  }

 
  /*
   * Calculate predictive pixel value based on motion vectors and copy to
   * dest plane.
   */
  
  if ((!down_half_for) && (!right_half_for)) {

    if (!zflag) {
      copyFunctions->copy8_src2linear_crop(rindex1,dct_start,index,row_size);
    } else {
      if (right_for & 0x1) {

	/* No alignment, used byte copy */
	copyFunctions->copy8_byte(rindex1,index,row_size);


      } else if (right_for & 0x2) {
	/* Half-word bit aligned, use 16 bit copy */
	unsigned short *src = (unsigned short *)rindex1;
	unsigned short *dest = (unsigned short *)index;
	row_size >>= 1;
	copyFunctions->copy8_word(src,dest,row_size);

      } else {
	/* Word aligned, use 32 bit copy */
	int *src = (int*) rindex1;
	int *dest = (int*) index;

	row_size >>= 2;


	for (rr = 0; rr < 8; rr++) {
	  dest[0] = src[0];
	  dest[1] = src[1];
	  dest += row_size;
	  src += row_size;
	}
	
      }
    }
  } else {
    rindex2 = rindex1 + right_half_for + (down_half_for * row_size);
    
    /* if one of the two is zero, then quality makes no difference */
    if ((!right_half_for) || (!down_half_for) || (!qualityFlag)) {
      
      if (!zflag) {
	// was +1
	copyFunctions->copy8_div2_src3linear_crop(rindex1,rindex2,dct_start,
						  index,row_size);
      } else { /* zflag */
	// was +1
	copyFunctions->copy8_div2_nocrop(rindex1,rindex2,index,row_size);
      }
    } else { /* qualityFlag on and both vectors are non-zero */
      rindex3 = rindex1 + right_half_for;
      rindex4 = rindex1 + (down_half_for * row_size);
      if (!zflag) {
	copyFunctions->copy8_div4_src5linear_crop(rindex1,rindex2,rindex3,
						  rindex4,dct_start,
						  index,row_size);
      } else { /* zflag */
	copyFunctions->copy8_div4_nocrop(rindex1,rindex2,rindex3,rindex4,
					 index,row_size);
      }
    }
    
  }
  
  return true;
}


/*
 *--------------------------------------------------------------
 *
 * ReconBMBlock --
 *
 *	Reconstructs back predicted macroblocks.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */

int Recon::ReconBMBlock(int bnum,
			int recon_right_back,
			int recon_down_back,
			int zflag,
			int mb_row,int mb_col,
			int row_size,short int* dct_start,
			PictureArray* pictureArray) {
  int row, col, rr;
  unsigned char *dest, *future;
  int right_back, down_back, right_half_back, down_half_back;
  unsigned char *rindex1, *rindex2, *rindex3, *rindex4;
  unsigned char *index;
  int lumLength=(pictureArray->getCurrent())->getLumLength();
  int colorLength=(pictureArray->getCurrent())->getColorLength();
  
  int endFuture=0;
  int endDest=0;


  /* If block is luminance block... */

  if (bnum < 4) {


    /* Set dest to luminance plane of current pict image. */

    dest = (pictureArray->getCurrent())->getLuminancePtr();
    endDest=lumLength;
    /*
     * If future frame exists, set future to luminance plane of future frame.
     */

    future = (pictureArray->getFuture())->getLuminancePtr();
    endFuture=lumLength;

    /* Calculate row,col of upper left pixel in block. */

    row = mb_row << 4;
    col = mb_col << 4;
    if (bnum > 1)
      row += 8;
    if (bnum % 2)
      col += 8;


  }
  /* Otherwise, block is NOT luminance block, ... */

  else {

    /* Construct motion vectors. */

    recon_right_back >>= 1;
    recon_down_back >>= 1;

    /* Establish row size. yuv color is half of whole size*/

    row_size >>= 1;

    /* Calculate row,col of upper left pixel in block. */

    row = mb_row << 3;
    col = mb_col << 3;


    /* If block is Cr block... */
    /* They were switched earlier, so 5 is first - eyhung */

    if (bnum == 5) {

      /* Set dest to Cr plane of current pict image. */

      dest = (pictureArray->getCurrent())->getCrPtr();

      /*
       * If future frame exists, set future to Cr plane of future image.
       */

      future = (pictureArray->getFuture())->getCrPtr();
    }
    /* Otherwise, block is Cb block... */

    else {

      /* Set dest to Cb plane of current pict image. */

      dest = (pictureArray->getCurrent())->getCbPtr();

      /*
       * If future frame exists, set future to Cb plane of future frame.
       */

      future = (pictureArray->getFuture())->getCbPtr();
    }
    endDest=colorLength;
    endFuture=colorLength;
  }

  /* Calculate right_back, down_back motion vectors. */
  right_back = recon_right_back >> 1;
  down_back = recon_down_back >> 1;
  right_half_back = recon_right_back & 0x1;
  down_half_back = recon_down_back & 0x1;

  /* For each pixel in block do... */
  
  index = dest + (row * row_size) + col;
  rindex1 = future + (row + down_back) * row_size + col + right_back;

  if ((index+7*row_size+7 >= dest+endDest) || (index < dest)) {
    DEBUG_RECON(cout << "urg! last resort -9"<<endl;)
    return false;
  }
  if ((rindex1+7*row_size+7 >= future+endFuture) || (rindex1 < future)) {
    DEBUG_RECON(cout << "urg! last resort -8"<<endl;)
    return false;
  }

  if ((!right_half_back) && (!down_half_back)) {
    if (!zflag) {
      copyFunctions->copy8_src2linear_crop(rindex1,dct_start,index,row_size);
    } else {
      if (right_back & 0x1) {
	/* No alignment, use byte copy */

	copyFunctions->copy8_byte(rindex1,index,row_size);
	
      } else if (right_back & 0x2) {
	/* Half-word bit aligned, use 16 bit copy */
	unsigned short *src = (unsigned short *)rindex1;
	unsigned short *dest = (unsigned short *)index;
	row_size >>= 1;
	copyFunctions->copy8_word(src,dest,row_size);

      } else {
	/* Word aligned, use 32 bit copy */
	int *src = (int *)rindex1;
	int *dest = (int *)index;
	row_size >>= 2;
	for (rr = 0; rr < 8; rr++) {
	  dest[0] = src[0];
	  dest[1] = src[1];
	  dest += row_size;
	  src += row_size;
	}
      }
    }
  } else {
    rindex2 = rindex1 + right_half_back + (down_half_back * row_size);
    if (!qualityFlag) {
      
      if (!zflag) {
	// was +1
	copyFunctions->copy8_div2_src3linear_crop(rindex1,rindex2,dct_start,
						  index,row_size);
      } else { /* zflag */
	// was +1
	copyFunctions->copy8_div2_nocrop(rindex1,rindex2,index,row_size);
      }
    } else { /* qualityFlag on */
      rindex3 = rindex1 + right_half_back;
      rindex4 = rindex1 + (down_half_back * row_size);
      if (!zflag) {
	copyFunctions->copy8_div4_src5linear_crop(rindex1,rindex2,rindex3,
						  rindex4,dct_start,
						  index,row_size);
      } else { /* zflag */
	copyFunctions->copy8_div4_nocrop(rindex1,rindex2,rindex3,rindex4,
					 index,row_size);
      }
    }
    
  }
  return true;
}


/*
 *--------------------------------------------------------------
 *
 * ReconBiMBlock --
 *
 *	Reconstructs bidirectionally predicted macroblocks.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */
int Recon::ReconBiMBlock(int bnum,
			 int recon_right_for,
			 int recon_down_for,
			 int recon_right_back,
			 int recon_down_back,
			 int zflag,int mb_row,
			 int mb_col,int row_size,short int* dct_start,
			 PictureArray* pictureArray) {
  int row, col;
  unsigned char *dest, *past=NULL, *future=NULL;
  int right_for, down_for, right_half_for, down_half_for;
  int right_back, down_back, right_half_back, down_half_back;
  unsigned char *index, *rindex1, *bindex1;
  int forw_row_start, back_row_start, forw_col_start, back_col_start;
  int lumLength=(pictureArray->getCurrent())->getLumLength();
  int colorLength=(pictureArray->getCurrent())->getColorLength();
  int endPast=0;
  int endFuture=0;




  /* If block is luminance block... */

  if (bnum < 4) {


    /* Set dest to luminance plane of current pict image. */

    dest = (pictureArray->getCurrent())->getLuminancePtr();

    /* If past frame exists, set past to luminance plane of past frame. */

    past = (pictureArray->getPast())->getLuminancePtr();
    endPast=lumLength;

    /*
     * If future frame exists, set future to luminance plane of future frame.
     */

    future = (pictureArray->getFuture())->getLuminancePtr();
    endFuture=lumLength;

    /* Calculate row,col of upper left pixel in block. */

    row = (mb_row << 4);
    col = (mb_col << 4);
    if (bnum > 1)
      row += 8;
    if (bnum & 0x01)
      col += 8;



  } else {
    /* Otherwise, block is NOT luminance block, ... */

    /* Construct motion vectors. */
    recon_right_for >>= 1;
    recon_down_for >>= 1;
    recon_right_back >>= 1;
    recon_down_back >>= 1;

    /* Establish row size. yuv color is half of whole size*/

    row_size /= 2;


    /* Calculate row,col of upper left pixel in block. */

    row = (mb_row << 3);
    col = (mb_col << 3);



    
    /* If block is Cr block... */
	/* Switched earlier, so we test Cr first - eyhung */

    if (bnum == 5) {
      /* Set dest to Cr plane of current pict image. */

      dest = (pictureArray->getCurrent())->getCrPtr();

      /* If past frame exists, set past to Cr plane of past image. */

      past = (pictureArray->getPast())->getCrPtr();
      endPast=colorLength;
      /*
       * If future frame exists, set future to Cr plane of future image.
       */

      future = (pictureArray->getFuture())->getCrPtr();
      endFuture=colorLength;
    }
    /* Otherwise, block is Cb block... */

    else {
      /* Set dest to Cb plane of current pict image. */
      dest = (pictureArray->getCurrent())->getCbPtr();

      /* If past frame exists, set past to Cb plane of past frame. */

      past = (pictureArray->getPast())->getCbPtr();
      endPast=colorLength;
      
      /*
       * If future frame exists, set future to Cb plane of future frame.
       */

      future = (pictureArray->getFuture())->getCbPtr();
      endFuture=colorLength;
    }
  }
  /*
   * Calculate right_for, down_for, right_half_for, down_half_for,
   * right_back, down_bakc, right_half_back, and down_half_back, motion
   * vectors.
   */
  
  right_for = recon_right_for >> 1;
  down_for = recon_down_for >> 1;
  right_half_for = recon_right_for & 0x1;
  down_half_for = recon_down_for & 0x1;
  
  right_back = recon_right_back >> 1;
  down_back = recon_down_back >> 1;
  right_half_back = recon_right_back & 0x1;
  down_half_back = recon_down_back & 0x1;

  forw_col_start = col + right_for;
  forw_row_start = row + down_for;
  
  back_col_start = col + right_back;
  back_row_start = row + down_back;

  /* For each pixel in block... */

  index = dest + (row * row_size) + col;

  rindex1 = past + forw_row_start  * row_size + forw_col_start;

  bindex1 = future + back_row_start * row_size + back_col_start;

  if ((rindex1+7*row_size+7 >= past+endPast) || (rindex1 < past))  {
    DEBUG_RECON(cout << "urg! last resort -1"<<endl;)
    return false;
  }
  if ((bindex1+7*row_size+7 >= future+endFuture) || (bindex1 < future)) {
    DEBUG_RECON(cout << "urg! last resort -2"<<endl;)
    return false;
  }



  if (!zflag) {
    copyFunctions->copy8_div2_src3linear_crop(rindex1,bindex1,dct_start,
					      index,row_size);
  } else {
    copyFunctions->copy8_div2_nocrop(rindex1,bindex1,index,row_size);
  }

  return true;
}


