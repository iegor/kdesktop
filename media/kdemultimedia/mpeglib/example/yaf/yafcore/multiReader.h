/*
  This class can waits for input on different istreams
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __MULTIREADER_H
#define __MULTIREADER_H

#include "lineStack.h"
#include "buffer.h"

extern "C" {
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <strings.h>
#include <string.h>
	   }

/**
 The purpose of this class is to have a thread safe input method
 for yaf command.
 <p>
 Sometime the decoder thread must send data to the control thread.
 E.g. the Stream has ended. The decoder thread must inform the
 control thread that the player should go into the "off" state.
 A direct call is not thread safe and this is why the decoder
 thread must use this class. For the control thread it looks
 like the user has typed "off".

*/

#define _MAX_INPUT 5

typedef struct {
  LineStack* tmpLineStack;        // owned by class
  int fd;                         // filedescriptor for input              
  int empty;
} LineInput;


class MultiReader {

  Buffer* buffer;
  LineInput* lineInputArray[_MAX_INPUT];
  LineStack* script;

 public:
  MultiReader();
  ~MultiReader();

  int add(int fd);
  void add(LineStack* aScript);
  void add(Buffer* aScript);
  void remove (int fd);

  void waitForLine();
  void poll(struct timeval* timeout);

  int hasLine();
  void getLine(Buffer* buffer);

 private:

  void doSelect(struct timeval *timeout);

  int getEmptySlot();
  int getSlot(int fd);
  

};





#endif
