/*
  describes a picture in yuv format
  Copyright (C) 2000 Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "yuvPicture.h"

#include <iostream>

using namespace std;

static int instanceCnt=0;

YUVPicture::YUVPicture(int width,int height) {
  this->width=width;
  this->height=height;

  instance=instanceCnt;
  instanceCnt++;
  imagePtr=NULL;

  setImageType(PICTURE_YUVMODE_CR_CB);


  startTimeStamp=new TimeStamp();
  waitTime=new TimeStamp();
  earlyTime=new TimeStamp();
  mpegType=-1;

}


YUVPicture::~YUVPicture() {
  delete imagePtr;
  delete earlyTime;
  delete startTimeStamp;
  delete waitTime;
}



int YUVPicture::getHeight() {
  return height;
}


int YUVPicture::getWidth() {
  return width;
}



int YUVPicture::getLumLength() {
  return lumLength;
}


int YUVPicture::getColorLength() {
  return colorLength;
}

int YUVPicture::getImageSize() {
  return imageSize;
}
  






void YUVPicture::print(const char* title) {
  cout << title <<":";
  printf(" instance:%d  ",instance);
  printf(" width:%d  ",width);
  printf(" height:%d  ",height);
  cout <<" picPerSec:"<<picPerSec;
  switch(mpegType) {
  case 1:
    printf("I_FRAME  ");
    break;
  case 2:
    printf("P_FRAME  ");
    break;
  case 3:
    printf("B_FRAME  ");
    break;
  case 4:
    printf("D_FRAME  ");
    break;  
  default:
    printf("<unknown>  ");
  }
  printf("\n");
  
  
}


void YUVPicture::setPicturePerSecond(float val) {
  this->picPerSec=val;
}


float YUVPicture::getPicturePerSecond() {
  return picPerSec;
}


void YUVPicture::setStartTimeStamp(TimeStamp* aStamp) {
  aStamp->copyTo(startTimeStamp);
}


TimeStamp* YUVPicture::getStartTimeStamp() {
  return startTimeStamp;
}

void YUVPicture::setWaitTime(TimeStamp* aStamp) {
  aStamp->copyTo(waitTime);
}


TimeStamp* YUVPicture::getWaitTime() {
  return waitTime;
}

void YUVPicture::setEarlyTime(TimeStamp* earlyTime) {
  this->earlyTime=earlyTime;
}


TimeStamp* YUVPicture::getEarlyTime() {
  return earlyTime;
}



void YUVPicture::setMpegPictureType(int type) {
  this->mpegType=type;
}


int YUVPicture::getMpegPictureType() {
  return mpegType;
}


void YUVPicture::setImageType(int imageType) {


  //
  // Reset everything
  //

  if (imagePtr != NULL) {
    delete [] imagePtr;
    imagePtr=NULL;
  }
  this->imageType=imageType;

  lumLength=0;
  colorLength=0;
  Cr_mode=NULL;
  Cb_mode=NULL;
  luminance_mode=NULL;


  //
  // YUV Images
  //
  if ( (imageType == PICTURE_YUVMODE_CR_CB) ||
       (imageType == PICTURE_YUVMODE_CB_CR) ) {


    lumLength=width * height;
    colorLength=width * height / 4;
    imageSize=lumLength+colorLength+colorLength;

    // the 64 is some "security" space
    imagePtr=new unsigned char[imageSize+64];

    if (imagePtr == NULL) {
      cout << "cannot create image"<<endl;
      exit(0);
    }

    // now caculate pointers to start addresses of lum/Cr/Cb blocks
    // we need the yuvPtr for direct dither in hardware
    // this should save a memcpy

    luminance = imagePtr;
    Cr = imagePtr+lumLength;
    Cb = imagePtr+lumLength+colorLength;
    
   
    if ( (luminance == NULL) ||
	 (Cr == NULL) ||
	 (Cb == NULL) ) {
      cout << "allocation luminance/Cr/Cb error"<<endl;
      exit(0);
    }

    switch(imageType) {
    case PICTURE_YUVMODE_CR_CB:
      Cr_mode=Cr;
      Cb_mode=Cb;
      luminance_mode=imagePtr;
      break;
    case PICTURE_YUVMODE_CB_CR:
      Cr_mode=Cb;
      Cb_mode=Cr;
      luminance_mode=imagePtr;
      break;
    default:
      cout << "unknown yuv mode:"<<imageType<<endl;
    }
  }
  else if ( (imageType == PICTURE_YUVMODE_YUY2) ||
       (imageType == PICTURE_YUVMODE_UYVY) ) {
    // these yuv-modes are packed

    imageSize=width * height * 2;

    // the 64 is some "security" space
    imagePtr=new unsigned char[imageSize+64];

    if (imagePtr == NULL) {
      cout << "cannot create image"<<endl;
      exit(0);
    }
  }

  //
  // RGB Imcdages
  //
  
  if ( (imageType == PICTURE_RGB) ||
       (imageType == PICTURE_RGB_FLIPPED) ){
    imageSize=width*height*4;
    imagePtr=new unsigned char[imageSize];
  }
  memset(imagePtr,0,imageSize);
}
