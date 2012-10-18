/*
  encapsulates the syncing methods, to use it in other classes
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __AVSYNCER_H
#define __AVSYNCER_H

#include <stdlib.h>

#include "../util/render/yuvPicture.h"
#include "../util/syncClock.h"
#include "audioTime.h"

class Performance;
class AudioDataArray;
class AudioData;

class AVSyncer {

  AudioData* audioDataInsert;
  AudioData* audioDataCurrent;
  AudioDataArray* audioDataArray;
  Performance* performance;

  AudioTime* audioTime;
  int onePicFrameInAudioBytes;
  int oneFrameTime;

  int lAudioRunning;

  abs_thread_mutex_t writeInMut;
  abs_thread_mutex_t changeMut;


  int bufferSize;

  TimeStamp* startAudio;
  TimeStamp* endAudio;
  
  TimeStamp* videoTimeStamp;
  TimeStamp* diffTime;
  TimeStamp* waitTime;

  int lPerformance;
  int lavSync;
  int lastAudioPacket;
  double pts_jitter;


 public:
  AVSyncer(int bufferSize);
  ~AVSyncer();

  // audio
  int audioSetup(int frequency,int stereo,int sign,int big,int sixteen);
  int audioPlay(TimeStamp* startStamp,
		TimeStamp* endStamp,char *buffer, int size);
  void audioClose(void);
  void setAudioBufferSize(int size);

  int getPreferredDeliverSize();

  // video
  int getFrameusec();
  int syncPicture(YUVPicture* syncPic);

  void config(const char* key,const char* value,void* user_data);



 private:
  int getAudioRunning();
  void setAudioRunning(int lAudioRunning);

  void lockSyncData();
  void unlockSyncData();
  void setAudioSync(AudioData* audioData);
 
   // methods which belong not to the OutputStream interface
  int avSync(TimeStamp* startVideoStamp,
	     TimeStamp* waitTime,
	     TimeStamp* earlyTime,
	     float picPerSec);

};
#endif
