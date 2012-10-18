/*
  thread safe wrapper for input Stream
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "threadSafeInputStream.h"


ThreadSafeInputStream::ThreadSafeInputStream(InputStream* input) {
  threadQueue=new ThreadQueue();
  this->input=input;
}


ThreadSafeInputStream::~ThreadSafeInputStream() {
  delete threadQueue;
  delete input;
}



int ThreadSafeInputStream::open(const char* dest) {
  int back;
  threadQueue->waitForExclusiveAccess();
  back=input->open(dest);
  threadQueue->releaseExclusiveAccess();
  return back;
}


void ThreadSafeInputStream::close() {
  threadQueue->waitForExclusiveAccess();
  input->close();
  threadQueue->releaseExclusiveAccess();
}


int ThreadSafeInputStream::isOpen() {
  int back;
  threadQueue->waitForExclusiveAccess();
  back=input->isOpen();
  threadQueue->releaseExclusiveAccess();
  return back;
}


int ThreadSafeInputStream::eof() {
  int back;
  threadQueue->waitForExclusiveAccess();
  back=input->eof();
  threadQueue->releaseExclusiveAccess();
  return back;  
}


int ThreadSafeInputStream::read(char* buf,int len) {
  int back;
  threadQueue->waitForExclusiveAccess();
  back=input->read(buf,len);
  threadQueue->releaseExclusiveAccess();
  return back;  
}


int ThreadSafeInputStream::seek(long bytePos) {
  int back;
  threadQueue->waitForExclusiveAccess();
  back=input->seek(bytePos);
  threadQueue->releaseExclusiveAccess();
  return back;   
}

void ThreadSafeInputStream::clear() {
  threadQueue->waitForExclusiveAccess();
  input->clear();
  threadQueue->releaseExclusiveAccess();
}


long ThreadSafeInputStream::getByteLength() {
  long back;
  threadQueue->waitForExclusiveAccess();
  back=input->getByteLength();
  threadQueue->releaseExclusiveAccess();
  return back;  
}


long ThreadSafeInputStream::getBytePosition() {
  long back;
  threadQueue->waitForExclusiveAccess();
  back=input->getBytePosition();
  threadQueue->releaseExclusiveAccess();
  return back;  
}


void ThreadSafeInputStream::insertTimeStamp(TimeStamp* src,long key,int len) {
  threadQueue->waitForExclusiveAccess();
  input->insertTimeStamp(src,key,len);
  threadQueue->releaseExclusiveAccess(); 
}


TimeStamp* ThreadSafeInputStream::getTimeStamp(long key) {
  TimeStamp* back;
  threadQueue->waitForExclusiveAccess();
  back=input->getTimeStamp(key);
  threadQueue->releaseExclusiveAccess();
  return back; 
}


int ThreadSafeInputStream::bytesUntilNext(long key) {
  int back;
  threadQueue->waitForExclusiveAccess();
  back=input->bytesUntilNext(key);
  threadQueue->releaseExclusiveAccess();
  return back; 
}

void ThreadSafeInputStream::print() {
  threadQueue->waitForExclusiveAccess();
  input->print();
  threadQueue->releaseExclusiveAccess();
}



