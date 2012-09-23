/*
  simple file access interface.
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#ifndef __FILEACCESS_H
#define __FILEACCESS_H


#include "stdio.h"

/**
   Extension to the normal "read" function.
   This interface is useful for the AudioInfo.
*/

class FileAccess {

  FILE* file;
  long length;

 public:
  FileAccess();
  virtual ~FileAccess();

  virtual int open(const char* file);
  virtual void close();
  virtual int read(char* dest,int len);
  virtual int eof();
  virtual int seek(long pos);
  virtual long getBytePosition();
  virtual long getByteLength();

 private:
  long calcByteLength();

};
#endif

