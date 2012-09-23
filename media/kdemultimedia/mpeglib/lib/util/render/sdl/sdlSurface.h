/*
  surface wrapper for SDL
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __SDLSURFACE_H
#define __SDLSURFACE_H


#include "../surface.h"
#include "imageDeskSDL.h"

 

#ifndef SDL_WRAPPER 
  class SDLSurface : public Surface {
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


class SDLSurface : public Surface {

  int lOpen;
  int imageMode;
  int lSDLInit;
  int video_bpp;
  SDL_Surface* surface;
  SDL_Rect rect;
  SDL_Rect resize_rect;
  const SDL_VideoInfo *video_info;

  ImageBase* imageCurrent;
  
  ImageDeskSDL* imageDeskSDL;

 public:
  SDLSurface();
  ~SDLSurface();

  int isOpen();
  int open(int width, int height,const char *title, bool border=false);
  int close();
  int getHeight();
  int getWidth();
  int getDepth();
  int getImageMode();
  int checkEvent(int* mode);

  int openImage(int imageMode, YUVPicture* pic = NULL);
  int closeImage();
  int dither(YUVPicture* pic);
  int putImage(YUVPicture* pic);


 private:
  void sdlinit();
};
#endif

#endif
