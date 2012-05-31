/*
  wrapper for X11 Window
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "ditherWrapper.h"

#include <iostream>

using namespace std;


/* 
   Flag for gamma correction 
   Makes images brighter/darker. 
   It's in the source but not activated (for now)
*/
int gammaCorrectFlag = 0;
double gammaCorrect = 1.0;

/* 
   Flag for chroma correction.
   reduce the color intensity..
   It's in the source but not activated (for now)
*/
int chromaCorrectFlag = 0;
double chromaCorrect = 1.0;



DitherWrapper::DitherWrapper(int bpp,unsigned int redMask,
			     unsigned int greenMask,unsigned int blueMask,
			     unsigned char pixel[256]) {

  this->bpp=bpp;
  this->redMask=redMask;
  this->greenMask=greenMask;
  this->blueMask=blueMask;


  dither8Bit=new Dither8Bit(pixel);
  dither16Bit=new Dither16Bit(redMask,greenMask,blueMask);
  dither32Bit=new Dither32Bit(redMask,greenMask,blueMask);
  ditherRGB_flipped=new DitherRGB_flipped();
  ditherRGB=new DitherRGB();
 
  
#ifdef INTEL
  lmmx=mm_support();
#else
  lmmx=false;
#endif


}


DitherWrapper::~DitherWrapper(){
  delete dither16Bit;
  delete dither8Bit;
  delete dither32Bit;
  delete ditherRGB_flipped;
  delete ditherRGB;
}





void  DitherWrapper::doDither(YUVPicture* pic,int depth,int imageMode,
			      unsigned char* dest,int offset) {


  //
  // according to the input imageType and the output area
  // handle different dither methods
  //

  int inputType=pic->getImageType();

  if ( (inputType == PICTURE_YUVMODE_CR_CB) ||
       (inputType == PICTURE_YUVMODE_CB_CR) ) {
    doDitherYUV(pic,depth,imageMode,dest,offset);
    return;
  }

  if ( (inputType == PICTURE_RGB) ||
       (inputType == PICTURE_RGB_FLIPPED) ){
    doDitherRGB(pic,depth,imageMode,dest,offset);
    return;
  }
  
  cout << "unknown inputType:"<<inputType
       << " in DitherWrapper::doDither"<<endl;
}


void DitherWrapper::doDitherRGB(YUVPicture* pic,int depth,int imageMode,
				unsigned char* dest,int offset) {
  
  int inputType=pic->getImageType();

  switch(inputType) {
  case PICTURE_RGB:
    doDitherRGB_NORMAL(pic,depth,imageMode,dest,offset);
    break;
  case  PICTURE_RGB_FLIPPED:
    doDitherRGB_FLIPPED(pic,depth,imageMode,dest,offset);
    break;
  default:
    cout << "unknown RGB type:"<<inputType<<" in DitherWrapper"<<endl;
    exit(0);
  }
}


void DitherWrapper::doDitherRGB_NORMAL(YUVPicture* pic,
				       int depth,int imageMode,
				       unsigned char* dest,int offset) {
 
  int w=pic->getWidth();
  int h=pic->getHeight();

  unsigned char* src=pic->getImagePtr();

  if (imageMode & _IMAGE_DOUBLE) {
    ditherRGB->ditherRGBImage_x2(dest,src,depth,w,h,offset);
  } else {
    ditherRGB->ditherRGBImage(dest,src,depth,w,h,offset);
  }
}

void DitherWrapper::doDitherRGB_FLIPPED(YUVPicture* pic,
					int depth,int imageMode,
					unsigned char* dest,int offset) {
 
  int w=pic->getWidth();
  int h=pic->getHeight();

  unsigned char* src=pic->getImagePtr();

  ditherRGB_flipped->flipRGBImage(dest,src,depth,w,h,offset);
}



void DitherWrapper::doDitherYUV(YUVPicture* pic,int depth,int imageMode,
				 unsigned char* dest,int offset) {

  if (imageMode & _IMAGE_DOUBLE) {
    doDither_x2(pic,depth,dest,offset);
  } else {
    doDither_std(pic,depth,dest,offset);
  }
}


void DitherWrapper::doDither_std(YUVPicture* pic,int depth,
				 unsigned char* dest,int offset){
  
  int h=pic->getHeight();
  int w=pic->getWidth();
  unsigned char* lum=pic->getLuminancePtr();
  unsigned char* cr=pic->getCrPtr();
  unsigned char* cb=pic->getCbPtr();


  switch (depth) {
  case 8:
    dither8Bit->ditherImageOrdered(lum, cr, cb,dest , h, w);
    break;
  case 16:
    if (lmmx) {
      ditherBlock(lum,cr,cb,dest,h,w,offset);
    } else {
      dither16Bit->ditherImageColor16(lum,cr,cb,dest,h,w,offset);
    }

    break;
  case 24:
  case 32:
    if (lmmx) {
      dither32_mmx(lum, cr, cb,dest ,h,w,offset);
    } else {
      dither32Bit->ditherImageColor32(lum, cr, cb,dest ,h,w,offset);
    }


    break;
  default:
    cout << "cannot dither depth:"<<depth<<endl;
  }

}
  

void DitherWrapper::doDither_x2(YUVPicture* pic,int depth,
				unsigned char* dest,int offset){

  int h=pic->getHeight();
  int w=pic->getWidth();
  unsigned char* lum=pic->getLuminancePtr();
  unsigned char* cr=pic->getCrPtr();
  unsigned char* cb=pic->getCbPtr();


  switch (depth) {
  case 8: {
    // we do dither with the 8Bit std YUV ditherer to RGB
    // and then we do the double part with the
    // RGB ditherer. Its obviously much slower but at
    // least it works. To not allocate memory twice
    // we are a bit tricky. We know that the image 
    // has space for doubls size. We but the not double size
    // image at the bottom of the dest. Maybe that
    // the last line gets overwritten
    int memPos=3*h*w;
    dither8Bit->ditherImageOrdered(lum, cr, cb,dest+memPos, h, w);
    unsigned char* src=dest+memPos;
    ditherRGB->ditherRGBImage_x2(dest,src,depth,w,h,0);
    break;
  }
  case 16:
    dither16Bit->ditherImageTwox2Color16(lum,cr,cb,dest,h,w,offset);
    break;
  case 24:
  case 32:
    if (lmmx) {
      //dither32x2_mmx(lum, cr, cb,dest ,h,w,offset);
      dither32Bit->ditherImageTwox2Color32(lum,cr,cb,dest,h,w,offset);
    } else {
      dither32Bit->ditherImageTwox2Color32(lum,cr,cb,dest,h,w,offset);
    }
    break;
  default:
    cout << "cannot dither depth:" << depth << endl;
  }
}
