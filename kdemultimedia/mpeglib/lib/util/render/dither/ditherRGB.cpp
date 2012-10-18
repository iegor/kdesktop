/*
  copys RGB images to a destination
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "ditherRGB.h"

#include <iostream>

using namespace std;

DitherRGB::DitherRGB() {
}


DitherRGB::~DitherRGB() {
}


int DitherRGB::getDepth(int pixel) {
  int byteDepth=0;

  switch(pixel) {
  case 8:
    byteDepth=1;
    break;
  case 15:
  case 16:
    byteDepth=2;
    break;
  case 24:
  case 32:
    byteDepth=4;
    break;
  default:
    cout << "unknown byteDepth:"<<pixel
         << " in DitherRGB_flipped::flipRGBImage"<<endl;
  }
  return byteDepth;
   
}

void DitherRGB::ditherRGBImage(unsigned char* dest,unsigned char* src,
			       int depth,int width,int height,int offset) {
  int byteDepth=getDepth(depth);
  if (byteDepth == 0) {
    return;
  }

  
  if (offset==0) {
    int bytes=height*width*byteDepth;
    memcpy(dest,src,bytes);
    return;
  }

  int i;
  int lineSize=width*byteDepth;
  
  offset=offset*byteDepth+lineSize;

  for (i=0;i<height;i++) {
    memcpy(dest,src,lineSize);
    src+=lineSize;
    dest+=offset;
  }

  
}

void DitherRGB::ditherRGBImage_x2(unsigned char* dest,unsigned char* src,
				  int depth,int width,int height,int offset) {

  int byteDepth=getDepth(depth);
  if (byteDepth == 0) {
    return;
  }

  switch(byteDepth) {
  case 1:
    ditherRGB1Byte_x2(dest,src,1,width, height,offset);
    break;
  case 2:
    ditherRGB2Byte_x2(dest,src,2,width, height,offset);
    break;
  case 4:
    ditherRGB4Byte_x2(dest,src,4,width, height,offset);
    break;
  default:
    cout <<"ditherRGBImage_x2 byteDepth:"<<byteDepth
	 <<" not supported"<<endl;
  }
}
 

void DitherRGB::ditherRGB1Byte_x2(unsigned char* dest,unsigned char* src,
				  int depth,int width,int height,int offset) {
  
  //
  // dest  destr
  // destd destrd

  int lineInc=2*width+offset;
  unsigned char* destr=dest+1;
  unsigned char* destd=dest+lineInc;
  unsigned char* destrd=destd+1;
  
  int row;
  int col;
  //
  // We copy byte by byte this is slow, but works for
  // all byteDepth
  // this memcpy can be optimized with MMX very i) good ii) easily

  for(row=0;row<height;row++) {
    for(col=0;col<width;col++) {
      *dest++=*src;
      *destr++=*src;
      *destd++=*src;
      *destrd++=*src;
      dest++;
      destr++;
      destd++;
      destrd++;
      
      src++;
    }
    dest+=lineInc;
    destr+=lineInc;
    destd+=lineInc;
    destrd+=lineInc;
  }
}


void DitherRGB::ditherRGB2Byte_x2(unsigned char* destination,
				  unsigned char* source,
				  int depth,int width,int height,int offset) {
  //
  // dest  destr
  // destd destrd

  unsigned short int* src=(unsigned short int*) source;
  unsigned short int* dest=(unsigned short int*) destination;
  
  int lineInc=2*width+offset;
  unsigned short int* destr=dest+1;
  unsigned short int* destd=dest+lineInc;
  unsigned short int* destrd=destd+1;
  
  int row;
  int col;
  //
  // We copy byte by byte this is slow, but works for
  // all byteDepth
  // this memcpy can be optimized with MMX very i) good ii) easily

  for(row=0;row<height;row++) {
    for(col=0;col<width;col++) {
      *dest++=*src;
      *destr++=*src;
      *destd++=*src;
      *destrd++=*src;
      dest++;
      destr++;
      destd++;
      destrd++;
      
      src++;
    }
    dest+=lineInc;
    destr+=lineInc;
    destd+=lineInc;
    destrd+=lineInc;
  }
}


void DitherRGB::ditherRGB4Byte_x2(unsigned char* destination,
				  unsigned char* source,
				  int depth,int width,int height,int offset) {

  //
  // dest  destr
  // destd destrd

  unsigned int* src=(unsigned int*) source;
  unsigned int* dest=(unsigned int*) destination;
  
  int lineInc=2*width+offset;
  unsigned int* destr=dest+1;
  unsigned int* destd=dest+lineInc;
  unsigned int* destrd=destd+1;
  
  int row;
  int col;
  //
  // We copy byte by byte this is slow, but works for
  // all byteDepth
  // this memcpy can be optimized with MMX very i) good ii) easily

  for(row=0;row<height;row++) {
    for(col=0;col<width;col++) {
      *dest++=*src;
      *destr++=*src;
      *destd++=*src;
      *destrd++=*src;
      dest++;
      destr++;
      destd++;
      destrd++;
      
      src++;
    }
    dest+=lineInc;
    destr+=lineInc;
    destd+=lineInc;
    destrd+=lineInc;
  }

}
 
