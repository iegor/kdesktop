/*
  feeds audio/video streams to the decoders
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __MPEGSTREAMPLAYER_H
#define __MPEGSTREAMPLAYER_H

#include "../decoder/decoderPlugin.h"
#include "../util/syncClock.h"

class MpegSystemHeader;

class MpegStreamPlayer {

 protected:
  SyncClock* syncClock;
  DecoderPlugin* audioDecoder;
  DecoderPlugin* videoDecoder;
  BufferInputStream* audioInput;
  BufferInputStream* videoInput;
  OutputStream* output;
  InputStream* input;
  TimeStamp* timeStampVideo;
  TimeStamp* timeStampAudio;

  int packetCnt;
  int audioPacketCnt;
  int videoPacketCnt;
  int seekPos;
  int writeToDisk;

  char* nukeBuffer;

 public:
  MpegStreamPlayer(InputStream* input,
		   OutputStream* output,
		   DecoderPlugin* audioDecoder,
		   DecoderPlugin* videoDecoder);
  ~MpegStreamPlayer();

  int isInit();
  void processThreadCommand(Command* command);

  //
  // Mpeg special functions [START]
  //

  int processSystemHeader(MpegSystemHeader* mpegSystemHeader);
  int insertAudioData(MpegSystemHeader* header,int len);
  int insertVideoData(MpegSystemHeader* header,int len);

  //
  // Mpeg special functions [END]
  //
 


  int insertAudioDataRaw(unsigned char* input,int len,TimeStamp* stamp);
  int insertVideoDataRaw(unsigned char* input,int len,TimeStamp* stamp);

  void dumpData(MpegSystemHeader* mpegSystemHeader);

  void setWriteToDisk(int lwriteToDisk);
  int getWriteToDisk();
  
  int hasEnd();
 protected:
  int processResyncRequest();

  int getByteDirect();
  void nuke(int byte);

  // useful methods
  int finishAudio(int len);
  int finishVideo(int len);

};

#endif
