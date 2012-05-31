/*
  mpeg video picture
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */





#ifndef __PICTURE_H
#define __PICTURE_H


#include "mpegExtension.h"


class Picture {

/* Picture structure. */

  unsigned int temp_ref;                 /* Temporal reference.             */
  unsigned int code_type;                /* Frame type: P, B, I             */
  unsigned int vbv_delay;                /* Buffer delay.                   */
  int full_pel_forw_vector;              /* Forw. vectors specified in full
					    pixel values flag.              */
  unsigned int forw_r_size;              /* Used for vector decoding.       */
  unsigned int forw_f;                   /* Used for vector decoding.       */
  int full_pel_back_vector;              /* Back vectors specified in full 
					    pixel values flag.              */
  unsigned int back_r_size;              /* Used in decoding.               */
  unsigned int back_f;                   /* Used in decoding.               */


  // MPEG 2 [START]
  

  MpegExtension* extension;
  // sync info
  class TimeStamp* startOfPicStamp;

  // MPEG 2 things
  int extraBit;

 public:
  Picture();
  ~Picture();

  int processPicture(class MpegVideoStream* mpegVideoStream);
  int processPictureCodingExtension(class MpegVideoStream* mpegVideoStream);


  inline unsigned int getCodeType()      { return code_type; }
  inline unsigned int getForw_f()        { return forw_f;}
  inline unsigned int getBack_f()        { return back_f;}
  inline void setForw_f(unsigned int f)  { forw_f=f;}
  inline void setBack_f(unsigned int f)  { back_f=f;}

  inline int getExtraBit()               { return extraBit; }

  inline TimeStamp* getStartOfPicStamp() { return startOfPicStamp;}
  inline unsigned int getFull_pel_forw_vector() {return full_pel_forw_vector;}
  inline unsigned int getFull_pel_back_vector() {return full_pel_back_vector;}

  inline void setFull_pel_forw_vector(unsigned int v) {full_pel_forw_vector=v;}
  inline void setFull_pel_back_vector(unsigned int v) {full_pel_back_vector=v;}

  unsigned int geth_back_r(class MpegVideoStream* mpegVideoStream);
  unsigned int getv_back_r(class MpegVideoStream* mpegVideoStream);
 
  unsigned int geth_forw_r(class MpegVideoStream* mpegVideoStream);
  unsigned int getv_forw_r(class MpegVideoStream* mpegVideoStream);
 

};
#endif
