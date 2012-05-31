/*
  mpeg player plugin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __MPEGPLUGIN_H
#define __MPEGPLUGIN_H

#include "../decoder/decoderPlugin.h"


class MpegVideoHeader;
class MpegVideoStream;


class MpegPlugin : public DecoderPlugin {
  
  int lCalcLength;

  MpegVideoHeader* mpegVideoHeader;
  MpegVideoStream* mpegVideoStream;

 public:
  MpegPlugin();
  ~MpegPlugin();

  void decoder_loop();
  void config(const char* key,const char* value,void* user_data);

  
 private:
  void init();

  int getSongLength();
 

};
#endif

