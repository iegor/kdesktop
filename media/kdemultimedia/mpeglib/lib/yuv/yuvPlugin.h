/*
  raw yuv stream player
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */





#ifndef __YUVPLUGIN_H
#define __YUVPLUGIN_H

#include "../decoder/decoderPlugin.h"
#include <kdemacros.h>

class KDE_EXPORT YUVPlugin : public DecoderPlugin {
  
  int lCalcLength;
  int nWidth;
  int nHeight;
  int imageType;
  float picPerSec;

 public:
  YUVPlugin();
  ~YUVPlugin();

  void decoder_loop();
  void config(const char* key,const char* value,void* user_data);

  
 private:
  void init();

  int getSongLength();
 

};

#endif
