/*
  stores sequence header info, for reinit of stream
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "mpegVideoHeader.h"

#include "mpegExtension.h"
#include "mpegVideoStream.h"

#include <iostream>

using namespace std;

/* Video rates table */
/* Cheat on Vid rates, round to 30, and use 30 if illegal value 
   Except for 9, where Xing means 15, and given their popularity, we'll
   be nice and do it */

static double VidRateNum[16]={29.97, 24, 24, 25, 29.97, 29.97, 50, 59.94, 
			      59.94, 14.985, 29.97, 29.97, 29.97, 29.97,
			      29.97, 29.97};



static const unsigned char default_intra_matrix[64] = {
  8, 16, 19, 22, 26, 27, 29, 34,
  16, 16, 22, 24, 27, 29, 34, 37,
  19, 22, 26, 27, 29, 34, 34, 38,
  22, 22, 26, 27, 29, 34, 37, 40,
  22, 26, 27, 29, 32, 35, 40, 48,
  26, 27, 29, 32, 35, 40, 48, 58,
  26, 27, 29, 34, 38, 46, 56, 69,
  27, 29, 35, 38, 46, 56, 69, 83};
  


/* Set up array for fast conversion from zig zag order to row/column
   coordinates.
*/

const int zigzag[64][2] = {
  {0,0},{1,0},{0,1},{0,2},{1,1},{2,0},{3,0},{2,1},{1,2},{0,3},{0,4},{1,3},
  {2,2},{3,1},{4,0},{5,0},{4,1},{3,2},{2,3},{1,4},{0,5},{0,6},{1,5},{2,4},
  {3,3},{4,2},{5,1},{6,0},{7,0},{6,1},{5,2},{4,3},{3,4},{2,5},{1,6},{0,7},
  {1,7},{2,6},{3,5},{4,4},{5,3},{6,2},{7,1},{7,2},{6,3},{5,4},{4,5},{3,6},
  {2,7},{3,7},{4,6},{5,5},{6,4},{7,3},{7,4},{6,5},{5,6},{4,7},{5,7},{6,6},
  {7,5},{7,6},{6,7},{7,7} };





MpegVideoHeader::MpegVideoHeader() {
  init();

}


void MpegVideoHeader::init() {

  h_size=0;
  v_size=0;
  mb_height=0;
  mb_width=0;
  mb_size=0;
  aspect_ratio=0;
  bit_rate=0;
  vbv_buffer_size=0;
  const_param_flag=0;
  picture_rate=0.0;
  extension=new MpegExtension();
  init_quanttables();

}

void MpegVideoHeader::init_quanttables() {
  int i;
  int j;

  /* Copy default intra matrix. */

  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      intra_quant_matrix[i][j]=default_intra_matrix[i*8+j];
    }
  }

  /* Initialize non intra quantization matrix. */

  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      non_intra_quant_matrix[i][j] = 16;
    }
  }
}


MpegVideoHeader::~MpegVideoHeader() {

  delete extension;

}



/*
 *--------------------------------------------------------------
 *
 * ParseSeqHead --
 *
 *      Assumes bit stream is at the END of the sequence
 *      header start code. Parses off the sequence header.
 *
 * Results:
 *      Fills the vid_stream structure with values derived and
 *      decoded from the sequence header. Allocates the pict image
 *      structures based on the dimensions of the image space
 *      found in the sequence header.
 *
 * Side effects:
 *      Bit stream irreversibly parsed off.
 *
 *--------------------------------------------------------------
 */
int MpegVideoHeader::parseSeq(MpegVideoStream* mpegVideoStream) {
  unsigned int data;
  int i ;

  // seq_start_code already flushed!!!

  /* Get horizontal size of image space. */

  h_size=mpegVideoStream->getBits(12);

  /* Get vertical size of image space. */

  v_size=mpegVideoStream->getBits(12);


  /* Calculate macroblock width and height of image space. */

  mb_width = (h_size + 15) / 16;
  mb_height = (v_size + 15) / 16;
  mb_size=mb_height * mb_width-1;
  


  /* Parse of aspect ratio code. */

  data=mpegVideoStream->getBits(4);

  aspect_ratio = (unsigned char) data;

  /* Parse off picture rate code. */

  data=mpegVideoStream->getBits(4);
  picture_rate=VidRateNum[data];

  /* Parse off bit rate. */

  data=mpegVideoStream->getBits(18);
  bit_rate = data;

  /* Flush marker bit. */

  mpegVideoStream->flushBits(1);

  /* Parse off vbv buffer size. */

  data=mpegVideoStream->getBits(10);
  vbv_buffer_size = data;

  /* Parse off contrained parameter flag. */

  data=mpegVideoStream->getBits(1);
  if (data) {
    const_param_flag = true;
  } else
    const_param_flag = false;

  /*
   * If intra_quant_matrix_flag set, parse off intra quant matrix values.
   */
  data=mpegVideoStream->getBits(1);
  if (data) {
    for (i = 0; i < 64; i++) {
      data=mpegVideoStream->getBits(8);
      intra_quant_matrix[zigzag[i][1]][zigzag[i][0]]=(unsigned char)data;
    }
  }
  /*
   * If non intra quant matrix flag set, parse off non intra quant matrix
   * values.
   */

  data=mpegVideoStream->getBits(1);
  if (data) {
    for (i = 0; i < 64; i++) {
      data=mpegVideoStream->getBits(8);

      non_intra_quant_matrix[zigzag[i&0x3f][1]][zigzag[i&0x3f][0]]=
	(unsigned char) data;
    }
  }
  /*
   * If next start code is extension/user start code, 
   * parse off extension data.
   */
  extension->processExtensionData(mpegVideoStream);

  return true;

}




void MpegVideoHeader::copyTo(MpegVideoHeader* dest) {


  dest->h_size=h_size;
  dest->v_size=v_size;
  dest->mb_height=mb_height;
  dest->mb_width=mb_width;
  dest->mb_size=mb_size;
  dest->aspect_ratio=aspect_ratio;
  dest->bit_rate=bit_rate;
  dest->vbv_buffer_size=vbv_buffer_size;
  dest->const_param_flag=const_param_flag;
  dest->picture_rate=picture_rate;

  int i;
  int j;


   /* Copy default intra matrix. */

  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      dest->intra_quant_matrix[i][j]=intra_quant_matrix[i][j];

    }
  }

  /* Initialize non intra quantization matrix. */

  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      dest->non_intra_quant_matrix[i][j] =non_intra_quant_matrix[i][j] ;
    }
  }
  
}




double MpegVideoHeader::getPictureTime() {
  if (picture_rate > 0) {
    return 1.0/picture_rate;
  }
  return 0.0;
}







void MpegVideoHeader::print(char* description) {
  cout << "MpegVideoHeader [START]:"<<description<<endl;
  cout <<"h_size:"<<h_size<<endl;
  cout <<"v_size:"<<v_size<<endl;
  cout <<"mb_height:"<<mb_height<<endl;
  cout <<"mb_width:"<<mb_width<<endl;
  cout <<"mb_size:"<<mb_size<<endl;
  cout <<"aspect_ratio:"<<aspect_ratio<<endl;
  cout <<"bit_rate:"<<bit_rate<<endl;
  cout <<"vbv_buffer_size:"<<vbv_buffer_size<<endl;
  cout <<"const_param_flag:"<<const_param_flag<<endl;
  cout << "MpegVideoHeader [END]:"<<endl;

}
