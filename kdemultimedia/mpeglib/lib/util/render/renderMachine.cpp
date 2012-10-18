 /*
  puts the yuv images onto a surface
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "renderMachine.h"

#include <iostream>

using namespace std;


RenderMachine::RenderMachine() {

#ifndef SDL_WRAPPER 
  surface=new X11Surface();
#endif
#ifdef SDL_WRAPPER 
  surface=new SDLSurface();
#endif


  pictureArray=NULL;

  startTime=new TimeStamp();
  endTime=new TimeStamp();

  initialMode = _IMAGE_DESK;
}


RenderMachine::~RenderMachine() {


  closeWindow();
  delete surface;

  delete startTime;
  delete endTime;

}






void RenderMachine::waitRestTime() {
  endTime->gettimeofday();
  startTime->minus(endTime,endTime);
  endTime->waitForIt();

}

int RenderMachine::x11WindowId()
{
	return surface->x11WindowId();
}


int RenderMachine::openWindow(int width, 
			      int height,const char *title) {
if (surface->open(width,height,title)) {
    pictureArray=new PictureArray(width, height);
    
    return switchToMode(initialMode);
  }
  return false;
}


void RenderMachine::closeWindow() {
  
  if (surface->isOpen()==false) {
    return;
  }
  if (pictureArray != NULL) {
    delete pictureArray;
    pictureArray=NULL;
  }
  surface->close();
}

/**
   important method. This is our only way to switch from
   fullscreen back to the desktop screen.
   This method is called if the video end (normal or by user request)
   We dont have a callback, thus after the image stops, we would
   never get events.
*/
void RenderMachine::flushWindow() {

  // we always switch back to desk mode.
  if (IS_FULL(surface->getImageMode())) {
    switchToMode(surface->getImageMode() ^ _IMAGE_DESK ^ _IMAGE_FULL);
  }


}




void RenderMachine::putImage(YUVPicture* pic,
			     TimeStamp* waitTime,
			     TimeStamp* ) {
  if (pic == NULL) {
    cout << "pic is null"<<endl;
    return;
  }
  startTime->gettimeofday();
  startTime->addOffset(waitTime);

  // need dither?
  surface->dither(pic);
 
  int nextMode;
  if (surface->checkEvent(&nextMode) == true) {
    switchToMode(nextMode);
  }
  surface->putImage(pic);
  waitRestTime();
}


int RenderMachine::switchToMode(int mode) {
  if (surface->getImageMode() != mode) {
    surface->closeImage();
    if (mode != _IMAGE_NONE) {
      surface->openImage(mode);
    }
    
    else {
      cout << "no imageMode, no open, that's life"<<endl;
      return false;
    }
  }
  return true;
}



PictureArray* RenderMachine::lockPictureArray() {
  return pictureArray;
}


void RenderMachine::unlockPictureArray(PictureArray* pictureArray) {

  // chance to switch mode
  
  
  // put picture out
  if (surface->getImageMode() != _IMAGE_NONE) {
    YUVPicture* pic=pictureArray->getYUVPictureCallback();
    if (pic != NULL) {
      TimeStamp* waitTime=pic->getWaitTime();
      TimeStamp* earlyTime=pic->getEarlyTime();
      putImage(pic,waitTime,earlyTime);
    }
  } else {
    cout << "no mode selected"<<endl;
  }
}


void RenderMachine::config(const char* key, 
			   const char* value,void* user_data) {
  if (strcmp(key,"getDepth")==0) {
    int* val=(int*)user_data;
    *val=surface->getDepth();
  }
  if (surface != NULL) {
    int mode = surface->getImageMode();
    if (strcmp(key,"toggleFullscreen")==0) {
      if (surface->findImage(mode ^ _IMAGE_FULL) != NULL) {
	if (surface->isOpen())
	  switchToMode(mode ^ _IMAGE_FULL);
	else
	  initialMode = _IMAGE_FULL;
      }
    }
    if (strcmp(key,"toggleDouble")==0) {
      if (surface->findImage(mode ^ _IMAGE_DOUBLE) != NULL) {
	if (surface->isOpen())
	  switchToMode(mode ^ _IMAGE_DOUBLE);
	else
	  initialMode = _IMAGE_DOUBLE;
      }
    }
  }

  surface->config(key,value,user_data);
}



