/*
  xfree 4.0 XV extension desk mode
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#include "imageXVDesk.h"

#include <iostream>

using namespace std;

ImageXVDesk::ImageXVDesk() {

  lSupport=false;
 
  ditherWrapper=NULL;
  supportedModes = _IMAGE_NONE;
  setIdentifier("XV");

  xWindow = NULL;

#ifdef X11_XV
  keepRatio = false;
  
#endif
}


ImageXVDesk::~ImageXVDesk() {
  if (ditherWrapper != NULL) {
    delete ditherWrapper;
  }
  freeImage();
}

void ImageXVDesk::init(XWindow* xWindow, YUVPicture*)
{
#ifdef X11_XV
  this->xWindow=xWindow;

  xv_port=-1;
  shmem_flag = 0;
  yuv_image=NULL;
  yuv_shminfo.shmaddr=NULL;
  yuv_shminfo.shmid=-1;
  
  if (XShmQueryExtension(xWindow->display)) shmem_flag = 1;
  if (!shmem_flag) {
    printf("no shmem available.\n");
    return;
  }
  

  if (haveXVSupport(xWindow)==true) {
    supportedModes = _IMAGE_DESK | _IMAGE_DOUBLE | _IMAGE_FULL | _IMAGE_RESIZE;
    lSupport=true;
  } else {
    return;
  }

  if (ditherWrapper == NULL) {
    ditherWrapper=new Dither2YUV();
  }
  imageID = -1;
#endif
}

int ImageXVDesk::support() {
  return lSupport;
}

int ImageXVDesk::openImage(int imageMode) {

  
  if (imageMode & _IMAGE_FULL) {
    XResizeWindow(xWindow->display, xWindow->window,
		  xWindow->screenptr->width, xWindow->screenptr->height);
    setKeepRatio(true);
  } else if (imageMode & _IMAGE_DOUBLE) {
    XResizeWindow(xWindow->display, xWindow->window,
		  xWindow->width * 2, xWindow->height * 2);
    setKeepRatio(false);
  } else {
    setKeepRatio(false);
  }
  
  return true;
}


int ImageXVDesk::closeImage() {
  freeImage();
  return true;
}

void ImageXVDesk::ditherImage(YUVPicture* pic) {

#ifdef X11_XV
  int x_return;
  int y_return;
  int height, dy; 
  unsigned int	border_width_return;
  unsigned int	depth_return;
  unsigned int  _w;
  unsigned int  _h;
  Window	_dw;
  
  if (xWindow == NULL) {
    cout << "ImageXVDesk::ditherImage - you have to call before dithering an image!" << endl;
    return;
  }
  
  // check for not supported formats and if possible convert them
  int inputType=pic->getImageType();
  if (inputType == PICTURE_RGB_FLIPPED) {
    cout << "xv for flipped rgb not implemented"<<endl;
    return;
  }
  
  // create xv image 
  int id;
  if (imageID != pic->getImageType()) {
    imageID = pic->getImageType();
    switch (imageID) {
    case PICTURE_YUVMODE_CR_CB:
    case PICTURE_YUVMODE_CB_CR:
    case PICTURE_RGB:
      id = GUID_YUV12_PLANAR;
      break;
    case PICTURE_YUVMODE_YUY2:
      id = GUID_YUY2_PACKED;
      break;
    case PICTURE_YUVMODE_UYVY:
      id = GUID_UYVY_PACKED;
      break;
    default:
      cout << "unknown type for yuv image!" << endl;
      return;
    }
    freeImage();
    
    createImage(id);
  }
  
  XGetGeometry(xWindow->display,(Drawable)xWindow->window,
	       &_dw, &x_return, &y_return, &_w, &_h,
	       &border_width_return, &depth_return);
  
  // now dither the image
  
  // we (currently) cannot create yuvPicture _in_
  // the shared segment here we copy it
  
  unsigned char* image=pic->getImagePtr();
  if (inputType == PICTURE_RGB) {
    ditherWrapper->doDither(pic, 
			    DefaultDepth(xWindow->display,xWindow->screennum),
			    _SIZE_NORMAL, (unsigned char*) yuv_image->data, 0);
  } else {
    memcpy(yuv_image->data,image,pic->getImageSize());
  }
	
  if (keepRatio) {
    height = (_w * yuv_image->height) / yuv_image->width;
    dy = (((int) _h) - height + 1) / 2;
    XvShmPutImage(xWindow->display, xv_port,xWindow->window,
		  xWindow->gc, yuv_image,
		  0, 0, yuv_image->width, yuv_image->height,
		  0, dy, _w, height, False);
    if (dy > 0) {
		XFillRectangle(xWindow->display, xWindow->window,xWindow->gc, 
					   0, 0, _w, dy);
		XFillRectangle(xWindow->display, xWindow->window,xWindow->gc, 
					   0, height+dy-1, _w, dy+1);
	}
  } else {
    XvShmPutImage(xWindow->display, xv_port,xWindow->window,
		  xWindow->gc, yuv_image,
		  0, 0, yuv_image->width, yuv_image->height,
		  0, 0, _w, _h, False);
  }
#endif
}


void ImageXVDesk::putImage() {

  //XFlush(xWindow->display);      
  XSync(xWindow->display, false);      
}

void ImageXVDesk::setKeepRatio(bool enable)
{
#ifdef X11_XV
  keepRatio = enable;
#endif
}


int ImageXVDesk::haveXVSupport(XWindow* xWindow) {
#ifdef X11_XV
  int ret;
  unsigned int		p_version=0;
  unsigned int          p_release=0;
  unsigned int          p_request_base=0;
  unsigned int          p_event_base=0;
  unsigned int          p_error_base=0;

  unsigned int		p_num_adaptors=0;

  /**------------------------------- XV ------------------------------------*/
  
  /** query and print Xvideo properties */

  ret = XvQueryExtension(xWindow->display, 
			 &p_version, &p_release, &p_request_base,
			 &p_event_base, &p_error_base);
  if (ret != Success) {
    if (ret == XvBadExtension) {
      printf("XvBadExtension returned at XvQueryExtension.\n");
    } else if (ret == XvBadAlloc) {
      printf("XvBadAlloc returned at XvQueryExtension.\n");
    } else {
      printf("other error happened at XvQueryExtension.\n");
    }
    return false;
  }
  /*
    printf("========================================\n");
    printf("XvQueryExtension returned the following:\n");
    printf("p_version      : %u\n", p_version);
    printf("p_release      : %u\n", p_release);
    printf("p_request_base : %u\n", p_request_base);
    printf("p_event_base   : %u\n", p_event_base);
    printf("p_error_base   : %u\n", p_error_base);
    printf("========================================\n");
  */
  
  ret = XvQueryAdaptors(xWindow->display, DefaultRootWindow(xWindow->display),
			&p_num_adaptors, &ai);
  
  if (ret != Success) {
    if (ret == XvBadExtension) {
      printf("XvBadExtension returned at XvQueryExtension.\n");
    } else if (ret == XvBadAlloc) {
      printf("XvBadAlloc returned at XvQueryExtension.\n");
    } else {
      printf("other error happaned at XvQueryAdaptors.\n");
    }
    return false;
  }
  /*
  printf("=======================================\n");
  printf("XvQueryAdaptors returned the following:\n");
  printf("%d adaptors available.\n", p_num_adaptors);
  */
  if (p_num_adaptors == 0) {
    //cout << "no adaptors found. XV not possible"<<endl;
    return false;
  }

  unsigned int i;
  unsigned int j;

  for (i = 0; i < p_num_adaptors; i++) {
    /*
      printf(" name:        %s\n"
      " type:        %s%s%s%s%s\n"
      " ports:       %ld\n"
      " first port:  %ld\n",
      ai[i].name,
      (ai[i].type & XvInputMask)	? "input | "	: "",
      (ai[i].type & XvOutputMask)	? "output | "	: "",
      (ai[i].type & XvVideoMask)	? "video | "	: "",
      (ai[i].type & XvStillMask)	? "still | "	: "",
      (ai[i].type & XvImageMask)	? "image | "	: "",
      ai[i].num_ports,
      ai[i].base_id);
    */
    xv_port = ai[i].base_id;
    
    //printf("adaptor %d ; format list:\n", i);
    for (j = 0; j < ai[i].num_formats; j++) {
      /*
	printf(" depth=%d, visual=%ld\n",
	ai[i].formats[j].depth,
	ai[i].formats[j].visual_id);
      */
    }
    unsigned int p;
    unsigned int encodings;
    int attributes;
    int formats;

    for (p = ai[i].base_id; p < ai[i].base_id+ai[i].num_ports; p++) {
      
      //printf(" encoding list for port %d\n", p);
      if (XvQueryEncodings(xWindow->display, p, &encodings, &ei) != Success) {
	//printf("XvQueryEncodings failed.\n");
	continue;
      }
      for (j = 0; j < encodings; j++) {
	/*
	printf("  id=%ld, name=%s, size=%ldx%ld, numerator=%d, denominator=%d\n",
	       ei[j].encoding_id, ei[j].name, ei[j].width, ei[j].height,
	       ei[j].rate.numerator, ei[j].rate.denominator);
	*/
      }
      XvFreeEncodingInfo(ei);
      int k;
      //printf(" attribute list for port %d\n", p);
      at = XvQueryPortAttributes(xWindow->display, p, &attributes);
      for (k = 0; k < attributes; k++) {
	/*
	printf("  name:       %s\n"
	       "  flags:     %s%s\n"
	       "  min_color:  %i\n"
	       "  max_color:  %i\n",
	       at[k].name,
	       (at[k].flags & XvGettable) ? " get" : "",
	       (at[k].flags & XvSettable) ? " set" : "",						
	       at[k].min_value, at[k].max_value);
	*/
      }
      if (at)
	XFree(at);
      
      //printf(" image format list for port %d\n", p);
      fo = XvListImageFormats(xWindow->display, p, &formats);
      for (k = 0; k < formats; k++) {
	/*
	printf("  0x%x (%4.4s) %s\n",
	       fo[k].id,
	       (char *)&fo[k].id,
	       (fo[k].format == XvPacked) ? "packed" : "planar");
	*/
      }
      if (fo)
	XFree(fo);
    }
    printf("\n");
  }
  if (p_num_adaptors > 0)
    XvFreeAdaptorInfo(ai);
  if (xv_port == -1) {
    return false;
  }
#endif
  return true;
  
}


void ImageXVDesk::freeImage() {
#ifdef X11_XV
  if (xWindow == NULL) {
    return;
  }
  if (yuv_shminfo.shmid >=0) {
    XShmDetach(xWindow->display,&yuv_shminfo);
    if(yuv_shminfo.shmaddr) {
      shmdt(yuv_shminfo.shmaddr);
      XFree(yuv_image);
      yuv_shminfo.shmaddr=NULL;
    }
    XSync(xWindow->display, False);
    yuv_shminfo.shmid=-1;
  }
#endif 
}


void ImageXVDesk::createImage(int id) {
#ifdef X11_XV
  if (xWindow == NULL) {
    cout << "ImageXVDesk::freeImage - you have to call init before creating an image!" << endl;
    return;
  }
  
  yuv_image = XvShmCreateImage(xWindow->display, xv_port, 
			       id, 0, 
			       xWindow->width,
			       xWindow->height, &yuv_shminfo);
  
  yuv_shminfo.shmid = shmget(IPC_PRIVATE, 
			     yuv_image->data_size, IPC_CREAT | 0777);
  yuv_shminfo.shmaddr = yuv_image->data = 
    (char*)shmat(yuv_shminfo.shmid, 0, 0);
  yuv_shminfo.readOnly = False;

  if (!XShmAttach(xWindow->display, &yuv_shminfo)) {
    printf("XShmAttach failed !\n");
    lSupport=false;
    return;
  }
  shmctl(yuv_shminfo.shmid, IPC_RMID, 0);
#endif  
}
