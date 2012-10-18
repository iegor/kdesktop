/*
  surface wrapper for SDL
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "sdlSurface.h"


#ifdef SDL_WRAPPER


SDLSurface::SDLSurface() {
  surface=NULL;
  lOpen=false;
  imageMode=_IMAGE_NONE;
  lSDLInit=false;
  imageDeskSDL=new ImageDeskSDL();
}


SDLSurface::~SDLSurface() {
  close();
  if (imageCurrent != NULL) {
    imageCurrent->closeImage();
    delete imageCurrent;
    // otherwise test of NULL will fail
    imageCurrent = NULL;
  }
}


int SDLSurface::isOpen() {
  return lOpen;
}

int SDLSurface::getImageMode() {
  return imageMode;
}

int SDLSurface::open(int width, int height,const char *title,bool border) {
  cout << "SDL openImage:"<<title<<endl;

  sdlinit();

  rect.x = 0;
  rect.y = 0;
  rect.w = width;
  rect.h = height;

  return true;
}


int SDLSurface::close() {
  if (isOpen()==false) {
    cout << "WindowOut::closeWindow already closed"<<endl;
    return true;
  }
  if(surface) {
    SDL_FreeSurface(surface);
    surface = NULL;
  }

  lOpen=false;
  return true;
}


int SDLSurface::getHeight() {
  return rect.h;
}


int SDLSurface::getWidth() {
  return rect.w;
}


int SDLSurface::getDepth() {
  return video_bpp;
}

int SDLSurface::putImage(YUVPicture* ) {
  return true;
}


int SDLSurface::openImage(int imageMode, YUVPicture* pic) {
  if (this->imageMode != _IMAGE_NONE) {
    cout << "bad open error X11Surface::openImage"<<endl;
    exit(0);
  }
  cout << "************* openImage SDL"<<imageMode<<endl;
  this->imageMode=imageMode;
  imageCurrent=NULL;
  int w=getWidth();
  int h=getHeight();
  if(imageMode & _IMAGE_RESIZE) {
    w=resize_rect.w; 
    h=resize_rect.h; 
  }
 
  if (imageMode & _IMAGE_DOUBLE) {
    w=rect.w*2;
    h=rect.h*2;
  }

  if (imageMode & _IMAGE_DESK) {
    if (imageDeskSDL->support()) {
      imageCurrent=imageDeskSDL;
      int video_flags = SDL_SWSURFACE;
      video_flags |= SDL_ASYNCBLIT;
      video_flags |= SDL_RESIZABLE;
      if(surface)
	SDL_FreeSurface(surface);
      surface = SDL_SetVideoMode(w, h, video_bpp, video_flags);

    }
  }
  if (imageMode & _IMAGE_FULL) {
    if (imageDeskSDL->support()) {
      imageCurrent=imageDeskSDL;
      int video_flags = SDL_FULLSCREEN;
      video_flags |= SDL_ASYNCBLIT;
      video_flags |= SDL_HWSURFACE;
      if(surface)
	SDL_FreeSurface(surface);
      surface = SDL_SetVideoMode(w, h, video_bpp, video_flags);
    }
  }
  if (imageCurrent != NULL) {
    cout << "surface:"<<surface<<endl;
    imageCurrent->init((XWindow*)surface,(YUVPicture*)&rect);
    imageCurrent->openImage(imageMode);
  }
  return (imageCurrent != NULL);
}


int SDLSurface::closeImage() {
  this->imageMode = _IMAGE_NONE;
  if (imageCurrent != NULL) {
    imageCurrent->closeImage();
  }
  imageCurrent=NULL;
  return true;
}

int SDLSurface::checkEvent(int* newMode) {
  int back=false;
  SDL_Event event;
  SDL_PollEvent(&event);
  switch (event.type) {
  case SDL_MOUSEBUTTONDOWN: {
    int button=event.button.button;
    if (button == 1) {
      *newMode = imageMode ^ _IMAGE_DOUBLE;
      back=true;
    }
    if (button == 3) {
      *newMode = imageMode ^ _IMAGE_DESK ^ _IMAGE_FULL;
      back=true;
    }
    break;
  }
  case SDL_VIDEORESIZE : {
    resize_rect.w = event.resize.w;
    resize_rect.h = event.resize.h;
    *newMode = imageMode | _IMAGE_RESIZE;   
    back = true;
    break;
  }

  }
  return back;
}

int SDLSurface::dither(YUVPicture* pic) {
  if (imageCurrent != NULL) {
    imageCurrent->ditherImage(pic);
    if(imageMode & _IMAGE_RESIZE) {
      imageCurrent->putImage(resize_rect.w, resize_rect.h);
    } else {
      imageCurrent->putImage(rect.w, rect.h);
    }
  }
  return true;
}

void SDLSurface::sdlinit() {
  if (lSDLInit == false) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) {
      fprintf(stderr, "Warning: Couldn't init SDL video: %s\n",
	      SDL_GetError());
      fprintf(stderr, "Will ignore video stream\n");
      exit(0);
    }
    atexit(SDL_Quit);
    cout << "****************** SDL VIDEO System **********"<<endl;
    /* Get the "native" video mode */
    video_info=SDL_GetVideoInfo();
    video_bpp=video_info->vfmt->BitsPerPixel;
    imageMode=_IMAGE_NONE;
    
    imageCurrent=NULL; 
  }
  lSDLInit=true;
}

#endif
