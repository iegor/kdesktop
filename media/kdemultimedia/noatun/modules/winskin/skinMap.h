/*
  generic type for describing skins.
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef _SKINMAP_H
#define _SKINMAP_H

#include <qpixmap.h>

typedef struct {
    int x;
    int y;
    int width;
    int height;
} SkinMap;

typedef struct {
    int fileId;
    int x;
    int y;
    int width;
    int height;
} SkinDesc;



#endif
