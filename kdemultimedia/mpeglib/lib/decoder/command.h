/*
  describes a command
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __COMMAND_H
#define __COMMAND_H

#include "../util/abstract/abs_thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define _COMMAND_NONE             0
#define _COMMAND_PLAY             1
#define _COMMAND_PAUSE            2
#define _COMMAND_SEEK             3
#define _COMMAND_CLOSE            4
#define _COMMAND_START            5
#define _COMMAND_RESYNC_START     6
#define _COMMAND_RESYNC_END       7
#define _COMMAND_PING             8



class Command {

  int id;
  int intArg;

 public:
  Command(int id);
  Command(int id,int arg);
  ~Command();
  
  int getID();
  int getIntArg();
  void copyTo(Command* dest);
  void print(const char* text);
};

#endif
