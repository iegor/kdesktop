/*
  parses extension data (picture and sequence)
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */





#ifndef __MPEGEXTENSION_H
#define __MPEGEXTENSION_H

#include "mpegVideoStream.h"

#define EXT_BUF_SIZE 1024

class MpegExtension {

  char* userData;
  char* extData;
  char* extraBit;

 public:
  MpegExtension();
  ~MpegExtension();
  
  inline char* getExtData()  { return extData;}
  inline char* getUserData() { return userData;}
  inline char* getExtraBit() { return extraBit;}

  int processExtensionData(class MpegVideoStream* mpegVideoStream);
  int processExtra_bit_info(class MpegVideoStream* mpegVideoStream);
  void processExtBuffer(class MpegVideoStream* mpegVideoStream);
  char* get_ext_data(class MpegVideoStream* mpegVideoStream);
 
 private:
  char* get_extra_bit_info(MpegVideoStream* mpegVideoStream);

  int next_bits(int num,unsigned int mask,MpegVideoStream* input);

};
#endif
