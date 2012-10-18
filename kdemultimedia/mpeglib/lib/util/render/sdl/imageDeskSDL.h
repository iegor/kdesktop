/*
  SDL surface output
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __IMAGEDESKSDL_H
#define __IMAGEDESKSDL_H

#include "../imageBase.h"

 

#ifndef SDL_WRAPPER 
  class ImageDeskSDL : public ImageBase {
  };
#endif  

#ifdef SDL_WRAPPER

#if defined WIN32
#include <SDL.h>
#include <SDL_video.h>
#else
#include <SDL/SDL.h>
#include <SDL/SDL_video.h>
#endif

class ImageDeskSDL : public ImageBase {

  int lSupport;
  SDL_Overlay *image;

  SDL_Surface* surface;
  SDL_Rect* rect;
  int imageMode;

 public:
  ImageDeskSDL();
  ~ImageDeskSDL();

  int support();
  void init(XWindow* xWindow, YUVPicture* pic=NULL);


  int openImage(int imageMode);
  int closeImage();

  void ditherImage(YUVPicture* pic);

  void putImage(int w, int h);



};
#endif

#endif
