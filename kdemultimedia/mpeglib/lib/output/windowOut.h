/*
  wrapper for X11 Window
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __WINDOWOUT_H
#define __WINDOWOUT_H

 
#include "../util/abstract/abs_thread.h"
#include "../util/timeStamp.h"
#include "../util/render/renderMachine.h"

/**
   Stupid class. survivor of ancient days.
*/


class WindowOut {


  RenderMachine* renderMachine;

 public:
  WindowOut();
  ~WindowOut();

  int openWindow(int width, int height,const char *title);
  int x11WindowId();
  void closeWindow();
  void flushWindow();

  // get the current surfaces to draw into
  PictureArray* lockPictureArray();
  void unlockPictureArray(PictureArray* pictureArray);

  void config(const char* key, const char* value,void* user_data);


 private:


};
#endif
