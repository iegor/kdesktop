/*
  describes a picture in yuv format
  Copyright (C) 2000 Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __YUVPICTURE_H
#define __YUVPICTURE_H


extern "C" {
#include <stdio.h>
#include <string.h>
}

#include "../timeStamp.h"

#define PICTURE_NO_TYPE     -1

#define PICTURE_YUVMODE_CR_CB   1
#define PICTURE_YUVMODE_CB_CR   2
#define PICTURE_RGB             3
#define PICTURE_RGB_FLIPPED     4
#define PICTURE_YUVMODE_YUY2    5
#define PICTURE_YUVMODE_UYVY    6

class YUVPicture {

  unsigned char* imagePtr;               /* Pointer to complete yuv image */

  unsigned char* luminance;              /* Luminance plane.   */
  unsigned char* Cr;                     /* Cr plane.          */
  unsigned char* Cb;                     /* Cb plane.          */


  int width;
  int height;

  float picPerSec;
  int lumLength;
  int colorLength;
  int imageSize;

  TimeStamp* startTimeStamp;
  TimeStamp* waitTime;
  TimeStamp* earlyTime;

  int mpegType;
  int instance;
  int imageType;

  unsigned char* image_mode;            /* start Pointer to complete image */
  unsigned char* luminance_mode;         /* Luminace plane.          */
  unsigned char* Cr_mode;                /* Cr plane.          */
  unsigned char* Cb_mode;                /* Cb plane.          */

 public:
  YUVPicture(int width,int height);
  ~YUVPicture();
 
  //
  //  For YUV Images
  //
  inline unsigned char* getLuminancePtr() {return luminance_mode;}
  inline unsigned char* getCrPtr() {return Cr_mode;}
  inline unsigned char* getCbPtr() {return Cb_mode;}

  // general
  inline unsigned char* getImagePtr() {return imagePtr;}

  // use these to swap the image Types
  inline int getImageType()           { return imageType; }
  void setImageType(int mode);


  int getHeight();
  int getWidth();

  int getLumLength();
  int getColorLength();
  int getImageSize();

  void setPicturePerSecond(float val);
  float getPicturePerSecond();


  void setStartTimeStamp(TimeStamp* timeStamp);
  TimeStamp* getStartTimeStamp();

  void setWaitTime(TimeStamp* waitTime);
  TimeStamp* getWaitTime();

  void setEarlyTime(TimeStamp* earlyTime);
  TimeStamp* getEarlyTime();

  void setMpegPictureType(int type);
  int getMpegPictureType();


  void print(const char* title);
};
#endif
