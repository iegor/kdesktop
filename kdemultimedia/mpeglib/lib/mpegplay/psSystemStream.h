/*
  demux "normal" mpeg stream (does this have a name?)
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __PSSYSTEMSTREAM_H
#define __PSSYSTEMSTREAM_H


#include <stdlib.h>
#include "../input/inputPlugin.h"
#include "mpegSystemHeader.h"

/**
   This class is used inside mpegSystemStream.h when we found
   during initialisation an "normal" mpeg stream.
   This means: 1ba,1bb startcodes.
   

*/

class PSSystemStream {

  InputStream* input;
  
  int paket_len;
  int paket_read;

 public:
  PSSystemStream(InputStream* input);
  ~PSSystemStream();

  // called when we found a valid ts startcode
  int processStartCode(MpegSystemHeader* mpegHeader);

 private:

  // read stream methods
  int read(char* ptr,int bytes);
  int getByteDirect();
  int nukeBytes(int bytes);
  int skipNextByteInLength();

  int processPackHeader(MpegSystemHeader* mpegHeader);
  int processSystemHeader(MpegSystemHeader* mpegHeader);

  
};
#endif
