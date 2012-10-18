/*
  reads input data
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#ifndef  __BUFFERINPUTSTREAM_H
#define  __BUFFERINPUTSTREAM_H

#include "../util/abstract/abs_thread.h"

#include "inputStream.h"


class SimpleRingBuffer;

class BufferInputStream : public InputStream {

  SimpleRingBuffer* ringBuffer;
  int leof;
  long bytePos;
  int fillgrade;
  int lLock;
  abs_thread_mutex_t writeInMut;

 public:
  BufferInputStream(int size,int minlinSize,const char* name);
  ~BufferInputStream();

  int open(const char* name);
  void close();

  int isOpen();


  int eof();
  void setBlocking(int lblock);
  int read(char* ptr,int size);

  // reads from a buffer
  int write(char* ptr,int len,TimeStamp* stamp);

  // this method directy read from another inputstream (faster);
  int write(InputStream* ptr,int len,TimeStamp* stamp);
  
  int seek(long bytePos);


  long getByteLength();
  long getBytePosition();

  void setBytePosition(long bytePos);
  int getFillgrade();
  int getSize();
  int getFreeRead();
  int getFreeSpace();

  void clear();

  // remote read extension
  // Note you _need_ to call always both methods
  // readRemote and forwardReadPtr even if bytes==0!!!
  // (we hold a resizeLock during this operation)
  int readRemote(char** ptr,int size);
  void forwardReadPtr(int bytes);
  void setCanWaitForData(int lBlock);


  // this method is only safe to call by the writer in the buffer
  // a reader never should call this (segfault possible)
  void resizeBuffer(int changeSize);

  // for pause/play over loopback
  int getHold();
  void setHold(int lLock);

 private:
  void lockBuffer();
  void unlockBuffer();

};
#endif
