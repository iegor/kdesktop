/*
  cdda input class based on cdparanoia
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __CDDAINPUTSTREAM_H
#define __CDDAINPUTSTREAM_H


#include "inputStream.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef CDDA_PARANOIA
class CDDAInputStream : public InputStream {
};
#else

#include <sys/types.h>
typedef int16_t size16;
typedef int32_t size32;

extern "C" {
#include <cdda_interface.h>
#include <cdda_paranoia.h>
void paranoiaCallback(long, int);
}
//#define CDDA_INCLUDE


class CDDAInputStream : public InputStream {

//#ifdef CDDA_INCLUDE
  cdrom_paranoia * paranoia;
  struct cdrom_drive * drive;
//#else
//  void * drive;
//  void * paranoia;
//#endif
    

  char* device;
  int track;

  int firstSector;
  int lastSector;
  int currentSector;

 public:
  CDDAInputStream();
   ~CDDAInputStream();

   int open(const char* dest);
   void close();
   int isOpen();

   int eof();
   int read(char* ptr,int size);
   int seek(long bytePos);
   // clears possible input buffers
   // (called by the decoderPlugin after a resyncCommit)
   void clear();

   long getByteLength();
   long getBytePosition();
 private:
   int getTrackAndDevice(const char* url);
 
};
#endif
//CDDA_PARANOIA

#endif
