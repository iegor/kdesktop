/*
  xfree 4.0 dga fullscreen mode
  Copyright (C) 2000  Martin Vogt, Christian Gerlach

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __IMAGEDGAFULL_H
#define __IMAGEDGAFULL_H
#include "xinit.h"

#include "../imageBase.h"



#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>




#define DGA_MINMAJOR 2
#define DGA_MINMINOR 0




/**
   
   Displays and renders X11 images in software with the help
   of the ditherWrapper class. It switches to xfree 4.0 dga 2.0
   and needs root priviliges for that
   
*/


class ImageDGAFull : public ImageBase {


  XWindow* m_pxWindow;

  // DGA status
  int m_iMajorVersion;
  int m_iMinorVersion;
  int m_iEventBase;
  int m_iErrorBase;
  
  int m_iScreen;
  
  Display    *m_pDisplay;
  
  int         m_iNumberModes;

#ifdef X11_DGA2
  XDGAMode   *m_pDGAModes;
  XDGADevice *m_pDevice;
#endif
  
  int         m_iScreenWidth;
  int         m_iScreenHeight;
  
  char       *m_pAddr;            // Base address of the screen
  
  // DGA parameter
  int  m_iVideoWidth;
  int  m_iVideoHeight;
  
  int   m_iBytesPerLine;
  int   m_iBytesPerRow;           // Size of one image line
  int   m_iBytesPerPixel;
  int   m_iOffsetScreen;
  int   m_iOffsetLine;
  char *m_pStartAddr;             // Start address for a centered image
  
  int  m_iImageMode;
  int  m_iMode;
  bool m_bZoom;
  bool m_bAllowZoom;

  bool m_bIsActive;

  int lSupport;
  DitherWrapper* ditherWrapper;


 public:
  ImageDGAFull();
  ~ImageDGAFull();

  void init(XWindow *xWindow, YUVPicture* pic = NULL);

  int support();

  int openImage(int mode);
  int closeImage();

  void ditherImage(YUVPicture* pic);
  void putImage();

  int active() { return m_bIsActive; }

 private:
  
  // Tries to find a fullscreen-mode which matches the resolution best
  int findMode(int width, int height, int bpp);

  // Returns TRUE if an event is waiting
  int event();

  // Returns the start address of the upper left corner of the video frame
  unsigned char *address();
  
  // Number of bytes from the end of a row to the beginning of next one
  int  offset();

  // Disables DGA-View (performs a mode-switch if neccesary)
  void stop();



};

#endif
