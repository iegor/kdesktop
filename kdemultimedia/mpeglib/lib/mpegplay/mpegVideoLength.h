/*
  mpg I video/audio player plugin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __MPEGVIDEOLENGTH_H
#define __MPEGVIDEOLENGTH_H

#include "../input/inputPlugin.h"






/**
   This class calculates the length (in seconds) of an mpeg stream.
   I do not know how to make it right, my approach
   seeks at the beginning of the stream for the timecode in
   the picture gop then jumps near the end and does the same.
   then it substracts the values.
   It has some limitations, it does not seek higher than 600 MB,
   because it seems the kernel has some problems with this.
   (It resets the scsi system, and sometimes hang/crash)
   Thus it only seek near 600 and assumes linear relation
   for the rest. (Means: it calculates the length of the rest
   with the sec/mb value calculates from the 600 mb
   should be exact enough.
*/

class MpegVideoStream;
class MpegVideoHeader;
class MpegSystemStream;
class MpegSystemHeader;
class GOP;

class MpegVideoLength {

  // these are used fo non system streams
  MpegVideoHeader*     mpegVideoHeader;
  MpegVideoStream*     mpegVideoStream;

  // these for system streams
  MpegSystemHeader*    mpegSystemHeader;
  MpegSystemStream*    mpegSystemStream;

  
  InputStream* input;
  GOP* startGOP;
  GOP* endGOP;
  GOP* lengthGOP;


  int lHasStart;
  int lHasEnd;
  int lCanSeek;
  int lHasStream;
  int lHasSystemStream;
  int lHasRawStream;
  int lHasResync;
  int lSysLayer;
  long upperEnd;
  long realLength;

 public:
  MpegVideoLength(InputStream* input);
  ~MpegVideoLength();

  int firstInitialize();

  long getLength();
  long getSeekPos(int seconds);


 private:
  int seekToStart();
  int seekToEnd();
  int seekValue(unsigned int code,long& endPos);
  int parseToGOP(GOP* gop);
  int parseToPTS(GOP* gop);
  

};
#endif

