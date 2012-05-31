/*
  standard and shared mem  X11 images 
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __IMAGEDESKX11_H
#define __IMAGEDESKX11_H
#include <limits.h>
#include "xinit.h"

#include "../imageBase.h"

#define VIDEO_XI_NONE         0x00 /* No access defined */
#define VIDEO_XI_STANDARD     0x01 /* Use standard Xlib calls */
#define VIDEO_XI_SHMSTD       0X02 /* Use Xlib shared memory extension */

/**
   
   Displays and renders X11 images in software with the help
   of the ditherWrapper class.
*/


class ImageDeskX11 : public ImageBase {

#ifdef X11_SHARED_MEM
  XShmSegmentInfo *shmseginfo;
#endif
  
  unsigned char *virtualscreen;
  int videoaccesstype;
  XImage *ximage;
  int lSupport;

  int XShmMajor,XShmMinor;
  Bool XShmPixmaps;

  XWindow* xWindow;
  int imageMode;
  DitherWrapper* ditherWrapper;

  int iOffsetX;
  int iOffsetY;
  int iWidth;
  int iHeight;

#ifdef X11_XVIDMODE
  XF86VidModeModeInfo **vm_modelines;

  int iOldMode;
#endif

  bool bZoom;

 public:
  ImageDeskX11();
  ~ImageDeskX11();

  void init(XWindow* xWindow, YUVPicture* pic = NULL);
  
  int support();

  int openImage(int ditherSize);
  int closeImage();

  void ditherImage(YUVPicture* pic);
  void putImage();

 private:
  int createImage(int createType,int size);
  int destroyImage();

  bool switchMode(int width, int height, bool zoom);

};

#endif
