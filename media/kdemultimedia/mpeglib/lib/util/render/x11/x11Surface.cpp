/*
  surface wrapper for X11 Window
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "x11Surface.h"

#include <iostream>

using namespace std;

const char *ERR_XI_STR[] = {
  "X initialisation OK!",
  "No Shared Memory available", 
  "cannot open Display", 
  "bad color depth",
  "can't create Window", 
  "can't alloc memory for virtual screen",
  "cannot create XImage",
  "can't alloc memory for Shared memory segment info",
  "cannot create Shared Memory XImage",
  "Shared memory segment info error",
  "Shared memory virtual screen allocation failed",
  "cannot attach Shared Memory segment to display"
};


#ifndef KDE_USE_FINAL
static int dummy(Display* , XErrorEvent*) {
  cout << "received x11 error!"<<endl;
  return true;
}
#endif

X11Surface::X11Surface() {
  xWindow=(XWindow *)malloc(sizeof(XWindow));
  xWindow->lOpen=false;
  xWindow->x = xWindow->y = 0;
  xWindow->window = 0;
  m_windowIdAvailable = false;
  
  imageMode=_IMAGE_NONE;

  imageCurrent=NULL;
  xWindow->lOpen=false;


  xWindow->display=XOpenDisplay(NULL);
  if (xWindow->display)
     XFlush(xWindow->display);
  xWindow->redMask=0;
  xWindow->greenMask=0;
  xWindow->blueMask=0;
  lXVAllow=true;
  
  images=0;
  imageList = new ImageBase* [4];
  imageList[images++] = new ImageXVDesk();
  imageList[images++] = new ImageDGAFull();
  imageList[images++] = new ImageDeskX11();
  imageList[images] = NULL;
}


X11Surface::~X11Surface() {
  close();
  if (xWindow->display)
     XCloseDisplay(xWindow->display);
  free(xWindow);

  for (int count=0 ; count<images ; count++) {
	  if (imageList[count] != NULL)
		  delete imageList[count];
  }
  delete [] imageList;
}


int X11Surface::getHeight() {
  return xWindow->height;
}


int X11Surface::getWidth() {
  return xWindow->width;
}

int X11Surface::isOpen() {
  return xWindow->lOpen;
}

int X11Surface::x11WindowId() {
  if(m_windowIdAvailable)
  	return xWindow->window;
  else
    return -1;
}

int X11Surface::open(int width, int height,const char *title, bool border) {

  close();
  xWindow->width=width;
  xWindow->height=height;

  if(!xWindow->display) {
    printf("\nX initialisation error:\n *** %s\n",ERR_XI_STR[ERR_XI_DISPLAY]);
    printf("check ipcs and delete resources with ipcrm\n");
    exit(0);
  }

  xWindow->screennum=DefaultScreen(xWindow->display);
  xWindow->screenptr=DefaultScreenOfDisplay(xWindow->display);
  xWindow->visual=DefaultVisualOfScreen(xWindow->screenptr);
  xWindow->depth=DefaultDepth(xWindow->display,xWindow->screennum);

  switch(xWindow->depth) {
    case 8:
      xWindow->pixelsize=1;
      break;
    case 16:
      xWindow->pixelsize=2;
      break;
    case 24:
      xWindow->pixelsize=4;
      break;
    case 32:
      xWindow->pixelsize=4;
      break;
   default:
     cout << "unknown pixelsize for depth:"<<xWindow->depth<<endl;
     exit(0);
  }

  XColor background, ignored;
  XAllocNamedColor (xWindow->display,
		    DefaultColormap (xWindow->display, xWindow->screennum),
		    "black", &background, &ignored);
  
  XSetWindowAttributes attributes;
  attributes.background_pixel=background.pixel;
  attributes.backing_store=NotUseful;
  attributes.override_redirect=True;

  xWindow->window=XCreateWindow(xWindow->display,
				RootWindowOfScreen(xWindow->screenptr), 
				0,0,
				xWindow->width, 
				xWindow->height,0,
				xWindow->depth,
				InputOutput, xWindow->visual,
				(border) ? CWBackingStore : CWBackPixel|CWOverrideRedirect,
				&attributes);

  m_windowIdAvailable = true;
  if(!xWindow->window) {
    printf("\nX initialisation error:\n *** %s\n",ERR_XI_STR[ERR_XI_WINDOW]);
    printf("check ipcs and delete resources with ipcrm\n"); 
    return false;
  }

  WM_DELETE_WINDOW = XInternAtom(xWindow->display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(xWindow->display, xWindow->window, &WM_DELETE_WINDOW, 1);

  XSetErrorHandler(dummy);

  XStoreName(xWindow->display,xWindow->window,title);
  XSelectInput(xWindow->display,xWindow->window,
	       ExposureMask|KeyPressMask|KeyReleaseMask|ButtonPressMask);
  xWindow->gc=XCreateGC(xWindow->display,xWindow->window,0,NULL);
  XMapRaised(xWindow->display,xWindow->window);

  if (xWindow->depth >= 16) {
    initColorDisplay(xWindow);

  } else {
    // depth is <= 8
    // allocate memory for dithertables
    // gets the rgb masks
    initColorDisplay(xWindow);
    // create 8 bit dithertables 
    // create private colormap
    initSimpleDisplay(xWindow);
   
  }

  xWindow->palette=NULL;
  xWindow->screensize=xWindow->height*xWindow->width*xWindow->pixelsize;
  xWindow->lOpen=true;

  for (int count=0 ; count<images ; count++) {
    if (imageList[count] != NULL)
      imageList[count]->init(xWindow);
  }
  
  return true;
}


int X11Surface::close() {
  if (isOpen()==false) {
    return true;
  }
  closeImage();
  
  XFreeGC(xWindow->display,xWindow->gc);
  XDestroyWindow(xWindow->display,xWindow->window);

  xWindow->lOpen=false;


  return true;
}

ImageBase *X11Surface::findImage(int mode) {
  for (int count=0 ; count<images ; count++) {
    if ((imageList[count] == NULL) || (IS_DISABLED(imageList[count])))
      continue;
    
    if (imageList[count]->supportedModes & mode)
      return imageList[count];
  }
  return NULL;
}

ImageBase **X11Surface::getModes() {
	return imageList;
}

void X11Surface::setModes(ImageBase **modes) {
	imageList = modes;
}

int X11Surface::openImage(int mode, YUVPicture*) {
  if (imageMode != _IMAGE_NONE) {
    cout << "bad open error X11Surface::openImage"<<endl;
    return false;
  }
  if (mode == _IMAGE_NONE) {
    cout << "X11Surface::openImage - no valid mode specified"<<endl;
    return false;
  }

  ImageBase *newImage=findImage(mode);
  
  if (newImage == NULL) {
    cout << " X11Surface::openImage - no matching image found"<<endl;
    imageMode=_IMAGE_NONE;
  } else {
    /*
      printf("Best image found: %s\n", newImage->getIdentifier());
      printf("\tsupported modes: desk=%d, double=%d, full=%d, resize=%d\n",
      HAS_DESK(newImage),
      HAS_DOUBLE(newImage),
      HAS_FULL(newImage),
      HAS_RESIZE(newImage));
    */
    
    open(xWindow->width, xWindow->height, "mpeglib", !(mode & _IMAGE_FULL));
    newImage->openImage(mode);
    if (!IS_FULL(mode)) {
      XMoveWindow(xWindow->display, xWindow->window,
		  xWindow->x, xWindow->y);
      
      XSizeHints hints;
      hints.flags = PMaxSize;
      if (HAS_RESIZE(newImage)) {
	hints.max_width = INT_MAX;
	hints.max_height = INT_MAX;
      } else {
	hints.max_width = xWindow->width;
	hints.max_height = xWindow->height;
      }
      XSetWMNormalHints(xWindow->display, xWindow->window, &hints);
    }
    
    imageMode=mode;
  }
  imageCurrent = newImage;
  XSync(xWindow->display,true);
  return (imageCurrent != NULL);
}


int X11Surface::closeImage() {

  if ((imageMode == _IMAGE_NONE) || (!xWindow->lOpen))
    return false;

  ImageBase *old = imageCurrent;
  imageCurrent=NULL;
	  
  XWindowAttributes attr;
  Window junkwin;
  
  if (!IS_FULL(imageMode)) {
    if (!XGetWindowAttributes(xWindow->display, xWindow->window, &attr))
      cout << "Can't get window attributes." << endl;
    
    XTranslateCoordinates (xWindow->display, xWindow->window, attr.root, 
			   -attr.border_width,
			   -attr.border_width,
			   &xWindow->x, &xWindow->y, &junkwin);
  }
  
  imageMode=_IMAGE_NONE;
  old->closeImage();

  return true;
}


int X11Surface::dither(YUVPicture* pic) {
  if (imageCurrent != NULL) {
    imageCurrent->ditherImage(pic);
  }
  return true;
}


int X11Surface::putImage(YUVPicture* ) {
  if (imageCurrent != NULL) {
    imageCurrent->putImage();
  }
  return true; 
}


int X11Surface::getDepth() {
  return xWindow->depth;
}

int X11Surface::getImageMode() {
  return imageMode;
}

int X11Surface::checkEvent(int* newMode) {
  XEvent event;

  if (isOpen()==false)
    return false;

  // check if we forward the call to the FULLSCREEN mode
  if (!imageCurrent->active()) {
    if (IS_FULL(imageMode)) {
      *newMode=imageMode ^ _IMAGE_FULL;
      return true;
    }
  }
  
  // normal X11 images use the X11 event queue
  if (XCheckTypedWindowEvent(xWindow->display, 
			     xWindow->window,ButtonPress,&event)) {
    if (event.xbutton.button == Button1) {
      if (findImage(_IMAGE_DOUBLE) != NULL)
	*newMode = imageMode ^ _IMAGE_DOUBLE;
    } else if (event.xbutton.button == Button3) {
      if (findImage(_IMAGE_FULL) != NULL)
	*newMode = imageMode ^ _IMAGE_DESK ^ _IMAGE_FULL;
    }
    return true;
  }
  // now check if there are unneeded events in the queue,
  // then delete them
  int eventCnt=XPending(xWindow->display);
  if (eventCnt > 10) {
    XSync(xWindow->display,true);
  }
  return false;


}



void X11Surface::config(const char* key,
			const char* value, void* ) {
  if (strcmp(key,"xvAllow")==0) {
	  lXVAllow=atoi(value);
  }
}
 
