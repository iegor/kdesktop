/*
  a wrapper for the audioDevice.
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef _DSPWRAPPER_H
#define _DSPWRAPPER_H

class AudioFrame;
class PCMFrame;
class FloatFrame;
#include <kdemacros.h>

/**
   This class wraps the platform specific /dev/dsp implementation.
   The only unusal thing is, that it supports each order of
   init/open.
   i) you can first init the device and the open
   ii) you can first open the device and the init it
   The implementation takes care that the calls are forwarded
   in the right order to the /dev/dsp implementation.
   (means: before the init it, we need to open it)
   But a caller can do it in both orders.
*/

class KDE_EXPORT DSPWrapper  {

  int lopenDevice;
  int lopenMixer;
  PCMFrame* currentFormat;


 public:
  DSPWrapper();
  ~DSPWrapper();
  
  int openDevice();
  int closeDevice();
  int isOpenDevice();

  int openMixer();
  int closeMixer();
  int isOpenMixer();

  int getAudioBufferSize();
  void setVolume(float leftPercent,float rightPercent);

  int audioSetup(int stereo,int sampleSize,int lSigned,
		 int lBigEndian,int freq);
  int audioSetup(AudioFrame* audioFrame);
  
  int audioPlay(char *buffer, int size);  
  int audioPlay(PCMFrame* pcmFrame);
  int audioPlay(FloatFrame* floatFrame);
  void audioFlush();

  int isEqual(int samplesize,int speed,int stereo,int lSigned,int lBigEndian);
  int write(char* buf,int len);
  void print();
};

#endif




