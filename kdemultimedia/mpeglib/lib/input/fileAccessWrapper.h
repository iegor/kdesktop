/*
  wraps an inputStream for the splayFileBuffer.
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __FILEACCESSWRAPPER_H
#define __FILEACCESSWRAPPER_H


#include "../util/file/fileAccess.h"
#include "inputStream.h"

 
class FileAccessWrapper : public FileAccess {

  InputStream* input;

 public:
  FileAccessWrapper(InputStream* input);
  virtual ~FileAccessWrapper();

  int open(const char* file);
  void close();
  int read(char* dest,int len);
  int eof();
  int seek(long pos);
  long getBytePosition();
  long getByteLength();

};


#endif
