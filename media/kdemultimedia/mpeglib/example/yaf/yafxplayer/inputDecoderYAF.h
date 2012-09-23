/*
  base Implementation of a yaf decoder
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __INPUTDECODERYAF_H
#define __INPUTDECODERYAF_H

#include <pthread.h>

#include "inputDecoderXPlayer.h"
#include "yafOutputStream.h"
#include <kdemacros.h>


class KDE_EXPORT InputDecoderYAF : public InputDecoderXPlayer {

  DecoderPlugin* plugin;
  YafOutputStream* output;
  InputStream* input;
  int lAutoPlay;

 public:
  InputDecoderYAF(DecoderPlugin* plugin,YafOutputStream* output);
  ~InputDecoderYAF();

  const char* processCommand(int command,const char* args);
  void doSomething();
  
  int getAutoPlay();
  void setAutoPlay(int lAutoPlay);

 private:

  int lFileSelected;
  Buffer* songPath;
};


#endif

