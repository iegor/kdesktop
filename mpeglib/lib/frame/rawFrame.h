/*
  base class for raw frames (encoded data, where only the type is known)
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __RAW_FRAME_H
#define __RAW_FRAME_H

#include "frame.h"

/**
   Raw frames represents bitstreams. They most likely have have now
   real value for anyone but a decoder.
   In general you simply allocate a rawFrame with a given size, This
   size should make sure, that the bitstream packet does fit into
   the frame. Sometime, in derived classes you can set thes pointer
   to the allocated directly by calling the protected method: setRemoteData.
*/


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef WORDS_BIGENDIAN
#define RAWFRAME_BIGENDIAN 1
#else
#define RAWFRAME_BIGENDIAN 0
#endif


class RawFrame : public Frame {

  unsigned char* data;
  int size;
  int len;
  int lRemoteData;

 public:
  RawFrame(int size);
  RawFrame(int type,int size);
  ~RawFrame();

  // access start of frameData
  unsigned char* getData()                { return data; }

  // current size in byte
  int getLen()                            { return len; }
  void setLen(int bytes)                  { this->len=bytes; }

  // maximum size of allocated memory
  int getSize()                           { return size; }


  void print(const char* msg);

 private:
  void init(int type,int size);

 protected:
  void setRemoteData(unsigned char* data,int size);
};



#endif
