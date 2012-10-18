/*
  describes a command
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#include "command.h"

#include <iostream>

using namespace std;

Command::Command(int id) {
  this->id=id;
  this->intArg=0;
}

Command::Command(int id,int arg) {
  this->id=id;
  this->intArg=arg;
}


Command::~Command() {
}


int Command::getID() {
  return id;
}


int Command::getIntArg() {
  return intArg;
}


void Command::copyTo(Command* dest) {
  dest->id=this->id;
  dest->intArg=this->intArg;
}

void Command::print(const char* text) { 
  cout << "COMMAND:"<<text<<endl;
  switch(id) {
  case _COMMAND_NONE:
    cout << "_COMMAND_NONE";
    break;
  case _COMMAND_PING:
    cout << "_COMMAND_PING";
    break;
  case _COMMAND_PAUSE:
    cout << "_COMMAND_PAUSE";
    break;
  case _COMMAND_PLAY:
    cout << "_COMMAND_PLAY";
    break;
  case _COMMAND_SEEK:
    cout << "_COMMAND_SEEK";
    cout << " intArg:"<<intArg;
    break;
  case _COMMAND_CLOSE:
    cout << "_COMMAND_CLOSE";
    break;
  case _COMMAND_START:
    cout << "_COMMAND_START";
    break;
  case _COMMAND_RESYNC_START:
    cout << "_COMMAND_RESYNC_START";
    break;
  case _COMMAND_RESYNC_END:
    cout << "_COMMAND_RESYNC_END";
    break;

  default:
    cout << "unknown command id in Command::print"<<endl;
  }
  cout << endl;
}
