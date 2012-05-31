/*
  standard and shared mem  X11 images 
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "imageDeskX11.h"

#include <iostream>

using namespace std;

static int lXerror;

static int dummy(Display* , XErrorEvent*) {
  lXerror=true;
  return true;
}



ImageDeskX11::ImageDeskX11() {
  lSupport=true;
  supportedModes = _IMAGE_DESK | _IMAGE_DOUBLE | _IMAGE_FULL;
  setIdentifier("Standard X11");
  xWindow = NULL;
  ditherWrapper=NULL;
#ifdef X11_XVIDMODE
  iOldMode = -1;
  vm_modelines = NULL;
#endif
}


ImageDeskX11::~ImageDeskX11() {
  destroyImage();
  if (ditherWrapper != NULL) {
    delete ditherWrapper;
  }
}


void ImageDeskX11::init(XWindow* xWindow, YUVPicture*)
{
  videoaccesstype=VIDEO_XI_NONE;
  this->xWindow=xWindow;
  virtualscreen=NULL;
  ximage=NULL;
  imageMode=_IMAGE_NONE;
  if (ditherWrapper == NULL) {
    ditherWrapper=new DitherWrapper(xWindow->depth,
				    xWindow->redMask,
				    xWindow->greenMask,
				    xWindow->blueMask,
				    xWindow->pixel);
  }

#ifdef X11_SHARED_MEM
  shmseginfo=NULL;
#endif
}

int ImageDeskX11::support() {
  return lSupport;
}


int ImageDeskX11::openImage(int mode) {

  if (xWindow == NULL) {
    cout << "ImageDeskX11::openImage - call init before open!" << endl;
    return false;
  }
  
  closeImage();
  imageMode = mode;
  int err;

  if ((err=createImage(VIDEO_XI_SHMSTD,imageMode)) != ERR_XI_OK) {
    printf("\nX initialisation error:\n *** %s\n",ERR_XI_STR[err]);
    printf("check ipcs and delete resources with ipcrm\n");
    if ((err=createImage(VIDEO_XI_STANDARD,imageMode)) != ERR_XI_OK) {
      printf("\nX initialisation error:\n *** %s\n",ERR_XI_STR[err]);
      videoaccesstype=VIDEO_XI_NONE;
    } else {
      lSupport=true;
    }
  } else {
    lSupport=true;
  }
  switch(videoaccesstype)  {
    case VIDEO_XI_STANDARD:
      //printf("  # using conventional Xlib calls.\n\n");
      break;
    case VIDEO_XI_SHMSTD:
      //printf("  # Using Xlib shared memory extension %d.%d\n\n",
      //XShmMajor,XShmMinor);
      break;
  default:
    cout << "could not create image->no video output possible"<<endl;

  }  

  iOffsetX = iOffsetY = 0;
  int w = xWindow->width;
  int h = xWindow->height;
  if (IS_FULL(imageMode)) {
    switchMode(xWindow->width, xWindow->height, IS_DOUBLE(imageMode));
    iOffsetX = (iWidth -  w) / 2;
    iOffsetY = (iHeight - h) / 2;
    if (bZoom) {
      iOffsetX -= w / 2;
      iOffsetY -= h / 2;
    }
    XResizeWindow(xWindow->display, xWindow->window, iWidth, iHeight);
  } else if (IS_DOUBLE(imageMode)) {
    XResizeWindow(xWindow->display, xWindow->window,
		  xWindow->width * 2, xWindow->height * 2);
  }

  if (lSupport==true) {
    return true;
  }
  return false;
}


int ImageDeskX11::closeImage() {
  destroyImage();

#ifdef X11_XVIDMODE
  if (iOldMode != -1) {
    cout << "switch back to original videomode" << endl;
    XF86VidModeSwitchToMode(xWindow->display,XDefaultScreen(xWindow->display),
			    vm_modelines[iOldMode]);
    XFlush(xWindow->display);
    iOldMode=-1;
  }
#endif
  
  return true;
}


void ImageDeskX11::ditherImage(YUVPicture* pic) {
  if (xWindow == NULL) {
    cout << "ImageDeskX11::ditherImage - you have to call init first!" << endl;
    return;
  }
  
  ditherWrapper->doDither(pic,xWindow->depth,imageMode,
			  virtualscreen,0);
}


void ImageDeskX11::putImage(){
  if (xWindow == NULL) {
    cout << "ImageDeskX11::putImage - you have to call init first!" << endl;
    return;
  }
  

  int height=xWindow->height;
  int width=xWindow->width;

  if (imageMode & _IMAGE_DOUBLE) {
    height=2*height;
    width=2*width;
  }
  
#ifdef X11_SHARED_MEM
  switch(videoaccesstype) {
    case VIDEO_XI_SHMSTD:
      XShmPutImage(xWindow->display,xWindow->window, 
		   xWindow->gc,ximage,
		   0, 0, iOffsetX, iOffsetY, width, height, False);
      XSync(xWindow->display,false); /* true not needed, done by XPending */
      break;
      
      
    case VIDEO_XI_STANDARD:
#endif
      XPutImage(xWindow->display,xWindow->window,
		xWindow->gc, ximage,
		0, 0, iOffsetX, iOffsetY, width, height);      
      XSync(xWindow->display,false); /* true not needed, done by XPending */
#ifdef X11_SHARED_MEM
      break;
    }
#endif
}



int ImageDeskX11::createImage(int createType,int mode) {

  if (xWindow == NULL) {
    cout << "ImageDeskX11::createImage - you have to call init first!" << endl;
    return false;
  }
  
  videoaccesstype=VIDEO_XI_NONE;

#ifdef X11_SHARED_MEM
  if(XShmQueryVersion(xWindow->display,&XShmMajor,&XShmMinor,&XShmPixmaps)) {
    if (XShmPixmaps==True) {
      if (createType & VIDEO_XI_SHMSTD) {
	videoaccesstype=VIDEO_XI_SHMSTD;
      }
    }
  } else {
    if (createType & VIDEO_XI_SHMSTD) {
      return ERR_XI_NOSHAREDMEMORY;
    }
  }
#endif
  if (videoaccesstype == VIDEO_XI_NONE) {
    videoaccesstype=createType;
  }
    
  switch(videoaccesstype)
    {
#ifdef X11_SHARED_MEM


    case VIDEO_XI_SHMSTD:

      lXerror=false;
      XSetErrorHandler(dummy);

      shmseginfo=(XShmSegmentInfo *)malloc(sizeof(XShmSegmentInfo));
      if(!shmseginfo)
	return ERR_XI_SHMALLOC;

      memset(shmseginfo,0, sizeof(XShmSegmentInfo));

      if (imageMode & _IMAGE_DOUBLE) {
	ximage=XShmCreateImage(xWindow->display,xWindow->visual,
			       xWindow->depth,
			       ZPixmap,NULL,shmseginfo,2*xWindow->width,
			       2*xWindow->height);
      } else {
	ximage=XShmCreateImage(xWindow->display,xWindow->visual,
			       xWindow->depth,
			       ZPixmap,NULL,shmseginfo,xWindow->width,
			       xWindow->height);
      }
      
      if(!ximage)
	return ERR_XI_SHMXIMAGE;
      
      shmseginfo->shmid=shmget(IPC_PRIVATE,
			       ximage->bytes_per_line*
			       ximage->height,IPC_CREAT|0777);

      if(shmseginfo->shmid<0)
	return ERR_XI_SHMSEGINFO;
      
      shmseginfo->shmaddr=(char*)shmat(shmseginfo->shmid,NULL,0);
      ximage->data=shmseginfo->shmaddr;
      virtualscreen=(unsigned char *)ximage->data;

      if(!virtualscreen)
	return ERR_XI_SHMVIRTALLOC;

      shmseginfo->readOnly=False;

      XShmAttach(xWindow->display,shmseginfo);
      XSync(xWindow->display, False);
      XSetErrorHandler(NULL);
      XFlush(xWindow->display);
      if (lXerror) {
	cout << "ERR_XI_SHMATTACH -2"<<endl;
	return ERR_XI_SHMATTACH;
      }

      break;
#endif

    case VIDEO_XI_STANDARD:
      if (mode & _IMAGE_DOUBLE) {
	virtualscreen=(unsigned char *)
	  malloc(xWindow->screensize*sizeof(char)*4);
	
	if(virtualscreen==NULL)
	  return ERR_XI_VIRTALLOC;
	
	ximage=XCreateImage(xWindow->display,xWindow->visual,
			    xWindow->depth,ZPixmap,
			    0,(char*)virtualscreen,
			    2*xWindow->width,2*xWindow->height,
			    32,2*xWindow->width*xWindow->pixelsize);
      } else {
	virtualscreen=(unsigned char *)
	  malloc(xWindow->screensize*sizeof(char));
	
	if(virtualscreen==NULL)
	  return ERR_XI_VIRTALLOC;
	
	ximage=XCreateImage(xWindow->display,xWindow->visual,
			    xWindow->depth,ZPixmap,
			    0,(char*)virtualscreen,
			    xWindow->width,xWindow->height,
			    32,xWindow->width*xWindow->pixelsize);
      }
      
      if(!ximage)
	return ERR_XI_XIMAGE;
      break;
      
    default:
      return ERR_XI_FAILURE;

    }

  if ( (videoaccesstype == VIDEO_XI_STANDARD) ||
       (videoaccesstype == VIDEO_XI_SHMSTD) ) {
#ifndef WORDS_BIGENDIAN
    ximage->byte_order = LSBFirst;
    ximage->bitmap_bit_order = LSBFirst;
#else
    ximage->byte_order = MSBFirst;
    ximage->bitmap_bit_order = MSBFirst;
#endif
    
  }
  return ERR_XI_OK;
}



int ImageDeskX11::destroyImage() {
  if(xWindow && xWindow->display && xWindow->window) {
    switch(videoaccesstype) {
#ifdef X11_SHARED_MEM
    case VIDEO_XI_SHMSTD:
      if (shmseginfo) {
	XShmDetach(xWindow->display,shmseginfo);
	if(ximage) {
	  XDestroyImage(ximage);
	  ximage=NULL;
	}
	if(shmseginfo->shmaddr) {
	  shmdt(shmseginfo->shmaddr);
	  shmseginfo->shmaddr=NULL;
	}
	if(shmseginfo->shmid>=0)
	  shmctl(shmseginfo->shmid,IPC_RMID,NULL);
	
	free(shmseginfo);
      }
      shmseginfo=NULL;
      break;
      
#endif
    case VIDEO_XI_STANDARD:
      if(ximage) {
	XDestroyImage(ximage);
	ximage=NULL;
	/*
	  XDestroyImage function calls frees both the image structure
          and the data pointed to by the image structure.
	*/
	virtualscreen=NULL;
      }
      break;

    default:
		//      cout << "no open window to close"<<endl;
		break;
    }
  }
  videoaccesstype=VIDEO_XI_NONE;
  imageMode=_IMAGE_NONE;
  return true;
}


bool ImageDeskX11::switchMode(int width, int , bool zoom)
{
	iWidth = xWindow->screenptr->width;
	iHeight = xWindow->screenptr->height;

#ifdef X11_XVIDMODE
	iOldMode = -1;
	int vm_count,i;

	cout << "Find best matching videomode ..." << endl;

	if (!XF86VidModeGetAllModeLines(xWindow->display,XDefaultScreen(xWindow->display),
									&vm_count,&vm_modelines)) {
		return false;
	}

	int bestMode = -1;
	int border, minBorder = INT_MAX;

	for (i = 0; i < vm_count; i++) {
		printf("mode %d: %dx%d\n",i, vm_modelines[i]->hdisplay,vm_modelines[i]->vdisplay);

		if (xWindow->screenptr->width == vm_modelines[i]->hdisplay)
			iOldMode = i;
			
		border = vm_modelines[i]->hdisplay - width;
		if ((border > 0) && (border < minBorder)) {
			bestMode = i;
			minBorder = border;
			bZoom = false;
		}
		if (zoom) {
			border = vm_modelines[i]->hdisplay - 2 * width;
			if ((border > 0) && (border < minBorder)) {
				bestMode = i;
				minBorder = border;
				bZoom = true;
			}
		}
	}
	cout << "best mode: " << bestMode << endl;

	iWidth = vm_modelines[bestMode]->hdisplay;
	iHeight = vm_modelines[bestMode]->vdisplay;
	
	if (XF86VidModeSwitchToMode(xWindow->display,XDefaultScreen(xWindow->display),
								vm_modelines[bestMode])) {
		XF86VidModeSetViewPort(xWindow->display,XDefaultScreen(xWindow->display), 0, 0);
		XFlush(xWindow->display);
		return true;
	}
#endif
	return false;
}
