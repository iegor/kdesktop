/*
  thread safe fifo queue for commands
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __COMMAND_PIPE_H
#define __COMMAND_PIPE_H



#include "command.h"

/**
   This queue deals with the ugly deadlock problem.
   We insert all commands in a queue and the thread
   polls this queue for next commands.
*/

class CommandPipe {

  Command** commandArray;
  int entries;
  int readPos;
  int writePos;

  abs_thread_mutex_t pipeMut; 
  abs_thread_cond_t spaceCond;
  abs_thread_cond_t emptyCond;
  abs_thread_cond_t dataCond;

 public:
  CommandPipe();
  ~CommandPipe();
  
  // writer thread:
  void sendCommand(Command& cmd);
  void sendCommandNoWait(Command& cmd);
  
  void waitForEmptyQueue();

  // reader thread
  int hasCommand(Command* dest);
  void waitForCommand();
  



 private:
  void sendCommand(Command& cmd,int lWait);

  void lockCommandPipe();
  void unlockCommandPipe();
 

  // writer thread wait here
  void waitForSpace();
  void waitForEmpty();
  void waitForData();
  
  // reader thread signals this
  void signalSpace();
  void signalEmpty();
  void signalData();


  

};

#endif
