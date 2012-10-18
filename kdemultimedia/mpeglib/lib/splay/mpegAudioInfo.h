/*
  length detection etc.. for mpeg audio
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#ifndef __MPEGAUDIOINFO_H
#define __MPEGAUDIOINFO_H

#include "../util/file/fileAccess.h"

class MpegAudioStream;
class MpegAudioHeader;
class MpegAudioFrame;

typedef struct {
  char name   [30+1];
  char artist [30+1];
  char album  [30+1];
  char year   [ 4+1];
  char comment[30+1];
  unsigned char genre;
} ID3TAG;


class MpegAudioInfo {

  long length;
  int lXingVBR;
  ID3TAG* id3;
  MpegAudioStream* mpegAudioStream;
  MpegAudioHeader* mpegAudioHeader;
  MpegAudioFrame * mpegAudioFrame;
  FileAccess* input;
  int leof;

  int initState;
  unsigned char inputbuffer[8192];
  int lNeedInit;
 public:
  MpegAudioInfo(FileAccess* input);
  ~MpegAudioInfo();

  void reset();
  int initialize();

  // store info wheter init need or not here.(after reset its true)
  int getNeedInit();
  void setNeedInit(int lNeedInit);


  // returns byte positions
  long getSeekPosition(int second);
  // returns length in seconds
  long getLength();
  
  // for id3 parsing you need to seek -128 bytes before the end of strem
  // after parseID3 you must jump back to 0. This method is blocking
  int initializeID3();
  ID3TAG* getID3();

  void print(const char* msg);

 private:
  // non-blocking init. return true,false,or -1==eof
  int initializeLength(long fileSize);
  int initializeID3(long fileSize);
  
  

  void calculateLength(long fileSize);
  int parseXing(unsigned char* frame,int size);
  
  // used for seek
  void parseID3();
  int getByteDirect();

  // extension for xing vbr
#ifdef _FROM_SOURCE
  XHEADDATA* xHeadData;
#else
  void* xHeadData;
#endif

 private:
  int getFrame(MpegAudioFrame* frame);

};
#endif
