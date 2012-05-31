/*
  splay player plugin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __SPLAYPLUGIN_H
#define __SPLAYPLUGIN_H

#include "../decoder/decoderPlugin.h"
#include <kdemacros.h>

class SplayDecoder;
class MpegAudioFrame;
class AudioFrame;
class FloatFrame;
class PCMFrame;
class FileAccessWrapper;
class MpegAudioInfo;

class KDE_EXPORT SplayPlugin : public DecoderPlugin {
  
  int lnoLength;
  int lfirst;
  int lOutput;
  /*
   * directly writes decoded data as float, instead of converting it to
   * short int samples first. float->int conversions are _very_
   * time intensiv!
   */
  bool doFloat;

  SplayDecoder* splay;
  MpegAudioFrame* framer;
  FloatFrame* floatFrame;
  PCMFrame* pcmFrame;
  unsigned char* inputbuffer;
  int lenghtInSec;
  MpegAudioInfo* info;
  FileAccessWrapper* fileAccess; 
  int resyncCounter;
  AudioFrame* audioFrame;


 public:
  SplayPlugin();
  ~SplayPlugin();

  void decoder_loop();
  int seek_impl(int second);
  void config(const char* key,const char* value,void* user_data);
 
 protected: 
  int getTotalLength();
  void processStreamState(TimeStamp* stamp,AudioFrame* audioFrame);
  void audioSetup(AudioFrame* setupFrame);
  int doFrameFind();

};
#endif
