/*
  SDL surface output
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "imageDeskSDL.h"

#ifdef SDL_WRAPPER


ImageDeskSDL::ImageDeskSDL() {

  this->surface=NULL;
  this->rect=NULL;
  imageMode=_IMAGE_NONE;
  lSupport=true;
  image=NULL;
}


ImageDeskSDL::~ImageDeskSDL() {
  closeImage();
  cout << "SDL destry needed"<<endl;
}



int ImageDeskSDL::support() {
  return lSupport;
}


void ImageDeskSDL::init(XWindow* xWindow, YUVPicture* pic) {
  cout << "ImageDeskSDL::init"<<endl;
  this->surface=(SDL_Surface*)xWindow;
  this->rect=(SDL_Rect*)pic;
}

int ImageDeskSDL::openImage(int imageMode) {
  int w=rect->w;
  int h=rect->h;
  this->imageMode=imageMode;
  /* Create a YV12 image (Y + V + U) */
  cout << "CreateYUVOverlay -s"<<imageMode<<" w:"<<w<<" h:"<<h<<endl;
  image = SDL_CreateYUVOverlay(w,h,
			       SDL_YV12_OVERLAY, 
			       surface);
  if (image == NULL) {
    cout << "error creating image"<<endl;
    exit(0);
  }
  cout << "CreateYUVOverlay -e"<<endl;
  return true;
}


int ImageDeskSDL::closeImage() {
  if (image != NULL) {
    cout << "FreeYUVOverlay -s"<<endl;
    SDL_FreeYUVOverlay(image);
    // otherwise test of NULL will fail
    image = NULL;
    cout << "FreeYUVOverlay -e"<<endl;
  }
  return true;
}



void ImageDeskSDL::ditherImage(YUVPicture* pic) {

  int w=pic->getWidth();
  int h=pic->getHeight();
  int size=w*h+(w*h)/2;
  SDL_LockYUVOverlay(image);
  memcpy(*((char**)image->pixels),pic->getLuminancePtr(),size);
  SDL_UnlockYUVOverlay(image);
 
}


void ImageDeskSDL::putImage(int w, int h) {
  SDL_Rect dest;
  dest.x=0;
  dest.y=0;
  dest.w=rect->w;
  dest.h=rect->h;
  if (imageMode & _IMAGE_RESIZE) {
    dest.w = w;
    dest.h = h;
  }

  if (imageMode & _IMAGE_DOUBLE) {
    dest.w*=2;
    dest.h*=2;
  }
  SDL_DisplayYUVOverlay(image,&dest);

}

#endif

