/*
  include header for dcts/windows
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __COMMON_H
#define __COMMON_H

#include "attribute.h"
#include <stdio.h>

#define SSLIMIT      18
#define SBLIMIT      32


#define LS 0
#define RS 1


typedef float REAL;

// The inline code works on intel only with egcs >= 1.1
#ifdef __GNUC__
#if (__GNUC__ < 2 || ( __GNUC__ == 2 && __GNUC_MINOR__ < 91 ) )
#ifndef _AIX
#warning "inline code disabled! (buggy egcs version)"
#undef __NO_MATH_INLINES
#define __NO_MATH_INLINES 1
#endif
#endif
#endif
#include <math.h>
#include <stdlib.h>

#ifndef M_PI
#define MY_PI 3.14159265358979323846
#else
#define MY_PI M_PI
#endif

#ifdef PI
#undef PI
#endif
#define PI     MY_PI
#define PI_12  (PI/12.0)
#define PI_18  (PI/18.0)
#define PI_24  (PI/24.0)
#define PI_36  (PI/36.0)
#define PI_72  (PI/72.0)


#endif
