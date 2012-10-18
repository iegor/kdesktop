/*
  xfree 4.0 XV extension desk mode
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __IMAGEXVDESK_H
#define __IMAGEXVDESK_H

#include "xinit.h"

#include "../imageBase.h"
#include "../dither2YUV/dither2YUV.h"

#include <stdio.h>

#if !defined(__NetBSD__)
#include <semaphore.h>
#endif

//#undef X11_XV

#define GUID_YUV12_PLANAR 0x32315659
#define GUID_I420_PLANAR  0x30323449
#define GUID_YUY2_PACKED  0x32595559
#define GUID_UYVY_PACKED  0x59565955

/**
   The XV extension dither yuv images in hardware and allows 
   scaling of images.
   But its currently not supported by many drivers.

*/


class ImageXVDesk : public ImageBase {

#ifdef X11_XV
  XvAdaptorInfo		  *ai;
  XvEncodingInfo	  *ei;
  XvAttribute		  *at;
  XvImageFormatValues *fo;

  XvImage		 *yuv_image;
  bool            keepRatio;

  int		      xv_port;
  int             imageID;
  
  int 			  shmem_flag;
  XShmSegmentInfo yuv_shminfo;
#endif
  Dither2YUV*    ditherWrapper;

  int lSupport;
  XWindow* xWindow;

 public:
  ImageXVDesk();
  ~ImageXVDesk();

  void init(XWindow* xWindow, YUVPicture* pic = NULL);

  int support();

  int openImage(int imageMode);
  int closeImage();

  void ditherImage(YUVPicture* pic);
  void putImage();

  void setKeepRatio(bool enable);
  
 private:
  int haveXVSupport(XWindow* xWindow);
  void freeImage();
  void createImage(int id);

};

#endif
