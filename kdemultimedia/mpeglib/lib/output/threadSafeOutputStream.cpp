/*
  thread safe wrapper for output Stream
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "threadSafeOutputStream.h"


ThreadSafeOutputStream::ThreadSafeOutputStream(OutputStream* output) {
  threadQueueAudio=new ThreadQueue();
  threadQueueVideo=new ThreadQueue();
  this->output=output;
}


ThreadSafeOutputStream::~ThreadSafeOutputStream() {
  delete threadQueueAudio;
  delete threadQueueVideo;
  delete output;
}

int ThreadSafeOutputStream::audioInit() {
  int back;
  threadQueueAudio->waitForExclusiveAccess();
  back=output->audioInit();
  threadQueueAudio->releaseExclusiveAccess();
  return back;
}


int ThreadSafeOutputStream::audioSetup(int freq,int stereo,
				       int sign,int big,int sampleSize) {
  int back;
  threadQueueAudio->waitForExclusiveAccess();
  back=output->audioSetup(freq,stereo,sign,big,sampleSize);
  threadQueueAudio->releaseExclusiveAccess();
  return back; 
}


int ThreadSafeOutputStream::audioPlay(TimeStamp* start,
				      TimeStamp* end,char* buf, int len) {
  int back;
  threadQueueAudio->waitForExclusiveAccess();
  back=output->audioPlay(start,end,buf,len);
  threadQueueAudio->releaseExclusiveAccess();
  return back;
}

void ThreadSafeOutputStream::audioFlush() {
  threadQueueAudio->waitForExclusiveAccess();
  output->audioFlush();
  threadQueueAudio->releaseExclusiveAccess(); 
}


void ThreadSafeOutputStream::audioClose() {
  threadQueueAudio->waitForExclusiveAccess();
  output->audioClose();
  threadQueueAudio->releaseExclusiveAccess();  
}

void ThreadSafeOutputStream::audioOpen() {
 threadQueueAudio->waitForExclusiveAccess();
 output->audioOpen();
 threadQueueAudio->releaseExclusiveAccess();    
}



int ThreadSafeOutputStream::getPreferredDeliverSize() {
  int back;
  threadQueueAudio->waitForExclusiveAccess();
  back=output->getPreferredDeliverSize();
  threadQueueAudio->releaseExclusiveAccess();
  return back;  
}

int ThreadSafeOutputStream::videoInit() {
  int back;
  threadQueueVideo->waitForExclusiveAccess();
  back=output->videoInit();
  threadQueueVideo->releaseExclusiveAccess();
  return back; 
}

int ThreadSafeOutputStream::openWindow(int w, int h,const char* title) {
  int back;
  threadQueueVideo->waitForExclusiveAccess();
  back=output->openWindow(w,h,title);
  threadQueueVideo->releaseExclusiveAccess();
  return back; 
}


void ThreadSafeOutputStream::closeWindow() {
  threadQueueVideo->waitForExclusiveAccess();
  output->closeWindow();
  threadQueueVideo->releaseExclusiveAccess();
}

void ThreadSafeOutputStream::flushWindow() {
  threadQueueVideo->waitForExclusiveAccess();
  output->flushWindow();
  threadQueueVideo->releaseExclusiveAccess(); 
} 


PictureArray* ThreadSafeOutputStream::lockPictureArray() {
  PictureArray* back;
  threadQueueVideo->waitForExclusiveAccess();
  back=output->lockPictureArray();
  threadQueueVideo->releaseExclusiveAccess();
  return back; 
}


void ThreadSafeOutputStream::unlockPictureArray(PictureArray* array) {
  threadQueueVideo->waitForExclusiveAccess();
  output->unlockPictureArray(array);
  threadQueueVideo->releaseExclusiveAccess();
}


int ThreadSafeOutputStream::getFrameusec() {
  int back;
  threadQueueVideo->waitForExclusiveAccess();
  back=output->getFrameusec();
  threadQueueVideo->releaseExclusiveAccess();
  return back;  
}

void ThreadSafeOutputStream::config(const char* key,
				    const char* value,void* user_data) {

  threadQueueVideo->waitForExclusiveAccess();
  threadQueueAudio->waitForExclusiveAccess();
  output->config(key,value,user_data);
  threadQueueVideo->releaseExclusiveAccess();
  threadQueueAudio->releaseExclusiveAccess();
  
}
 


