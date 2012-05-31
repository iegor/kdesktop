/*
  reads paket input data
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#ifndef __MPEGSYSTEMSTREAM_H
#define __MPEGSYSTEMSTREAM_H



#include "tsSystemStream.h"
#include "psSystemStream.h"
#include "pesSystemStream.h"


class MpegSystemStream {

  InputStream* input;
  int lHasLength;
  unsigned int syncCode;
  int lState;
  TSSystemStream*  tsSystemStream;
  PSSystemStream*  psSystemStream;
  PESSystemStream* pesSystemStream;

 public:
  MpegSystemStream(InputStream* input);
  ~MpegSystemStream();

  // returns true if init successful
  int firstInitialize(MpegSystemHeader* mpegHeader);
  int nextPacket(MpegSystemHeader* mpegHeader);
  inline int eof() { return input->eof(); }
  void reset();

 private:
  int readSyncCode();
  int getByteDirect();
  int demux_ts_pes_buffer(MpegSystemHeader* mpegHeader);

  int processSystemHeader(MpegSystemHeader* mpegHeader);
  

};

#endif
