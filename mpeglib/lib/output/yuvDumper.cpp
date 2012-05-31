/*
  writes yuv images on HD
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "yuvDumper.h"

YUVDumper::YUVDumper() {
  picCnt=1;
  method=_DUMP_YUV_AS_STREAM;
}

YUVDumper::~YUVDumper() {
}


int YUVDumper::openWindow(int w, int h,const char*) {
  
  FILE* formatFile=fopen("stream.yuv.format","w+");
  fprintf(formatFile,"Version 0.1\nw:%dh:%d\n",w,h);
  fclose(formatFile);



  if (method == _DUMP_YUV_AS_STREAM) {
    FILE* outFile=fopen("stream.yuv","w+");
    fclose(outFile);
  }
 return true;
}

void YUVDumper::closeWindow() {
}


void YUVDumper::flushWindow() {
}


int YUVDumper::getMethod() {
  return method;
}


void YUVDumper::setMethod(int method) {
  this->method=method;
}

void YUVDumper::unlockPictureArray(PictureArray* pictureArray) {
  YUVPicture* pic=pictureArray->getYUVPictureCallback();
  if (pic == NULL) {
    return;
  }

  FILE* outFile=NULL;

  if (method == _DUMP_YUV_AS_STREAM) {
    outFile=fopen("stream.yuv","a+");
  }

  if (outFile == NULL) {
      perror("fopen");
      return;
  }

  int lumSize=pic->getLumLength();
  int colorSize=pic->getColorLength();
  fwrite(pic->getLuminancePtr(),1,lumSize,outFile);
  fwrite(pic->getCrPtr(),1,colorSize,outFile);
  fwrite(pic->getCbPtr(),1,colorSize,outFile);
  fclose(outFile);

}
