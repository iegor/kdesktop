/*
  base class for all mpeglib decoders
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __DECODERBASEOBJECT_IMPL_H
#define __DECODERBASEOBJECT_IMPL_H


#include <math.h>
#include "decoderBaseObject.h"
#include "stdsynthmodule.h"
#include "convert.h"
#include <X11/Xlib.h>
#include <audiosubsys.h>
#include <stdio.h>
#include <string.h>

#include <arts/kmedia2.h>

#define _THREADSTATE_INIT         0
#define _THREADSTATE_OPENED       1
#define _THREADSTATE_PAUSED       2
#define _THREADSTATE_PLAYING      3
#define _THREADSTATE_CLOSED       4

#include <queue>

class DecoderPlugin;
class InputStream;
class ArtsOutputStream;

using namespace std;
using Arts::poState;
using Arts::poTime;
using Arts::poCapabilities;
using Arts::DataPacket;
using Arts::mcopbyte;

class DecoderBaseObject_impl :
  virtual public Arts::StdSynthModule,
  virtual public DecoderBaseObject_skel {

  poState _state;
  InputStream* m_inputStream;
  Arts::InputStream m_artsInputStream;
  ArtsOutputStream* m_outputStream;
  
  double flpos;
  float startTime;
  int instance;
  int lastAudioBufferSize;
  int streamState;
  bool _blocking;
  float _speed;

  bool m_streaming;

  queue<DataPacket<mcopbyte>*> *m_packetQueue;

public:

  DecoderBaseObject_impl();
  virtual ~DecoderBaseObject_impl();

  virtual DecoderPlugin* createPlugin();
  virtual InputStream* createInputStream(const char* url);
  virtual bool doFloat() { return false; }

  bool loadMedia(const string &filename);
  bool streamMedia(Arts::InputStream instream);
  void process_indata(DataPacket<mcopbyte>* inpacket);
  Arts::InputStream inputStream() { return m_artsInputStream; }

  string description();
  void description(const string &);

  poTime currentTime();
  poTime overallTime();

  poCapabilities capabilities();
  string mediaName();
  poState state();
  void play();
  void halt();

  void blocking(bool newvalue);
  bool blocking();

  void speed(float newValue);
  float speed();

  void seek(const class poTime &);
  void pause();
  void streamInit();
  void streamStart();
  void calculateBlock(unsigned long samples,float* left,float* right);
  void streamEnd();
 
  DecoderPlugin* decoderPlugin;
  
 private:
  void shudownPlugins();
  int getBufferSize();
  int fillArts(unsigned long samples,float* left , float* right);
  void setStreamState(int state);
  void processQueue();

};



#endif
