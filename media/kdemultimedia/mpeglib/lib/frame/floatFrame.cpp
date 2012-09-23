/*
  stores frames as floats.
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "floatFrame.h"

#include <iostream>

using namespace std;

FloatFrame::FloatFrame(int size) {
  data=new float[size];
  len=0;
  this->size=size;
  this->sampleSize=sizeof(float)*8;
  this->lSigned=true;
  this->lBigEndian=AUDIOFRAME_BIGENDIAN;
  setFrameType(_FRAME_AUDIO_FLOAT);
}


FloatFrame::~FloatFrame() {
  delete [] data;
}


void FloatFrame::putFloatData(float* in,int lenCopy) {
  if ((len+lenCopy) > size) {
    cout << "cannot copy putFloatData. Does not fit"<<endl;
    exit(0);
  }
  memcpy(data+len,in,lenCopy*sizeof(float));
  len+=lenCopy;


}


void FloatFrame::putFloatData(float* left,float* right,int len) {
  cout << "not yet implemented"<<endl;
}
