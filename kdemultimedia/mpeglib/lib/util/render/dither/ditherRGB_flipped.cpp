/*
  flips RGB images
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "ditherRGB_flipped.h"

#include <iostream>

using namespace std;


DitherRGB_flipped::DitherRGB_flipped() {
  flipSpace=NULL;
  flipSize=0;
}

DitherRGB_flipped::~DitherRGB_flipped() {
  if (flipSpace != NULL) {
    delete flipSpace;
  }
}




void DitherRGB_flipped::flipRGBImage(unsigned char* dest,unsigned char* src,
				     int depth,int width,int height,int ) {

  int byteDepth;

  switch(depth) {
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
    cout << "unknown byteDepth:"<<depth
         << " in DitherRGB_flipped::flipRGBImage"<<endl;
    return;
  }
    

  int spaceNeeded=width*height*byteDepth;

  if (spaceNeeded > flipSize) {
    if (flipSpace != NULL) {
      delete flipSpace;
    }
    cout << "flipSpace:"<<spaceNeeded<<endl;
    flipSpace=new unsigned char[spaceNeeded+64];
    flipSize=spaceNeeded;
  }

  int i;
  int lineSize=width*byteDepth;
  unsigned char* end=dest+lineSize*(height-1);

  for (i=0;i<height;i++) {
    memcpy(end,src,lineSize);
    src+=lineSize;
    end-=lineSize;
  }

}


