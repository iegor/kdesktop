/*
  nice try of an X11 output plugin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __VIDEOOUTPUTX11_H
#define __VIDEOOUTPUTX11_H


#include "yuvPicture.h"
#include <stdlib.h>

#define _PICTURE_ARRAY_SIZE     5

class PictureArray {

  class YUVPicture* pictureArray[_PICTURE_ARRAY_SIZE];

  class YUVPicture* past;                /* Past predictive frame.     */
  class YUVPicture* future;              /* Future predictive frame.   */
  class YUVPicture* current;             /* Current frame.             */
  
  double picPerSec;
  int width;
  int height;

  int imageType;

 public:
  PictureArray(int width, int height);
  ~PictureArray();

  inline YUVPicture* getPast() {return past;}
  inline YUVPicture* getFuture() {return future;}
  inline YUVPicture* getCurrent() {return current;}


  inline void setPast(YUVPicture* pic) {past=pic;}
  inline void setFuture(YUVPicture* pic) {future=pic;}
  inline void setCurrent(YUVPicture* pic) {current=pic;}
  inline int getWidth()      { return width; }
  inline int getWidth_Half() { return width/2; }
  
  // attention with these!
  // these are shares pointer
  // only call after mpegVidRsrc and then set them back to NULL
  YUVPicture* getYUVPictureCallback();
  void setYUVPictureCallback(YUVPicture* pic);


  void forward();

  void setPicturePerSecond(double val);
  double getPicturePerSecond();

  // use these to swap the yuv Mode
  inline int getImageType()           { return imageType; }
  void setImageType(int mode);


 private:
  YUVPicture* pictureCallback;


};
#endif

