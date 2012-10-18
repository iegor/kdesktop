/*
  xfree 4.0 dga fullscreen mode
  Copyright (C) 2000  Martin Vogt, Christian Gerlach

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "imageDGAFull.h"

#include <iostream>

using namespace std;

ImageDGAFull::ImageDGAFull() {

  m_iMode = -1;
  m_bIsActive = false;
  lSupport=false;
  m_pxWindow = NULL;
  m_iImageMode = _IMAGE_NONE;
  ditherWrapper=NULL;
  supportedModes = _IMAGE_NONE;
  setIdentifier("DGA");
}


ImageDGAFull::~ImageDGAFull() {
  if (ditherWrapper != NULL) {
    delete ditherWrapper;
  }
}

void ImageDGAFull::init(XWindow *xWindow, YUVPicture*)
{
  int uid;

  m_pxWindow = xWindow;
  if (ditherWrapper == NULL) {
    ditherWrapper=new DitherWrapper(xWindow->depth,
				    xWindow->redMask,
				    xWindow->greenMask,
				    xWindow->blueMask,
				    xWindow->pixel);
  }

#ifndef X11_DGA2
  return;
#endif

#ifdef X11_DGA2
  m_pDGAModes=NULL;
  m_iNumberModes = 0;

  m_iVideoWidth = xWindow->width;
  m_iVideoHeight = xWindow->height;
  uid=getuid();
  if (uid != 0) {
    //cout << "you are :"<<uid<<" and not root(0). DGA 2.0 needs root"<<endl;
    return;
  }
  
  if ((m_pDisplay =xWindow->display)==NULL ) {
    fprintf( stderr, " cannot connect to X server %s\n", XDisplayName(NULL));
    return;
  }

  m_iScreen = DefaultScreen(xWindow->display);


  if (!XF86DGAQueryVersion(xWindow->display, 
			   &m_iMajorVersion, &m_iMinorVersion)) { 
    fprintf(stderr, "Unable to query video extension version\n");
    return ;
  }
  printf("DGA version %d.%d detected!\n", m_iMajorVersion, m_iMinorVersion);
  
  // Fail if the extension version in the server is too old
  if (m_iMajorVersion < DGA_MINMAJOR || 
      (m_iMajorVersion == DGA_MINMAJOR && m_iMinorVersion < DGA_MINMINOR)) {
    fprintf(stderr, "Xserver is running an old XFree86-DGA version"
	    " (%d.%d)\n", m_iMajorVersion, m_iMinorVersion);
    fprintf(stderr, "Minimum required version is %d.%d\n",
	    DGA_MINMAJOR, DGA_MINMINOR);
    return ;
  }
  
  if (!XF86DGAQueryExtension(m_pDisplay, &m_iEventBase, &m_iErrorBase)) {
    fprintf(stderr, "Unable to query video extension information\n");
    return ;
  }
  printf("Event base %d\n", m_iEventBase);
  printf("Error base %d\n", m_iErrorBase);

  lSupport=true;
  supportedModes = _IMAGE_FULL;
#endif
}
	
int ImageDGAFull::support() {
  return lSupport;
}


int ImageDGAFull::openImage(int mode) {
#ifdef X11_DGA2
  int width, bank, ram;
  m_bAllowZoom = IS_DOUBLE(mode);
  m_iImageMode = mode;

  /* Open access to the framebuffer */
  if ( ! XDGAOpenFramebuffer(m_pDisplay,m_iScreen) ) {
    return(false);
  }

  findMode(m_pxWindow->width, m_pxWindow->height, m_pxWindow->depth);
  m_pDevice = XDGASetMode(m_pDisplay, m_iScreen, m_pDGAModes[m_iMode].num);
  

  XDGASelectInput(m_pDisplay, m_iScreen,
		  KeyPressMask | ButtonPressMask | PointerMotionMask);

  XF86DGAGetVideo(m_pDisplay,m_iScreen,&m_pAddr,&width,&bank,&ram);


  if(bank < (ram * 1024)) {
    XF86DGASetVidPage(m_pxWindow->display, 
		      DefaultScreen(m_pxWindow->display), 0);
  }

  XF86DGASetViewPort(m_pxWindow->display, 
		     DefaultScreen(m_pxWindow->display),0,0);


  printf("Offset:%8x\n",m_iOffsetScreen);
  m_pStartAddr = m_pAddr + m_iOffsetScreen;
  m_iOffsetLine = (m_iBytesPerLine - m_iBytesPerRow) / m_iBytesPerPixel; 
  cout << "LineOffset:     " << m_iOffsetLine << endl;

  // Clear the screen
  memset(m_pAddr, 0, m_iBytesPerLine * m_iScreenHeight);
  /*  char *pos = m_pStartAddr;
  int end = (m_bZoom) ? 2*m_iVideoHeight : m_iVideoHeight;
  for (int line=0 ; line<end ; line++) {
    memset(pos, 80, m_iBytesPerRow);
    pos += m_iBytesPerRow + m_iOffsetLine * m_iBytesPerPixel;
  }
  sleep(2);*/

  m_bIsActive = true;
#endif
  return true;
}


int ImageDGAFull::closeImage() {
#ifdef X11_DGA2
  m_bIsActive = false;
  stop();

  // delete resources
  if (m_pDGAModes != NULL) {
    delete m_pDGAModes;
    m_pDGAModes=NULL;
  }
#endif
  return true;
}


unsigned char* ImageDGAFull::address() { 
  return (unsigned char*) m_pStartAddr; 
}
  

int  ImageDGAFull::offset() { 
  return m_iOffsetLine;
}


void ImageDGAFull::ditherImage(YUVPicture* pic) {

	int useMode = (m_bZoom) ? m_iImageMode : m_iImageMode & (!_IMAGE_DOUBLE);
	
    ditherWrapper->doDither(pic,m_pxWindow->depth,useMode,
							address(),offset());
}


void ImageDGAFull::putImage() {
  
  if (event())
    closeImage();
}


int ImageDGAFull::findMode(int width, int height, int bpp) {
#ifdef X11_DGA2
  int minBorder = INT_MAX;
  int yBorder=0;
  int border;
  
  // TODO: also check the y-axis
  
  m_iMode = -1;
  m_iNumberModes = 0;
  m_pDGAModes = XDGAQueryModes(m_pDisplay, m_iScreen, &m_iNumberModes);
  printf("Number modes: %d\n", m_iNumberModes);
  
  for (int count=0 ; count<m_iNumberModes ; count++) {

    
    if (m_pDGAModes[count].depth != bpp)
      continue;

    printf("Mode: %d  %dx%d  \t bpp %d\n",
	   count, 
	   m_pDGAModes[count].viewportWidth,
	   m_pDGAModes[count].viewportHeight, 
	   m_pDGAModes[count].bitsPerPixel);
    
    // test normal video
    border = m_pDGAModes[count].viewportWidth - width;
    if ((border >= 0) && (border < minBorder)) {
      minBorder = border;
      m_iMode = count;
      m_bZoom = false;
      yBorder = m_pDGAModes[count].viewportHeight - height;
    }
    
    // test zoomed video
    if (m_bAllowZoom) {
      border = m_pDGAModes[count].viewportWidth - 2 * width;
      if ((border >= 0) && (border < minBorder)) {
  	minBorder = border;
  	m_iMode = count;
  	m_bZoom = true;
  	yBorder = m_pDGAModes[count].viewportHeight-2*height;
      }	
    }
  }
  
  if (m_iMode != -1) {
    m_iScreenWidth = m_pDGAModes[m_iMode].viewportWidth;
    m_iScreenHeight = m_pDGAModes[m_iMode].viewportHeight;
    
    m_iBytesPerPixel = m_pDGAModes[m_iMode].bitsPerPixel / 8;
    m_iBytesPerLine = m_pDGAModes[m_iMode].bytesPerScanline;
    m_iBytesPerRow = width * m_iBytesPerPixel;
    if (m_bZoom) {
      m_iBytesPerRow += m_iBytesPerRow;
    }

    m_iOffsetScreen = minBorder * (m_iBytesPerPixel / 2) + 
      (yBorder / 2) * m_iBytesPerLine;
  }
  
  cout << "Best Mode:      " << m_iMode << endl;
  cout << "Border Size:    " << minBorder / 2 << endl;
  cout << "Zoom:           " << m_bZoom << endl;
  cout << "Bytes per Line: " << m_iBytesPerLine << endl;
  cout << "Bytes per Row:  " << m_iBytesPerRow << endl;
  cout << "Bytes per Pixel:" << m_iBytesPerPixel << endl;
  cout << "Total offset:   " << m_iOffsetScreen << endl;
#endif  
  return (m_iMode != -1); 
}



int ImageDGAFull::event() {
   XEvent event;
   return XCheckTypedEvent(m_pDisplay, ButtonPress + m_iEventBase, &event);
}

void ImageDGAFull::stop() {
#ifdef X11_DGA2
  m_bIsActive = false;
  XF86DGADirectVideo(m_pDisplay, m_iScreen, 0);
  
  XUngrabPointer(m_pDisplay, CurrentTime);
  XUngrabKeyboard(m_pDisplay, CurrentTime);
#endif
}
