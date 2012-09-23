/*
  queues audio frames in an IOQueue, allows streaming from frames
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __AUDIOFRAMEQUE_H
#define __AUDIOFRAMEQUE_H

#include "IOFrameQueue.h"
#include "floatFrame.h"
#include "pcmFrame.h"

#include <kdemacros.h>

/**
   This class solves the problem that we produce audioFrames
   in a packet, but often we want that these packets looks
   like a stream.
   This class can convert from packets back to a stream.
*/

class KDE_EXPORT AudioFrameQueue : public IOFrameQueue {
  
  int frameType;
  int len;
  AudioFrame* currentAudioFrame;
  int currentRead;

 public:
  AudioFrameQueue(int queueSize,int frameSize,int frameType);
  ~AudioFrameQueue();

  void emptyQueueEnqueue(AudioFrame* frame);
  AudioFrame* emptyQueueDequeue();

  void dataQueueEnqueue(AudioFrame* frame);
  AudioFrame* dataQueueDequeue();

  // Meta info about stream
  AudioFrame* getCurrent();
 
  // Data info about "stream" (calculated from the packets)
  int getLen();

  // copy from packets to destination
  int copy(float* left,float* right,int len);
  int copy(short int* left,short int* right,int len);
  void forwardStreamDouble(int len);

  int copy(short int* dest,int len);
  int copy(float* dest,int len);
  void forwardStreamSingle(int len);

  void clear();

 private:
  void transferFrame(float* left,float* right,FloatFrame*,int start,int len);
  void transferFrame(short int* left,short int* right,
		     PCMFrame*,int start,int len);
  void transferFrame(short int* dest,
		     PCMFrame*,int start,int len);
  void transferFrame(float* dest,
		     FloatFrame*,int start,int len);
  int copygeneric(char* left,char* right,int wantLen,int version,int mux);
};
#endif
