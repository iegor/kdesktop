/*
  fifo for audioData
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __AUDIODATAARRAY_H
#define __AUDIODATAARRAY_H

#include "../util/abstract/abs_thread.h"
#include "audioData.h"


class AudioDataArray {

  AudioData** audioDataArray;
  int fillgrade;
  int entries;
  
  int writePos;
  int readPos;
  int pcmSum;

 public:
  AudioDataArray(int entries);
  ~AudioDataArray();

  int insertAudioData(AudioData* src);
  AudioData* readAudioData();

  int getFillgrade();
  int getPCMSum();
  void forward();
  void clear();

 private:
  void lockStampArray();
  void unlockStampArray();
  void internalForward();
  
  abs_thread_mutex_t writeInMut;
  abs_thread_mutex_t changeMut;



};
#endif
