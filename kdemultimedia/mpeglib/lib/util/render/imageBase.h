/*
  base clase for X11 images (stores important parameters and dither pic)
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __IMAGEBASE_H
#define __IMAGEBASE_H


#include <stdio.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "yuvPicture.h"
#include "dither/ditherWrapper.h"



#define _IMAGE_NONE         0
#define _IMAGE_DESK         1
#define _IMAGE_FULL         2
#define _IMAGE_DOUBLE       4
#define _IMAGE_RESIZE       8
#define _IMAGE_DISABLED    16


#define HAS_DESK(image)        ((((image)->supportedModes) & _IMAGE_DESK) > 0)
#define HAS_FULL(image)        ((((image)->supportedModes) & _IMAGE_FULL) > 0)
#define HAS_DOUBLE(image)      ((((image)->supportedModes) & _IMAGE_DOUBLE) > 0)
#define HAS_RESIZE(image)      ((((image)->supportedModes) & _IMAGE_RESIZE) > 0)
#define IS_DISABLED(image)     ((((image)->supportedModes) & _IMAGE_DISABLED) > 0)

#define IS_DESK(mode)           (((mode) & _IMAGE_DESK) > 0)
#define IS_FULL(mode)           (((mode) & _IMAGE_FULL) > 0)
#define IS_DOUBLE(mode)         (((mode) & _IMAGE_DOUBLE) > 0)
#define IS_RESIZEABLE(mode)     (((mode) & _IMAGE_RESIZE) > 0)


/**
   This class creates from a given X11 Window a special image to
   display.
   An image has some characteristics, like startadress, width height,
   if each row has a terminating modifier etc...

   The image is resposible for the conversion from the yuv
   format to the destination.

   It is initialized with the constructed x11Window.
   During a mode-switch (which is handled by x11window) the following
   sequence is called:

   support()   ->true/false  if this image type is supported
                             (switching to it is allowed)

   openImage()               called once when we switch to this
                             image type

   ditherImage(..)           for the conversion from yuv->rgb
                             obviously called for every image
   putImage(..)              time for display it

   closeImage()              called once, when we leave this image type


   This sequence is necessary, because the user likey to switch
   form desktop display to dga fullscreen.
   
   The following image classes seems to be useful:

   imageDeskX11   : standard ximage, maybe with shared mem support
                    full software rendering
   imageDeskXV    : image with hardware yuv->rgb rendering

   imageDGAFull   : dga 2.0 full software rendering (needs root)
   imageXVFull    : fullscreen hardware yuv->rgb rendering

   The hierarchy is as follows:

   (desk mode)
   imageStdX11  : fallback, should work everywhere
   imageStdXV   : if supported imageStdX11 is disabled

   (fullscreen mode)
   imageDGAFull : 
   imageXVFull  :
		   
   The supported switches between the modes are


   desktop          <->         fullscreen mode.
   

*/


class XWindow;

class ImageBase {
 private:
  char        *identifier;	

 public:
  unsigned int supportedModes;

 public:
  ImageBase();
  virtual ~ImageBase();

  virtual void init(XWindow* xWindow, YUVPicture* pic = NULL);
  
  virtual int support();

  virtual int openImage(int imageMode);
  virtual int closeImage();

  virtual void ditherImage(YUVPicture* pic);
  virtual void putImage();
  virtual void putImage(int w,int h);

  virtual int active() { return true; }

  void setIdentifier(const char *id);
  char *getIdentifier();

};

#endif

