/*
  wrapper for MMX calls
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "mmx.h"

static int mmSupport=-1;

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(MMX_SUPPORT) || defined(INTEL)
#define HAVE_ACCEL
#endif


#ifndef HAVE_ACCEL
int emms() {
  printf("urgs! emms() never should happen\n");
  exit(0);
}
  
int mm_support() {
  return 0;
}

#endif


#ifdef HAVE_ACCEL
#include <inttypes.h>
#include "mm_accel.h"




int emms() {
  __asm__  ("emms");
  return 1;
}


int mm_support() {

  int val;

  if (mmSupport == -1) {

    val=mm_accel();
    if (val & MM_ACCEL_X86_MMX) {
      mmSupport=1;
    } else {
      mmSupport=0;
    }
    
  }
  /* Return */
  return(mmSupport);
}


#endif
