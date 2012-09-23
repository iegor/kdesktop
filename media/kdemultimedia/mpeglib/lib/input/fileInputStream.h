/*
  reads input data
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __FILEINPUTSTREAM_H
#define __FILEINPUTSTREAM_H



#include "inputStream.h"



class FileInputStream : public InputStream{
  
  FILE* file;
  int lopen;
  long fileLen;
 public:
  FileInputStream();
  ~FileInputStream();

  int open(const char* dest);
  void close();
  int isOpen();

  int eof();
  int read(char* ptr,int size);
  int seek(long bytePos);

  long getByteLength();
  long getBytePosition();

  void print();
  
};
#endif
