/*
  here are the different initialisation routines for different displays
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "initDisplay.h"

#include <iostream>

using namespace std;



static unsigned long wpixel[256];




/*
 *--------------------------------------------------------------
 *
 * InitColorDisplay --
 *
 *	Initialized display for full color output.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */

void initColorDisplay(XWindow* xwindow) {
  XWindowAttributes winattr;


  XGetWindowAttributes(xwindow->display, xwindow->window, &winattr);

  xwindow->redMask = winattr.visual->red_mask;
  xwindow->greenMask = winattr.visual->green_mask;
  xwindow->blueMask = winattr.visual->blue_mask;
}







/*
 *--------------------------------------------------------------
 *
 * FindFullColorVisual
 *
 *  Returns a pointer to a full color bit visual on the display
 *
 * Results:
 *      See above.
 *  
 * Side effects:
 *      Unknown.
 *
 *--------------------------------------------------------------
 */
Visual* FindFullColorVisual (Display* dpy,int*  depth) {
  XVisualInfo vinfo;
  XVisualInfo *vinfo_ret;
  int numitems, maxdepth;

#if defined(__cplusplus) || defined(c_plusplus)
  vinfo.c_class = TrueColor;
#else
  vinfo.class = TrueColor;
#endif
  
  vinfo_ret = XGetVisualInfo(dpy, VisualClassMask, &vinfo, &numitems);
  
  if (numitems == 0) return NULL;

  maxdepth = 0;
  while(numitems > 0) {
    if (vinfo_ret[numitems-1].depth > maxdepth) {
      maxdepth = vinfo_ret[numitems-1 ].depth;
    }
    numitems--;
  }
  XFree((void *) vinfo_ret);

  if (maxdepth < 16) return NULL;

  if (XMatchVisualInfo(dpy, DefaultScreen(dpy), maxdepth, 
		       TrueColor, &vinfo)) {
    *depth = maxdepth;
    return vinfo.visual;
  }
  
  return NULL;
}


/*
 *--------------------------------------------------------------
 *
 * CreateFullColorWindow
 *
 *  Creates a window capable of handling 32 bit color.
 *
 * Results:
 *      See above.
 *  
 * Side effects:
 *      Unknown.
 *
 *--------------------------------------------------------------
 */
void CreateFullColorWindow (XWindow* xwindow) {
  int depth; 
  Visual *visual;
  XSetWindowAttributes xswa;
  unsigned long mask;
  unsigned int c_class;
  int screen;
  Display *dpy=xwindow->display;
  /*
  int x = xinfo->hints.x,
      y = xinfo->hints.y;
  unsigned int w = xinfo->hints.width,
               h = xinfo->hints.height;
  */
  screen = XDefaultScreen(dpy);
  c_class = InputOutput;	/* Could be InputOnly */
  if (xwindow->visual == NULL) {
    xwindow->visual = visual = FindFullColorVisual (dpy, &depth);
    xwindow->depth = depth;
  } else {
     visual=xwindow->visual;
     depth=xwindow->depth;
  }

  if (visual == NULL) {
    cout << "visual is null"<<endl;
    return;
  }
  mask = CWBackPixel | CWColormap | CWBorderPixel;
  if (xwindow->colormap==0) {
    xswa.colormap = XCreateColormap(dpy,
				    XRootWindow(dpy, screen),
				    visual, AllocNone);
  } else xswa.colormap = xwindow->colormap;
  xswa.background_pixel = BlackPixel(dpy, DefaultScreen(dpy));
  xswa.border_pixel = WhitePixel(dpy, DefaultScreen(dpy));
  XSetWindowColormap(xwindow->display,xwindow->window,xwindow->colormap);


  /*
  xwindow->window = XCreateWindow(dpy, RootWindow(dpy, screen), x, y, w, h,
				  (unsigned int) 1, depth, c_class, 
				  visual, mask, &xswa);
  */
}







/*
 *--------------------------------------------------------------
 *
 * InitSimpleDisplay --
 *
 *      Initialized display, sets up colormap, etc. Use for 8 Bit color
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */

void initSimpleDisplay(XWindow* xwindow) {
  int ncolors = LUM_RANGE*CB_RANGE*CR_RANGE;
  XColor xcolor;
  int i, lum_num, cr_num, cb_num;
  unsigned char r, g, b;
  Colormap dcmap;
  Display *display;
  ColorTable8Bit colorTable8Bit;

  display = xwindow->display;

    
  xwindow->colormap = XDefaultColormap(display, DefaultScreen(display));
  dcmap = xwindow->colormap;
    
  xcolor.flags = DoRed | DoGreen | DoBlue;
    
    
  //if (xinfo->owncmFlag) goto create_map;

retry_alloc_colors:
  for (i=0; i<ncolors; i++) {

    lum_num = (i / (CR_RANGE*CB_RANGE))%LUM_RANGE;
    cr_num = (i / CB_RANGE)%CR_RANGE;
    cb_num = i % CB_RANGE;

    colorTable8Bit.ConvertColor(lum_num, cr_num, cb_num, &r, &g, &b);

    xcolor.red = r * 256;
    xcolor.green = g * 256;
    xcolor.blue = b * 256;

   if ((XAllocColor(display,xwindow->colormap , &xcolor) == 0 
        && xwindow->colormap == dcmap)) {
      int j;
      unsigned long tmp_pixel;
      XWindowAttributes xwa;

      // Free colors. 
      for (j = 0; j < i; j ++) {
        tmp_pixel = wpixel[j];
        XFreeColors(display,xwindow->colormap , &tmp_pixel, 1, 0);
      }


      //create_map:
      XGetWindowAttributes(display, xwindow->window, &xwa);
      xwindow->colormap = XCreateColormap(display, xwindow->window,
                                          xwa.visual, AllocNone);
      XSetWindowColormap(display, xwindow->window,xwindow->colormap );

      goto retry_alloc_colors;
    }
    xwindow->pixel[i]=xcolor.pixel;
    wpixel[i] = xcolor.pixel;
  }

}


