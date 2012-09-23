/*
  base class for raw frames (encoded data, where only the type is known)
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "rawFrame.h"

#include <iostream>

using namespace std;

RawFrame::RawFrame(int size) {
  init(_FRAME_RAW_BASE,size);
}


RawFrame::RawFrame(int type,int size) {
  init(type,size);
}


RawFrame::~RawFrame() {
  if (data != NULL) {
    if (lRemoteData==false) {
      delete data;
    }
  }
}


void RawFrame::print(const char* msg) {
  cout << msg<<endl;
  cout << "major Frametype:"<<Frame::getFrameName(getFrameType());
  cout << "size:"<<getSize();
  cout << "len:"<<getLen();

}


void RawFrame::init(int type,int size) {

  if (size < 0) {
    cout << "size <= 0 in RawFrame::RawFrame"<<endl;
    exit(-1);
  }
  setFrameType(type);
  int majorType=getMajorFrameType();
  if (majorType !=  _FRAME_RAW) {
    cout << "invalid Major Frametype:"<<Frame::getFrameName(getFrameType())
	 << " for this class"<<endl;
    printf("ID:0x%x dec:%d majorID:%d\n",type,type,majorType);
    cout << "RawFrame::init"<<endl;
    exit(-1);
  }
  if (size == 0) {
    data=NULL;
    this->size=0;
  }
  if (size > 0) {
    data=new unsigned char[size];
    if (data != NULL) {
      cout <<"malloc error RawFrame"<<endl;
      exit(-1);
    }
    this->size=size;
  }
  setLen(0);
  lRemoteData=false;
}


void RawFrame::setRemoteData(unsigned char* data,int size) {
  if (this->data != NULL) {
    if (lRemoteData==false) {
      delete this->data;
    }
  }
  lRemoteData=true;
  this->data=data;
  this->size=size;
}
