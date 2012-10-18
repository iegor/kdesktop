/*
  a buffer
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */





#ifndef __MPEGVIDEOSTREAM_H
#define __MPEGVIDEOSTREAM_H

#include "startCodes.h"
#include "mpegVideoHeader.h"
#include "mpegSystemStream.h"
#include "mpegVideoBitWindow.h"

#define _BYTE_TEST 1024

/**
   A really ugly class. Most of the methods have names
   which does not make it clear for what they are useful.
   (Don't touch a running system :-)

   We wrap the inputStream and offer functions for getting
   bits, appending to the internal buffer, flushing, syncing
   all this stuff.


*/

class MpegVideoStream {

  int size;
  InputStream* input;
  MpegSystemStream* mpegSystemStream;
  MpegSystemHeader* mpegSystemHeader;
  MpegVideoBitWindow* mpegVideoBitWindow;

  int lHasStream;

 public:
  MpegVideoStream(InputStream* input);
  ~MpegVideoStream();

  // returns true if init successful
  int firstInitialize(MpegVideoHeader* mpegHeader);
  int nextGOP();
  int nextPIC();
  int next_start_code();
  
  int hasBytes(int bytes);
  int eof();

  inline void clear() { 
    mpegVideoBitWindow->clear();      
  }

  inline void flushBits(int bits) { 
    hasBytes(_BYTE_TEST);
    mpegVideoBitWindow->flushBitsDirect(bits); 
  }

  inline unsigned int showBits(int bits) { 
    hasBytes(_BYTE_TEST);
    return mpegVideoBitWindow->showBits(bits);
  }

  inline unsigned int showBits32() { 
    return mpegVideoBitWindow->showBits32(); 
  }

  inline unsigned int showBits16() { 
    return mpegVideoBitWindow->showBits(16); 
  }  

  inline void flushBitsDirect(unsigned int bits) {
     mpegVideoBitWindow->flushBitsDirect(bits);
  }

  unsigned int getBits(int bits) { 
    hasBytes(_BYTE_TEST);
    return mpegVideoBitWindow->getBits(bits);
  }

  inline void flushByteOffset() { 
    mpegVideoBitWindow->flushByteOffset();
  }

  TimeStamp* getCurrentTimeStamp();

 private:
  int getByteDirect();
  int get_more_video_data();
  void fill_videoBuffer(MpegSystemHeader* mpegSystemHeader);
  int isStartCode(unsigned int data);


};
#endif



