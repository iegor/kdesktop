/*
  surface base class
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __SURFACE_H
#define __SURFACE_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "yuvPicture.h"

class ImageBase;

class Surface {

 public:
  Surface();
  virtual ~Surface();

  virtual int isOpen();
  virtual int open(int width, int height,const char *title, bool border=false);
  virtual int close();
  virtual int getHeight();
  virtual int getWidth();
  virtual int getDepth();
  virtual int getImageMode();
  virtual int x11WindowId();
  
  virtual ImageBase *findImage(int imageMode);

  virtual int openImage(int mode, YUVPicture* pic = NULL);
  virtual int closeImage();
  virtual int dither(YUVPicture* pic);
  virtual int putImage(YUVPicture* pic);

  virtual int checkEvent(int* mode);

  // config surface
  virtual void config(const char* key, 
		      const char* value,void* user_data);


};
#endif
