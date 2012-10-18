/*
  wapper for other architectures than mmx
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "mmxidct.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

		
#ifndef INTEL
 void IDCT_mmx(short int* reconptr) {
  printf("urgs mmxidct!\n");
  exit(0);
 }
	
#endif
