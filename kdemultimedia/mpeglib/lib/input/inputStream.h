/*
  generic input class
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __MPEGLIBINPUTSTREAM_H
#define __MPEGLIBINPUTSTREAM_H


extern "C" {
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
}

#include "../util/timeStampArray.h"
#include "../util/dynBuffer.h"




/**
   The abstraction for the input stream. In derived classes
   we implement http,file and cdi access.
*/


class InputStream {



 public:
  InputStream();


  virtual ~InputStream();

  virtual int open(const char* dest);
  virtual void close();
  virtual int isOpen();

  virtual int eof();
  virtual int read(char* ptr,int size);
  virtual int seek(long bytePos);
  // clears possible input buffers
  // (called by the decoderPlugin after a resyncCommit)
  virtual void clear();

  virtual long getByteLength();
  virtual long getBytePosition();

  virtual void insertTimeStamp(TimeStamp* src,long key,int len);
  virtual TimeStamp* getTimeStamp(long key);
  virtual int bytesUntilNext(long key);
  virtual void print();
  char* getUrl();



 protected:
  DynBuffer* urlBuffer;


  class TimeStampArray* timeStampArray;
  void setUrl(const char* url);


};
#endif
