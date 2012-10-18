/*
  stores sequence header info, for reinit of stream
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */





#ifndef __MPEGVIDEOHEADER_H
#define __MPEGVIDEOHEADER_H



/**
   Some vcd do only have one sequence header for the whole
   cd.
   This means we must store this info even after a seek
   which usually destroy the video stream object.
   We then can re-init the video object with the values
   we store here.

   Here we have the "header" for an mpeg video stream.
   
*/


class MpegVideoHeader {
  
  unsigned int h_size;                        /* Horiz. size in pixels.     */
  unsigned int v_size;                        /* Vert. size in pixels.      */
  int mb_height;                              /* Vert. size in mblocks.     */
  int mb_width;                               /* Horiz. size in mblocks.    */
  int mb_size;                                /* mb_height*mb_width-1       */
  unsigned char aspect_ratio;                 /* Code for aspect ratio.     */
  unsigned int bit_rate;                      /* Bit rate.                  */
  unsigned int vbv_buffer_size;               /* Minimum buffer size.       */
  int const_param_flag;                       /* Contrained parameter flag. */
  float picture_rate;                         /* Parse off picture rate code*/
  unsigned int intra_quant_matrix[8][8];      /* Quantization matrix for
						 intracoded frames.         */
  unsigned int non_intra_quant_matrix[8][8];  /* Quanitization matrix for 
						 non intracoded frames.     */
  class MpegExtension* extension;


 public:

  MpegVideoHeader();
  ~MpegVideoHeader();

  int parseSeq(class MpegVideoStream* mpegVideoStream);
  void copyTo(MpegVideoHeader* dest);
  void print(char* description);
  void init_quanttables();

  inline int getMB_Height() { return  mb_height;}
  inline int getMB_Size() { return  mb_size;}
  inline int getMB_Width() { return  mb_width;}

  inline float getPictureRate() { return picture_rate ;}
  double getPictureTime();

  inline unsigned int* getIntra_quant_matrix() { 
    //    printf("getI:%p\n",intra_quant_matrix[0]);
    return  intra_quant_matrix[0]; }
  inline unsigned int* getNon_intra_quant_matrix() { 
    //    printf("getN:%p\n",non_intra_quant_matrix[0]);
    return  non_intra_quant_matrix[0]; }

 private:
  void init();

};

#endif
