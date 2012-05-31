/*
  demux transport stream
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __TSSYSTEMSTREAM_H
#define __TSSYSTEMSTREAM_H


#include <stdlib.h>
#include "../input/inputPlugin.h"
#include "mpegSystemHeader.h"



/**
   This class is used inside mpegSystemStream.h when we found
   during initialisation an transport stream.
   

*/

class TSSystemStream {

  InputStream* input;
  
  int paket_len;
  int paket_read;

 

 public:
  TSSystemStream(InputStream* input);
  ~TSSystemStream();

  // called when we found a valid ts startcode
  int processStartCode(MpegSystemHeader* mpegHeader);


 private:

  // read stream methods
  int read(char* ptr,int bytes);
  int getByteDirect();
  int nukeBytes(int bytes);
  int skipNextByteInLength();


  // process Format methods
  int processSection(MpegSystemHeader* mpegHeader);
  int processPrograms(int sectionLength,MpegSystemHeader* mpegHeader);
  int processElementary(int sectionLength,MpegSystemHeader* mpegHeader);

  int demux_ts_pat_parse(MpegSystemHeader* mpegHeader);
  int demux_ts_pmt_parse(MpegSystemHeader* mpegHeader);


};
#endif
