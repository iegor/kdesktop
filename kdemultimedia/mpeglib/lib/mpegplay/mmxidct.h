/*
  wapper for other architectures than mmx
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __MMXIDCT_H
#define __MMXIDCT_H

#include <stdio.h>
#include <stdlib.h>

extern "C" void IDCT_mmx(short int* reconptr);

#endif
