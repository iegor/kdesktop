/*
  An abstraction of a Command-Line
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */





#ifndef __BASICMESSAGE_H
#define __BASICMESSAGE_H


#include "buffer.h"


struct BasicMessageStruct {

  Buffer* identifier;
  Buffer* value;

};
    
typedef struct BasicMessageStruct tBasicMessage;


class CommandLine {
  

  int commandCount;
  tBasicMessage command[40];

 public:

  CommandLine();
  ~CommandLine();

  int getCommandCount();
  void setCommandCount(int commandCount);

  char* getIdentifier(int i);
  char* getValue(int i);

  void setIdentifier(int i,char* identifier);
  void setValue(int i,char* value);

  void printCommand();
  void clear();
};

#endif


