/*
  This class implements a static string buffer
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __BUFFER_H
#define __BUFFER_H


extern "C" {
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
	   }
#include <kdemacros.h>
  
class KDE_EXPORT Buffer {

  char* msg;
  int nSize;

 public:
  Buffer(int size);
  ~Buffer();

  void clear();
  void append(int value);     // appends values as string
  void append(char* msg);
  void append(const char* msg);
  
  void append(char* buffer,int buflen);

  int find(char zeichen);
  int len();

  void setData(char* msg);
  char* getData();

  int getSize();
  void grow(int size);

  void forward(int bytes);
  void print();
 private:
  char* getAppendPos();
  void read(FILE stream);

};


#endif



