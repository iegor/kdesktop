/*
  process Messages on the decoder
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __INPUTDECODER_H
#define __INPUTDECODER_H
#include <pthread.h>

#include "outputInterface.h"
#include <strings.h>
#include <kdemacros.h>

#define _DECODER_STATUS_IDLE       1
#define _DECODER_STATUS_WORKING    2
#define _DECODER_STATUS_EXIT       3

class CommandTable;
class CommandTableYAF;
class CommandLine;

class KDE_EXPORT InputDecoder {

  int status;
  CommandTable* commandTable;
  CommandTableYAF* yafCommands;
  Buffer* returnBuffer;
  Buffer* returnLine;
  int lRuntimeInfo;



  
 public:
  InputDecoder();
  virtual ~InputDecoder();
  
  void processCommandLine(CommandLine*);
  virtual const char* processCommand(int command,const char* args);
  virtual void doSomething();

  virtual void setDecoderStatus(int status);
  int getDecoderStatus();

  const char* getReturnCode();

  void appendCommandTable(CommandTable* table);

  void setRuntimeInfo(int lRuntimeInfo);
  int getRuntimeInfo();


 private:

  void clearReturnBuffer();
  void appendReturnBuffer(const char* msg);


  int commandCounter;
  int commandId;
  const char* commandMsg;
  const char* commandArgs;
  const char* retString;
  char* commandCounterString;
  
  
};

#endif

