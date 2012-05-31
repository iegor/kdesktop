/*
  this class dithery RGB picture to yuv12
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "dither2YUV.h"

#include <iostream>

using namespace std;


Dither2YUV::Dither2YUV() {


  lmmx=mm_support();

}


Dither2YUV::~Dither2YUV(){
}






void Dither2YUV::doDither(YUVPicture* pic,int depth,int ditherSize,
			  unsigned char* dest,int offset) {
  
  int inputType=pic->getImageType();

  switch(inputType) {
  case PICTURE_RGB:
    doDitherRGB_NORMAL(pic,depth,ditherSize,dest,offset);
    break;
  default:
    std::cout << "unknown RGB type:"<<inputType<<" in Dither2YUV"<<std::endl;
    exit(0);
  }
}


void Dither2YUV::doDitherRGB_NORMAL(YUVPicture* rgbPic,
				    int depth,int ditherSize,
				    unsigned char* dest,int offset) {
 
  switch (ditherSize) {
  case _SIZE_NORMAL: 
    doDither2YUV_std(rgbPic,depth,dest,offset);
    break;
  case _SIZE_DOUBLE:
    std::cout << "double not supported for RGB"<<std::endl;
    break;
  default:
    std::cout << "unknown size:"<<ditherSize<<" in Dither2YUV"<<std::endl;
    exit(0);
  }
}


void Dither2YUV::doDither2YUV_std(YUVPicture* rgbPic,int depth,
				  unsigned char* dest,int offset){
  
  int h=rgbPic->getHeight();
  int w=rgbPic->getWidth();
  int lumLength=w * h;
  int colorLength=(w * h) / 4;

  unsigned char* lum=dest;
  unsigned char* cr=lum+lumLength;
  unsigned char* cb=cr+colorLength;
  unsigned char* rgbSource=rgbPic->getImagePtr();


  switch (depth) {
  case 8:
    std::cout << "8 bit dither to yuv not supported"<<std::endl;
    exit(0);
    break;
  case 16:
    if (lmmx) {
#ifdef INTEL
      rgb2yuv16bit_mmx(rgbSource,lum,cr,cb,h,w);
#endif
    } else {
      rgb2yuv16bit(rgbSource,lum,cr,cb,h,w);
    }

    break;
  case 24:
    if (lmmx) {
#ifdef INTEL
      rgb2yuv24bit_mmx(rgbSource,lum,cr,cb,h,w);
#endif
    } else {
      rgb2yuv24bit(rgbSource,lum,cr,cb,h,w);
    }
    break;
  case 32:
    if (lmmx) {
#ifdef INTEL
      rgb2yuv32bit_mmx(rgbSource,lum,cr,cb,h,w);
#endif
    } else {
      rgb2yuv32bit(rgbSource,lum,cr,cb,h,w);
    }     
    break;
  default:
    std::cout << "cannot dither depth:"<<depth<<std::endl;
  }

}
  

