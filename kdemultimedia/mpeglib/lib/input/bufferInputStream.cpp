/*
  reads input data
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "bufferInputStream.h"
#include "simpleRingBuffer.h"


BufferInputStream::BufferInputStream(int size,int minlinSize,
				     const char* name) {
  ringBuffer=new SimpleRingBuffer(size,minlinSize);
  leof=false;
  bytePos=0;
  fillgrade=0;
  setUrl(name);
  lLock=false;
  abs_thread_mutex_init(&writeInMut);
}


BufferInputStream::~BufferInputStream() {
  delete ringBuffer;
  abs_thread_mutex_destroy(&writeInMut);
}


int BufferInputStream::open(const char*) {
  leof=false;
  setBlocking(true);
  return true;
}

void BufferInputStream::close() {
  leof=true;
  setBlocking(false);
}

int BufferInputStream::eof() {
  return (leof && (fillgrade==0));
}

int BufferInputStream::isOpen() {
  return !leof;
}

void BufferInputStream::setBlocking(int lblock) {
  ringBuffer->setCanWaitForSpace(lblock);
  ringBuffer->setCanWaitForData(lblock);
}

int BufferInputStream::getHold() {
  return lLock;
}


void BufferInputStream::setHold(int lLock) {
  if (lLock) {
    lockBuffer();
  } else {
    unlockBuffer();
  }
  this->lLock=lLock;
}

int BufferInputStream::read(char* ptr,int size) {
  int i=0;
  int n=size;
  int canCopy=n;
  char* readPtr;
  // here we read even if leof is true
  // we make sure that we empty the whole buffer!
  while((eof()==false) && (n > 0)) {
    canCopy=n;
    ringBuffer->getReadArea(readPtr,canCopy);
    if (canCopy <= 0){
      ringBuffer->waitForData(1);
      continue;
    }
    if (n < canCopy) {
      canCopy=n;
    }
    memcpy((char*)ptr+i,readPtr,canCopy);
    i=i+canCopy;
    n=n-canCopy;
    ringBuffer->forwardReadPtr(canCopy);
    ringBuffer->forwardLockPtr(canCopy);
    lockBuffer();
    bytePos+=canCopy;
    fillgrade-=canCopy;
    unlockBuffer();
  }
  return i;
}


int BufferInputStream::write(char* ptr,int len,TimeStamp* stamp) {
  int i=0;
  int n=len;
  int canWrite=n;
  char* writePtr;

  if (stamp) {
    lockBuffer();
    long key;
    key=bytePos+fillgrade;
    InputStream::insertTimeStamp(stamp,key,len);
    unlockBuffer();
  }
  // if eof is set we do not insert any more data
  // we do not call eof() !!!
  while((leof==false) && (n > 0)) {
    canWrite=n;
    ringBuffer->getWriteArea(writePtr,canWrite);
    if (canWrite <= 0){
      ringBuffer->waitForSpace(1);
      continue;
    }
    if (canWrite > n) {
      canWrite=n;
    }
    memcpy(writePtr,(char*)ptr+i,canWrite);
    i=i+canWrite;
    n=n-canWrite;
    ringBuffer->forwardWritePtr(canWrite);
    lockBuffer();
    fillgrade+=canWrite;
    unlockBuffer();
  }

  return i;
}



int BufferInputStream::write(InputStream* input,int len,TimeStamp* stamp) {
  int i=0;
  int n=len;
  int canWrite=n;
  int didWrite;
  char* writePtr;

  if (stamp) {
    lockBuffer();
    long key;
    key=bytePos+fillgrade;
    InputStream::insertTimeStamp(stamp,key,len);
    unlockBuffer();
  }
  // if eof is set we do not insert any more data
  // we do not call eof() !!!
  while((leof==false) && (n > 0)) {
    canWrite=n;
    ringBuffer->getWriteArea(writePtr,canWrite);
    if (canWrite <= 0){
      ringBuffer->waitForSpace(1);
      continue;
    }
    if (canWrite > n) {
      canWrite=n;
    }
    didWrite=input->read(writePtr,canWrite);
    if (input->eof()) break;
    i=i+didWrite;
    n=n-didWrite;
    ringBuffer->forwardWritePtr(didWrite);
    lockBuffer();
    fillgrade+=canWrite;
    unlockBuffer();
  }

  return i;
}



long BufferInputStream::getByteLength() {
  return ringBuffer->getFillgrade();
}

int BufferInputStream::getFillgrade() {
  return ringBuffer->getFillgrade();
}


int BufferInputStream::getFreeRead() {
  return ringBuffer->getFreeRead();
}


int BufferInputStream::getFreeSpace() {
  return ringBuffer->getFreeWrite();
}



long BufferInputStream::getBytePosition() {
  return bytePos;
}

void BufferInputStream::setBytePosition(long bytePos) {
  this->bytePos=bytePos;
}


int BufferInputStream::seek(long) {
  return false;
}


void BufferInputStream::clear() {

  ringBuffer->emptyBuffer();
  ringBuffer->exitWaitForData();
  ringBuffer->exitWaitForSpace();
  timeStampArray->clear();
  
  lockBuffer();
  bytePos=0;
  fillgrade=0;
  unlockBuffer();
 
}
 



// remote read extension
int BufferInputStream::readRemote(char** ptr,int size) {
  int n=0;
  char* readPtr;
  while((eof()==false)) {
    n=size;
    ringBuffer->getReadArea(readPtr,n);
    if (n < size){
      ringBuffer->waitForData(size);
      if (ringBuffer->getCanWaitForData()==false) {
	break;
      }
      continue;
    }
    break;
  }
  *ptr=readPtr;
  return n;
}


void BufferInputStream::forwardReadPtr(int bytes) {

  ringBuffer->forwardReadPtr(bytes);
  ringBuffer->forwardLockPtr(bytes);
  lockBuffer();
  bytePos+=bytes;
  fillgrade-=bytes;
  unlockBuffer();
  getTimeStamp(bytePos);
}
 

void BufferInputStream::setCanWaitForData(int lBlock) {
  ringBuffer->setCanWaitForData(lBlock);
}


void BufferInputStream::lockBuffer() {
  abs_thread_mutex_lock(&writeInMut);
}


void BufferInputStream::unlockBuffer() {
  abs_thread_mutex_unlock(&writeInMut); 
}


int BufferInputStream::getSize() {
  return ringBuffer->getSize();
}
 

