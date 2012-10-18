/*
  reads raw input data from cdrom (system dependent)
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __CDROMRAWACCESS_H
#define __CDROMRAWACCESS_H

#include "cdromToc.h"
#include "inputStream.h"


/**
   Here we have a system wrapper for raw cdrom access.
   Currently this is tested on Linux.
*/





class CDRomRawAccess {

  CDRomToc* cdromToc;
  char data[2352];
  int dataStart;
  int len;
  int lData;

 public:
  CDRomRawAccess(CDRomToc* cdromToc);
  virtual ~CDRomRawAccess();
  
  // overload this for new Systems
  virtual int readDirect(int minute,int second, int frame);
  
  // wrapper for readDirect
  int read(int minute,int second, int frame);

  char* getBufferStart();
  int getBufferLen();

  int open(const char* filename);
  int eof();
  void close();
  int isOpen();

  int isData();

 private:
  FILE* cdfile;
  int lOpen;
  int leof;
  int buflen;
};

#endif


