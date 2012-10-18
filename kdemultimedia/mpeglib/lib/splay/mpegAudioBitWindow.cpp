/*
  bitwindow class
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "mpegAudioBitWindow.h"


#include <iostream>

using namespace std;

int  MpegAudioBitWindow::getCanReadBits() {
  int p=bitindex>>3;
  int bytes=point - p;
  int bits=bytes*8+(bitindex&7);
  cout << "point:"<<point
       << " p:"<<p
       << " bytes:"<<bytes
       <<" bitindex:"<<bitindex<<" can read:"<<bits<<endl;
  return bits;
}

void MpegAudioBitWindow::wrap(void) {
  int p=bitindex>>3;
  point&=(WINDOWSIZE-1);

  if(p>=point) {
    for(register int i=4;i<point;i++)
      buffer[WINDOWSIZE+i]=buffer[i];
  }
  *((int *)(buffer+WINDOWSIZE))=*((int *)buffer);
}
