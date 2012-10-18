/*
  reads input data from cdrom
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __VIDEOCDINPUTSTREAM_H
#define __VIDEOCDINPUTSTREAM_H



#include "inputStream.h"



#define _CDROM_FRAMES   75
#define _CDROM_SECS     60


/**
   This is necessary because CD-I or VCD disks are not correctly
   mounted by the linux kernel.
   Windows seems to do it correct (sometimes, the other time it nearly crash)
   During my test I found out that CD-I and VCD seems to be different.
   One works with the CD-I loader the other with the VCD loader.
   
   This class tries to find out if its a CD-I / VCD stream.
   Then you can set the interface and load from this interface.

   This class depends on the reader routines from xreadcdi/ xreadvcd,
   Author: Ales Makarov <xmakarov@sun.felk.cvut.cz>
   FTP   : ftp://mca.sh.cvut.cz/pub/readvcd/

   The code is used in a few other packages, in which I looked as well.

*/

class CDRomRawAccess;
class CDRomToc;

class CDRomInputStream : public InputStream{

  CDRomRawAccess* cdRomRawAccess;
  CDRomToc* cdRomToc;

  int buflen;
  char* bufCurrent;
 
  int currentFrame;
  int currentMinute;
  int currentSecond;
  long byteCounter;
 
 public:
  CDRomInputStream();
  ~CDRomInputStream();

  int open(const char* dest);
  void close();
  int isOpen();

  int eof();
  int read(char* ptr,int size);
  int seek(long bytePos);

  long getByteLength();
  long getBytePosition();

  void print();

 private:
  long getBytePos(int min,int sec);

  int setTimePos(int posInTime);
  int getByteDirect();
  void next_sector();
  int readCurrent();
  int fillBuffer();
};
#endif
