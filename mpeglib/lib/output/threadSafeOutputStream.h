/*
  thread safe wrapper for output Stream
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#ifndef __THREADSAFEOUTPUTSTREAM_H
#define __THREADSAFEOUTPUTSTREAM_H

// read INTRO in threadQueue.h
// This class makes the outputStream (given in constructor)
// threadsafe by wrapping each call with a threadqueue.
//
// Important NOTE: the output pointer is the owned by this class !!!
//                 which means: we call delete on it!
// More Important NOTES:
//
// We make the audio and video calls seperate threadsafe
// and ONLY the config call threadsafe to both!

#include "../util/abstract/threadQueue.h"
#include "outputStream.h"


class ThreadSafeOutputStream : public OutputStream {

  ThreadQueue* threadQueueAudio;
  ThreadQueue* threadQueueVideo;
  OutputStream* output;

 public:
  ThreadSafeOutputStream(OutputStream* output);
  ~ThreadSafeOutputStream();

  // Thread safe Audio Stream handling
  int audioInit();
  int audioSetup(int freq,int stereo,int sign,int big,int sampleSize);
  int audioPlay(TimeStamp* startStamp,
		TimeStamp* endStamp,char *buffer, int size);  
  void audioFlush();
  void audioClose();
  void audioOpen();
  int getPreferredDeliverSize();
  
  
  // Video Output
  int videoInit();
  int openWindow(int width, int height,const char *title);
  void closeWindow();
  void flushWindow();
  PictureArray* lockPictureArray();
  void unlockPictureArray(PictureArray* pictureArray);
  int getFrameusec();
  
  // config Output
  void config(const char* key, 
	      const char* value,void* user_data);


};
#endif
