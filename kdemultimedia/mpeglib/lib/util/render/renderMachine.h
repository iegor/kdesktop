/*
  puts the yuv images onto a surface
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __RENDERMACHINE_H
#define __RENDERMACHINE_H




/**
   RenderMachine. We still have the problem, because of performance,
   that we cannot have a yuv picture format in the decoder
   and one in the output to x11. they must be shared.
   XV support then directly works on them and SDL images
   can be exported to the decoder as well.

   Another point is : mode switch. User want desktop->fullscreen switch.
   Due to the threaded nature, we must have a single synchronization
   point, when we know that the decoder currently does _not_ decode
   so that we can switch the imaged and free the memory.

   Some points are currently unclear, for example how to handle 
   applications, which want to redirect the image into their own 
   buffers, but hey, there are that many classes and layers
   I really think it should be doable somehow

*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef SDL_WRAPPER
#include "x11/x11Surface.h"
#endif

#ifdef SDL_WRAPPER
#include "sdl/sdlSurface.h"
#endif

#include "pictureArray.h"
#include "../abstract/abs_thread.h"

class RenderMachine {


  Surface* surface;
  PictureArray* pictureArray;

  
  TimeStamp* startTime;
  TimeStamp* endTime;

  int initialMode;

 public:
  RenderMachine();
  ~RenderMachine();

  int openWindow(int width, int height,const char *title);
  int x11WindowId();
  void closeWindow();
  void flushWindow();

  
  PictureArray* lockPictureArray();
  void unlockPictureArray(PictureArray* pictureArray);

  void config(const char* key, const char* value,void* user_data);

 private:
  void waitRestTime();
  void putImage(YUVPicture* pic,TimeStamp* waitTime,TimeStamp* earlyTime);

  int switchToMode(int mode);

};
#endif
