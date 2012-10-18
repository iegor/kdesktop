/*
  writes yuv images on HD
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __YUVDUMPER_H
#define __YUVDUMPER_H

#include "outputStream.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>


#define _DUMP_YUV_AS_STREAM  2


class YUVDumper : public OutputStream {


  int picCnt;
  int method;

 public:
  YUVDumper();
  ~YUVDumper();

  // Video Output

  int openWindow(int width, int height,const char *title);
  void closeWindow();
  void flushWindow();

  void unlockPictureArray(PictureArray* pictureArray);
  int getMethod();
  void setMethod(int method);


};

#endif
