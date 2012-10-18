/*
  a thread safe ring buffer without dependencies
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#include "simpleRingBuffer.h"
#include <string.h>

#include <iostream>

using namespace std;

static int instanceCnt=0;

SimpleRingBuffer::SimpleRingBuffer(int size,int minLinSize) {
  abs_thread_mutex_init(&mut);
  abs_thread_cond_init(&dataCond);
  abs_thread_cond_init(&spaceCond);

  instance=instanceCnt;
  instanceCnt++;
  this->size=size;
  startPos=(char*)malloc(size);
  readPos=startPos;
  writePos=startPos;
  lockPos=startPos;

  lockgrade=0;
  fillgrade=0;
  linAvail=size;
  lastPos=(startPos+size-1);
  eofPos=lastPos+1;

  canWrite=size;
  canRead=0;

  minLinBuf=new char[minLinSize];
  this->minLinBufSize=minLinSize;
  waitMinData=0;
  waitMinSpace=0;
  lWaitForData=false;
  lWaitForSpace=false;

  readBytes=0;
  writeBytes=0;

  lCanWaitForData=true;
  lCanWaitForSpace=true;
}


SimpleRingBuffer::~SimpleRingBuffer() {
  // The user of this class must take care that the threads
  // have exited!
  free(startPos);
  delete [] minLinBuf;
  abs_thread_mutex_destroy(&mut);
  abs_thread_cond_destroy(&dataCond);
  abs_thread_cond_destroy(&spaceCond);
}
  

int SimpleRingBuffer::getSize() {
  return size;
}


int SimpleRingBuffer::getWriteArea(char* &ptr,int &size) {

  // When we are in this area the following
  // can happen by the other thread
  // fillgrade is decreased (because reader fowards)
  // readPos is changed.

  ptr=writePos;
  size=canWrite;

  return size;
}


void SimpleRingBuffer::updateCanWrite() {
  if (lockPos < writePos) {
    canWrite=eofPos-writePos;
    //printf("1 c:%d l:%p w:%p",canWrite,lockPos,writePos);
  } else if (lockPos > writePos) {
    canWrite=lockPos-writePos;
    //printf("2 c:%d l:%p w:%p",canWrite,lockPos,writePos);
  } else {
    if (fillgrade > 0) {
      canWrite=0;
    } else {
      canWrite=eofPos-writePos;
    }
    //printf("2 c:%d ",canWrite);
    
  }
  if (canWrite < 0) {
    printf("error canWrite:%d fill:%d lock:%p start:%p eof:%p write:%p\n",
           canWrite,fillgrade,lockPos,startPos,eofPos,writePos);
  }

} 


void SimpleRingBuffer::updateCanRead() {
  canRead=fillgrade-lockgrade;
  int currentSpace=size-fillgrade;
  if (currentSpace >= waitMinSpace) {
    abs_thread_cond_signal(&spaceCond);
  } 
  if (canRead < 0) {
    printf("error canRead:%d fillgrade:%d lockgrade:%d \n",
           canRead,fillgrade,lockgrade);
  }
 
} 


void SimpleRingBuffer::forwardLockPtr(int nBytes) {
  abs_thread_mutex_lock(&mut);

  if (fillgrade < lockgrade) {
    printf("1:fillgrade:%d < lockgrade:%d\n",fillgrade,lockgrade);
  }
  fillgrade-=nBytes;
  lockgrade-=nBytes;
  if (fillgrade < lockgrade) {
    printf("2:fillgrade:%d < lockgrade:%d nBytes:%d\n",
	   fillgrade,lockgrade,nBytes);
  }
  lockPos=lockPos+nBytes;
  if (lockPos > lastPos) {   // we expects that we had a linAvail part
    // if user forwards more than buffer boundary
    nBytes=lockPos-lastPos;
    lockPos=startPos+nBytes-1;
  }
  updateCanWrite();
  updateCanRead();

  abs_thread_mutex_unlock(&mut);
  return;
}


void SimpleRingBuffer::forwardWritePtr(int nBytes) {
  abs_thread_mutex_lock(&mut);
  
  fillgrade=fillgrade+nBytes;
  if (fillgrade < lockgrade) {
    printf("3:fillgrade:%d < lockgrade:%d nBytes:%d\n",
	   fillgrade,lockgrade,nBytes);
  }
  writeBytes+=nBytes;
  writePos=writePos+nBytes;
  if(writePos >= eofPos) {
    if (writePos == eofPos) {
      writePos=startPos;
    } else {
      cout << "writePos > eofPos ! forward error:"<<(eofPos-writePos)
	   <<" bytes"<<endl;
    }
  }

  updateCanWrite();
  updateCanRead();
  if (fillgrade >= waitMinData) {
    abs_thread_cond_signal(&dataCond);
  }
  abs_thread_mutex_unlock(&mut);
}


int SimpleRingBuffer::waitForSpace(int bytes){
  abs_thread_mutex_lock(&mut);
  int back=0;
  waitMinSpace=bytes;
  if (waitMinSpace > size) {
    waitMinSpace=size;
  }
  if (waitMinSpace < 0) {
    cout << "negative waitForSpace"<<endl;
    waitMinSpace=0;
  }
  int currentSpace=size-fillgrade;
  if (lCanWaitForSpace) {
    if (currentSpace < waitMinSpace) {
      lWaitForSpace=true;
      // it is not possible to wait for data/space simultanously
      if (lWaitForData == true) {
	abs_thread_cond_signal(&dataCond);
      }
      abs_thread_cond_wait(&spaceCond,&mut);
      lWaitForSpace=false;
    }
  }
  if (size-fillgrade >= waitMinSpace) {
    back=1;
  }
  abs_thread_mutex_unlock(&mut);
  return back;
}


void SimpleRingBuffer::exitWaitForSpace(){
  abs_thread_mutex_lock(&mut);
  abs_thread_cond_signal(&spaceCond);
  abs_thread_mutex_unlock(&mut);
}

void SimpleRingBuffer::setCanWaitForSpace(int lCanWaitForSpace) {
  abs_thread_mutex_lock(&mut);
  this->lCanWaitForSpace=lCanWaitForSpace;
  abs_thread_cond_signal(&spaceCond);
  abs_thread_mutex_unlock(&mut);

}
  
  

void SimpleRingBuffer::forwardReadPtr(int nBytes) {
  abs_thread_mutex_lock(&mut);
  readBytes+=nBytes;
  readPos+=nBytes;
  linAvail=linAvail-nBytes;
  lockgrade+=nBytes;
  if (readPos > lastPos) {   // we expects that we had a linAvail part
    // if user forwards more than buffer boundary
    nBytes=readPos-lastPos;
    readPos=startPos+nBytes-1;
    linAvail=lastPos+1-readPos;
  }
  if (fillgrade < lockgrade) {
    printf("5:fillgrade:%d < lockgrade:%d nBytes:%d\n",
	   fillgrade,lockgrade,nBytes);
  }
  updateCanRead();
  abs_thread_mutex_unlock(&mut);
}



int SimpleRingBuffer::getReadArea(char* &ptr,int &readSize) {
  int pSize=readSize;
  ptr=readPos;

  if (canRead == 0) {
    readSize=0;
    return 0;
  }
  if (pSize < 0) { 
    cout << "Generic Memory Info invalid"<<endl;
    pSize=size/2;
  }
  //
  // Now the part the we deliver a minimum buffer if it is
  // possible
  //

  if ( (pSize > linAvail) &&
       (minLinBufSize >linAvail) &&
       (canRead > linAvail) ) {
    int copySize;
    copySize=canRead;  // we cannot copy more than this
    if (copySize > pSize) { // if it is too much reduche it
      copySize=pSize;
    }
    if (copySize > minLinBufSize) { // if it does not fit in buffer->reduce
      copySize=minLinBufSize;
    }
    memcpy(minLinBuf,readPos,linAvail);
    memcpy(minLinBuf+linAvail,startPos,copySize-linAvail);
    readSize=copySize;
    ptr=minLinBuf;
    return copySize;
  }

  // linAvail part end

  int copyBytes=linAvail;
  if (canRead < copyBytes) {
    copyBytes=canRead;
  }
  if (copyBytes >= pSize) {
    readSize=pSize;
  } else {
    readSize=copyBytes;
  }
  return readSize;
}



void SimpleRingBuffer::exitWaitForData(){
  abs_thread_mutex_lock(&mut);
  abs_thread_cond_signal(&dataCond);
  abs_thread_mutex_unlock(&mut);
}



int SimpleRingBuffer::waitForData(int bytes){
  abs_thread_mutex_lock(&mut);
  int back=0;
  waitMinData=bytes;
  if (waitMinData > size) {
    waitMinData=size;
  }
  if (waitMinData < 0) {
    cout << "negative waitForData"<<endl;
    waitMinData=0;
  }
  if (lCanWaitForData) {
    if (fillgrade < waitMinData) {
      lWaitForData=true;
      // it is not possible to wait for data space simultanously
      if (lWaitForSpace == true) {
	abs_thread_cond_signal(&spaceCond);
      }
      abs_thread_cond_wait(&dataCond,&mut);
      lWaitForData=false;
    }
  }
  if (fillgrade >= waitMinData) {
    back=1;
  }
  abs_thread_mutex_unlock(&mut);
  return back;
}

int SimpleRingBuffer::getCanWaitForData() {
  return lCanWaitForData;
}
  

void SimpleRingBuffer::setCanWaitForData(int lCanWaitForData) {
  abs_thread_mutex_lock(&mut);
  this->lCanWaitForData=lCanWaitForData;
  abs_thread_cond_signal(&dataCond);
  abs_thread_mutex_unlock(&mut);

}

void SimpleRingBuffer::emptyBuffer() {
  abs_thread_mutex_lock(&mut);
  writePos=readPos;
  if (fillgrade < lockgrade) {
    printf("4:fillgrade:%d < lockgrade:%d\n",fillgrade,lockgrade);
  }
  linAvail=lastPos+1-writePos;
  fillgrade=lockgrade;
  updateCanRead();
  updateCanWrite();
  readBytes=0;
  writeBytes=0;
  if (size-fillgrade >= waitMinSpace) {
    abs_thread_cond_signal(&spaceCond);
  }
  if (fillgrade >= waitMinData) {
    abs_thread_cond_signal(&dataCond);
  }
  abs_thread_mutex_unlock(&mut);
}


int SimpleRingBuffer::getFillgrade() {
  return fillgrade;
}


int SimpleRingBuffer::getReadBytes() {
  return readBytes;
}


int SimpleRingBuffer::getWriteBytes() {
  return writeBytes;
}


int SimpleRingBuffer::getFreeRead() {
  return fillgrade;
}

int SimpleRingBuffer::getFreeWrite() {
  return size-fillgrade;
}

void SimpleRingBuffer::resizeBuffer(int changeSize) {
  abs_thread_mutex_lock(&mut);
  int lPos=lockPos-startPos;
  int wPos=writePos-startPos;
  int rPos=readPos-startPos;
  startPos=(char *)realloc(startPos,changeSize);
  size=changeSize;
  readPos=startPos+lPos;
  writePos=startPos+wPos;
  lockPos=startPos+rPos;

  lastPos=(startPos+size-1);
  eofPos=lastPos+1;

  linAvail=lastPos+1-readPos;

  updateCanWrite();
  updateCanRead();
  abs_thread_mutex_unlock(&mut);


}
