/*
  Default output decoder
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __OUTPUTDECODER_H
#define __OUTPUTDECODER_H

#include "commandLine.h"

#include "runtimeTableYAF.h"
#include "commandTableYAF.h"


class OutputDecoder  {

 public:
  OutputDecoder();
  virtual ~OutputDecoder();

  void processCommandLine(CommandLine*);
  virtual int processRuntimeCommand(int command,const char* args);
  virtual int processReturnCommand(int cmdNr,int cmdId,
				   const char* ret,const char* args);

  void appendCommandTable(CommandTable* table);
  void appendRuntimeTable(CommandTable* table);

  CommandTable* getRuntimeTable();
  CommandTable* getCommandTable();

 private:
  CommandTable* yafCommands;
  CommandTable* yafRuntime;

  CommandTable commandTable;
  CommandTable runtimeTable;


};


#endif
