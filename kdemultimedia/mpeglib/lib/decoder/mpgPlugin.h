/*
  mpg I video/audio player plugin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __MPGPLUGIN_H
#define __MPGPLUGIN_H

#include "../decoder/decoderPlugin.h"
#include <kdemacros.h>

#define _INSERT_NO            0
#define _INSERT_VIDEO         1
#define _INSERT_AUDIO         2
#define _INSERT_ALL           3

class MpegSystemHeader;
class MpegSystemStream;
class MpegStreamPlayer;
class MpegVideoLength;

class KDE_EXPORT MpgPlugin : public DecoderPlugin {

  MpegSystemHeader* mpegSystemHeader;
  MpegSystemStream* mpegSystemStream;
  MpegStreamPlayer* mpegStreamPlayer;
  MpegVideoLength*  mpegVideoLength;
  TimeStamp* timeStamp;
  int lMono;
  int lDown;
  int lWriteStreams;
  int lDoLength;

 public:
  MpgPlugin();
  ~MpgPlugin();

  void decoder_loop();
  int getTime(int lCurrent);

  void config(const char* key,const char* value,void* user_data);

  int processThreadCommand(Command* command);

  
 
 protected: 
  int getTotalLength();


};
#endif

