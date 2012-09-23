/*
  here are the different initialisation routines for different displays
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __INITDISPLAY_H
#define __INITDISPLAY_H

#include "math.h"
#include "xinit.h"
#include "../dither/colorTable8Bit.h"



extern void initColorDisplay(XWindow* xwindow);
extern void initSimpleDisplay(XWindow* xwindow);

// helper functions
Visual *FindFullColorVisual (Display *dpy ,int *depth);
void CreateFullColorWindow (XWindow* xwindow);



#endif
