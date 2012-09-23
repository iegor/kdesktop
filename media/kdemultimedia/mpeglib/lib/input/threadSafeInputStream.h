/*
  thread safe wrapper for input Stream
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __THREADSAFEINPUTSTREAM_H
#define __THREADSAFEINPUTSTREAM_H

// read INTRO in threadQueue.h
// This class makes the inputStream (given in constructor)
// threadsafe by wrapping each call with a threadqueue.
//
// Important NOTE: the input pointer is the owned by this class !!!
//                 which means: we call delete on it!

#include "../util/abstract/threadQueue.h"
#include "inputStream.h"


class ThreadSafeInputStream : public InputStream {

  ThreadQueue* threadQueue;
  InputStream* input;

 public:
  ThreadSafeInputStream(InputStream* input);
  ~ThreadSafeInputStream();


  int open(const char* dest);
  void close();
  int isOpen();

  int eof();
  int read(char* ptr,int size);
  int seek(long bytePos);
  void clear();

  long getByteLength();
  long getBytePosition();

  void insertTimeStamp(TimeStamp* src,long key,int len);
  TimeStamp* getTimeStamp(long key);
  int bytesUntilNext(long key);
  void print();

};
#endif
