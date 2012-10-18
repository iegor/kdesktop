/*
  generic input class
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "inputStream.h"
#include "../util/mmx/mmx.h"

#include <iostream>

using namespace std;


InputStream::InputStream() {
  timeStampArray=new TimeStampArray((char*)"Input",3000);
  urlBuffer=new DynBuffer(20);
  // we call mm_support() here because it is the only position
  // where we gurantee that not threads are
  // running (the call is not thread safe)
  // afer the call we never execute the asm part again
  // and everything is fine
  mm_support();
}



InputStream::~InputStream() {
  delete timeStampArray;
  delete urlBuffer;
  
}


int InputStream::open(const char* dest) {
  cout << "direct virtual call InputStream::open:"<<dest<<endl;
  return false;
}


void InputStream::close() {
  cout << "direct virtual call InputStream::close"<<endl;
  exit(0); 
}


int InputStream::isOpen() {
  cout << "direct virtual call InputStream::isOpen"<<endl;
  exit(0);
  return false; 
}


int InputStream::eof() {
  cout << "direct virtual call InputStream::eof"<<endl;
  exit(0);
  return true;  
}


int InputStream::read(char* ,int ) {
  cout << "direct virtual call InputStream::read"<<endl;
  exit(0);
  return 0;  
}


int InputStream::seek(long bytePos) {
  cout << "direct virtual call InputStream::seek:"<<bytePos<<endl;
  exit(0);
  return false;  
}

void InputStream::clear() {
  cout << "direct virtual call InputStream::clear:"<<endl;
  exit(0);
}


long InputStream::getByteLength() {
  cout << "direct virtual call InputStream::getByteLength"<<endl;
  return 0;
}


long InputStream::getBytePosition() {
  cout << "direct virtual call InputStream::getBytePosition"<<endl;
  return 0;
}


void InputStream::insertTimeStamp(TimeStamp* src,long key,int len) {
  timeStampArray->insertTimeStamp(src,key,len);
}

TimeStamp* InputStream::getTimeStamp(long key) {
  return timeStampArray->getTimeStamp(key);
}


int InputStream::bytesUntilNext(long key) {
  return timeStampArray->bytesUntilNext(key);
}

void InputStream::print() {
  cout << "direct virtual call InputStream::print"<<endl;
}





char* InputStream::getUrl() {
  return urlBuffer->getData();
}


void InputStream::setUrl(const char* url) {
  urlBuffer->clear();
  if (url != NULL) {
    urlBuffer->append(url);
  }
}





