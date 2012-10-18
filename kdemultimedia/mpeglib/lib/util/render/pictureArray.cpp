/*
  nice try of an X11 output plugin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "pictureArray.h"



PictureArray::PictureArray(int width, int height) {
  int i;
  pictureCallback=NULL;
  imageType=PICTURE_NO_TYPE;

  for (i=0;i<_PICTURE_ARRAY_SIZE;i++) {
    pictureArray[i]=new YUVPicture(width,height);
    imageType=pictureArray[i]->getImageType();
  }  
  
  /* Find a pict image structure in ring buffer not currently locked. */
  /* Set current pict image structure to the one just found in ring. */
  
  current=pictureArray[0];
  past=pictureArray[1];
  future=pictureArray[2];

  picPerSec=0.0;
  this->width=width;
  this->height=height;

}


PictureArray::~PictureArray() {
  int i;
  for (i=0;i<_PICTURE_ARRAY_SIZE;i++) {
    if (pictureArray[i] != NULL) {
      delete pictureArray[i];
      pictureArray[i]=NULL;
    }

  } 
}





void PictureArray::setPicturePerSecond(double val) {
  picPerSec=val;
}


double PictureArray::getPicturePerSecond() {
  return picPerSec;
}




void PictureArray::forward() {
  /* Update past and future references if needed. */

  YUVPicture* tmp=past;

  past = future;
  future = current;
  current = tmp;
    

}




YUVPicture* PictureArray::getYUVPictureCallback() {
  return pictureCallback;
}


void PictureArray::setYUVPictureCallback(YUVPicture* pic) {
  this->pictureCallback=pic;
}


void PictureArray::setImageType(int imageType) {
  int i;
  this->imageType=imageType;
  for (i=0;i<_PICTURE_ARRAY_SIZE;i++) {
    pictureArray[i]->setImageType(imageType);
  }  
}

