/*
  initializer/resyncer/frame detection etc.. for mpeg audio
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "mpegAudioStream.h"

#include <stdlib.h>

MpegAudioStream::MpegAudioStream() {
  buffer=NULL;
}


MpegAudioStream::~MpegAudioStream() {
}



void MpegAudioStream::setFrame(unsigned char* ptr,int len) {
  this->buffer=(char*)ptr;
  this->len=len;
  bitindex=0;
}










