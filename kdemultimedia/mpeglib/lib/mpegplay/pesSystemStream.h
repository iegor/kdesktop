/*
  demux pes mpeg stream 
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __PESSYSTEMSTREAM_H
#define __PESSYSTEMSTREAM_H


#include <stdlib.h>
#include "../input/inputPlugin.h"
#include "mpegSystemHeader.h"


class PESSystemStream {


  InputStream* input;
  
  int bytes_read;

 public:
  PESSystemStream(InputStream* input);
  ~PESSystemStream();

  // called when we found a valid ts startcode
  // return number of bytes read from input and zero on error
  int processStartCode(unsigned int startCode,MpegSystemHeader* mpegHeader);

 private:
  int getByteDirect();
  int read(char* prt,int bytes);
  int processPacket(unsigned int startCode,MpegSystemHeader* mpegHeader);
  int processPacketHeader(MpegSystemHeader* mpegHeader);
  int processMPEG2PacketHeader(MpegSystemHeader* mpegHeader);
  int processPrivateHeader(MpegSystemHeader* mpegHeader);

  int readStartCode(unsigned int startCode,MpegSystemHeader* mpegHeader);

  void readSTD(unsigned char* inputBuffer,MpegSystemHeader* mpegHeader);
  void readTimeStamp(unsigned char* inputBuffer,unsigned char* hiBit,
		     unsigned long* low4Bytes);

  int makeClockTime(unsigned char hiBit, unsigned long low4Bytes,
		    double * clockTime);
  

};
#endif
