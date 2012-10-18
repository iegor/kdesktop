/*
  a thread safe ring buffer without dependencies
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef _SIMPLERINGBUFFER_H
#define _SIMPLERINGBUFFER_H

#include "../util/abstract/abs_thread.h"

extern "C" {
  #include <stdio.h>
}

/** 
   Note to parameter minLinBufSize in Constructor:
   <p>
   If the fillgrade is sufficient we can delivier at least
   this amount of bytes with one "fill"
   (If the fillgrade is not suffficient we can only deliever fillgrade)
   This values adresses the problem that a ring buffer cannot
   deliever linear memory the whole time(eg. if you read near the
   upper end)
   <p>
   If the requested Buffersize by the device is smaller than
   this number you can be sure that you get exactly
   your preferred buffersize. not more not less.(but
   only if the fillgrade allows this)
*/




class SimpleRingBuffer {


 public:

  SimpleRingBuffer(int ringBufferSize, int minLinBufferSize);
  virtual ~SimpleRingBuffer();

  // Writer thread can call these:

  int getWriteArea(char* &ptr,int &size);
  void forwardWritePtr(int bytes);
  int waitForSpace(int minSpace);
  void exitWaitForSpace();
  void setCanWaitForSpace(int lCanWaitForSpace);
  
  
  // Reader thread these:
 
  void forwardReadPtr(int bytes);
  int getReadArea(char* &ptr,int &size);
  int waitForData(int minData);
  void exitWaitForData();
  void setCanWaitForData(int lCanWaitForData);
  int getCanWaitForData();


  // and the lockPos
  void forwardLockPtr(int bytes);


  // both:

  int getFillgrade();       // return how much buffer between reader/writer
  void emptyBuffer();    // frees the space between them
  int getFreeRead();
  int getFreeWrite();

  int getSize();
  int getReadBytes();
  int getWriteBytes();

  // make sure that no one calls getReadArea/getWriteArea
  void resizeBuffer(int changeSize);
 private:
  void updateCanWrite();
  void updateCanRead();

  int size;
  
  int lockgrade;
  int fillgrade;
  
  char* readPos;
  char* writePos;
  char* lockPos;

  char* startPos;

  char* lastPos;
  char* eofPos;
  int canWrite;
  int canRead;

  int waitMinData;
  int waitMinSpace;

  abs_thread_mutex_t mut;
  abs_thread_cond_t dataCond;
  abs_thread_cond_t spaceCond;
  
  int insertBlock;
  int readBlock;
  int linAvail;

  char* minLinBuf;
  int minLinBufSize;
  int lWaitForData;
  int lWaitForSpace;

  // statistic purpose:
  int readBytes;
  int writeBytes;

  int lCanWaitForSpace;
  int lCanWaitForData;
  int instance;
};

#endif

  

