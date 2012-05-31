/*
  class for managing byte positions and associated time positions
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __TIMESTAMPARRAY_H
#define __TIMESTAMPARRAY_H

#include "abstract/abs_thread.h"
#include "timeStamp.h"


/**
   This class deals with the problem to sync audio and video.
   Both streams are decoded in different threads, sometimes
   the video is decoded faster than the audio and sometimes
   not.
   <p>
   You need a general mechanism to decide, which is faster.
   It works like this:
   <p>
   When the mpeg stream is split in video/audio part the split thread
   writes the video/audio data to the inputInterface.
   Additionally it writes a timestamp to the interface.
   The interface counts the bytes and forward the bytes/timeStamp
   pait to this class.
   Later when the threads write to the outputInterface the ask
   this class (with the bytePostions) which timestamp it
   has and hass the data and the timestamp to the outputInterface.
   There we can decide what to do with the data.
   <p>
   1) audio faster than video = drop video picture
   <p>
   2) video faster than audio - wait for audio.
*/



class TimeStampArray {

  TimeStamp** tStampArray;

  int lastWritePos;
  int writePos;
  int readPos;
  int fillgrade;
  char* name;
  int entries;
  
  abs_thread_mutex_t writeInMut;
  abs_thread_mutex_t changeMut;


 public:
  TimeStampArray(char* name,int entries);
  ~TimeStampArray();


  int insertTimeStamp(TimeStamp* src,long key,int len);
  TimeStamp*  getReadTimeStamp();
  TimeStamp*  getTimeStamp(long key);
  int getFillgrade();
  void forward();
  void clear();

  int bytesUntilNext(long key);

 private:
  void lockStampArray();
  void unlockStampArray();
  void internalForward();
  


};
#endif

