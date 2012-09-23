/*
  a class which scans a string and converts it into commandLines
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */





#ifndef __LINESTACK_H
#define __LINESTACK_H


#include "buffer.h"

/* 
   This class is a hack because I have problems with
   the io-streams. Why these stupid things cannot buffer
   a whole line?
   They behave strange, I hate them.
*/
   


class LineStack {

 public:
  LineStack();
  ~LineStack();

  void appendBottom(char* buffer);
  void appendBottom(char* buffer,int buflen);
  void appendBottom(LineStack* stack);
  
  int hasLine();
  void nextLine(Buffer* nextLine);

  void print(char* name);
 private:
  char* getReturnPos();

  Buffer* stack;
};


#endif
  
  
