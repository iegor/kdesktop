/*
  thread safe fifo queue for commands
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "commandPipe.h"

#define _COMMAND_ARRAY_SIZE   100

CommandPipe::CommandPipe() {
  abs_thread_cond_init(&spaceCond);
  abs_thread_cond_init(&emptyCond);
  abs_thread_cond_init(&dataCond);
  abs_thread_mutex_init(&pipeMut);

  entries=0;
  readPos=0;
  writePos=0;

  
  int i;
  
  commandArray=new Command*[_COMMAND_ARRAY_SIZE];


  for(i=0;i<_COMMAND_ARRAY_SIZE;i++) {
    commandArray[i]=new Command(_COMMAND_NONE,0);
  }
}


CommandPipe::~CommandPipe() {
  abs_thread_cond_destroy(&spaceCond);
  abs_thread_cond_destroy(&emptyCond);
  abs_thread_cond_destroy(&dataCond);
  abs_thread_mutex_destroy(&pipeMut);


  int i;

  for(i=0;i<_COMMAND_ARRAY_SIZE;i++) {
    delete commandArray[i];
  }
  delete [] commandArray;
}

void CommandPipe::sendCommand(Command& cmd) {
  sendCommand(cmd,true);
}

void CommandPipe::sendCommandNoWait(Command& cmd) {
  sendCommand(cmd,false);
}


void CommandPipe::sendCommand(Command& cmd,int lWait) {
  lockCommandPipe();
  if (entries==_COMMAND_ARRAY_SIZE) {
    waitForSpace();
  }
  cmd.copyTo(commandArray[writePos]);
  writePos++;
  if (writePos==_COMMAND_ARRAY_SIZE) {
    writePos=0;
  }
  entries++;
  // low water signal
  if (entries==1) {
    signalData();
  }
  unlockCommandPipe(); 
  if (lWait) {
    waitForEmptyQueue();
  }
}


int CommandPipe::hasCommand(Command* dest) {
  lockCommandPipe();
  if (entries==0) {
    unlockCommandPipe();
    return false;
  }

  commandArray[readPos]->copyTo(dest);
  readPos++;
  if (readPos==_COMMAND_ARRAY_SIZE) {
    readPos=0;
  }  
  entries--;
  // low water
  if (entries==0) {
    signalEmpty();
    unlockCommandPipe();
    return true;
  }
  // if we are on highwater, signal space
  if (entries==_COMMAND_ARRAY_SIZE-1) {
    signalSpace();
  }
  unlockCommandPipe();
  return true;
}


void CommandPipe::waitForEmptyQueue() {
  lockCommandPipe();
  while (entries > 0) {
    waitForEmpty();
  }
  unlockCommandPipe();
}

void CommandPipe::waitForCommand() {
  lockCommandPipe();
  while (entries == 0) {
    waitForData();
  }
  unlockCommandPipe();
}


void CommandPipe::lockCommandPipe() {
  abs_thread_mutex_lock(&pipeMut);
}


void CommandPipe::unlockCommandPipe() {
  abs_thread_mutex_unlock(&pipeMut);
}

void CommandPipe::waitForSpace() {
  abs_thread_cond_wait(&spaceCond,&pipeMut);
}

void CommandPipe::waitForEmpty() {
  abs_thread_cond_wait(&emptyCond,&pipeMut);
}
 
void CommandPipe::waitForData() {
  abs_thread_cond_wait(&dataCond,&pipeMut);
}


void CommandPipe::signalSpace() {
  abs_thread_cond_signal(&spaceCond);
}

void CommandPipe::signalEmpty() {
  abs_thread_cond_signal(&emptyCond);
}

void CommandPipe::signalData() {
  abs_thread_cond_signal(&dataCond);
}
