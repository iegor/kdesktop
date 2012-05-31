/*
  surface wrapper for X11 Window
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __X11SURFACE_H
#define __X11SURFACE_H

#include <limits.h>
#include "xinit.h"
#include "../surface.h"
#include "initDisplay.h"
#include "../dither/ditherWrapper.h"
#include "imageDeskX11.h"
#include "imageDGAFull.h"
#include "imageXVDesk.h"



class X11Surface : public Surface {

  int lOpen;
  int imageMode;
  
  XWindow* xWindow;

  ImageBase** imageList;
  int         images;
  
  ImageBase* imageCurrent;

  Atom WM_DELETE_WINDOW;
  Atom WM_RESIZE_WINDOW;
  int lXVAllow;

 public:
  X11Surface();
  ~X11Surface();

  int isOpen();
  int open(int width, int height,const char *title, bool border = false);
  int close();
  int getHeight();
  int getWidth();
  int getDepth();
  int getImageMode();
  int x11WindowId();
  
  ImageBase *findImage(int mode);
	  
  // these functions grant access to the supported images. be careful when changing
  // entries, because these are no copies. they are the original values!
  ImageBase **getModes();
  void setModes(ImageBase **modes);

  int openImage(int mode, YUVPicture* pic = NULL);
  int closeImage();
  int dither(YUVPicture* pic);
  int putImage(YUVPicture* pic);

  int checkEvent(int* mode);
  
  void config(const char* key, 
	      const char* value,void* user_data);


 private:
  int initX11();
  bool m_windowIdAvailable;
};
#endif
