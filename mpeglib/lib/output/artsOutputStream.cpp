/*
  output to arts
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "artsOutputStream.h"
#include "windowOut.h"
#include "avSyncer.h"
#include "../util/abstract/threadQueue.h"


ArtsOutputStream::ArtsOutputStream(void (*streamStateChangeCallback)(void*)) {
  
  audioTime=new AudioTime();
  x11Window=new WindowOut();
  privateBufferSize=1024*32;

  // we can dynamically change the buffer size,
  // over a config switch in avSyncer
  // but for now this should be ok
  // Arts must pass the total size of the audiobuffer
  // from /dev/dsp + connected nodes
  // or arts need a video interface
  stream=new BufferInputStream(privateBufferSize,1024*64,"artsLoopback");
  avSyncer=new AVSyncer(privateBufferSize);

  // with the ThreadQueue we protect all calls to the
  // x11 output. This is necessary, because arts may call
  // in the future "config" entries to switch the video 
  // mode (full,double,desktop)
  threadQueue=new ThreadQueue();
  // we set it, but its never used !
  this->streamStateChangeCallback=streamStateChangeCallback;
}


ArtsOutputStream::~ArtsOutputStream() {
  delete stream;
  delete audioTime;
  delete x11Window;
  delete avSyncer;
  delete threadQueue;
}




int ArtsOutputStream::audioSetup(int freq,int stereo,
				 int sign,int big,int sampleSize) {
  audioTime->setFormat(stereo,sampleSize,freq,sign,big);
  avSyncer->audioSetup(freq,stereo,sign,big,sampleSize);

  OutputStream::audioSetup(freq,stereo,sign,big,sampleSize);
  
  return true;
}

AudioTime* ArtsOutputStream::getAudioTime() {
  return audioTime;
}


void ArtsOutputStream::audioClose() {
  audioTime->setTime(0.0);
  // if we close this stream the thread is kicked out.
  stream->close();
  stream->clear();
  avSyncer->audioClose();
}

void ArtsOutputStream::audioFlush() {
  OutputStream::audioFlush();
  audioClose();
}


void ArtsOutputStream::audioOpen() {
  audioTime->setTime(0.0);
  // if we close this stream the thread is kicked out.
  stream->open("artsLoopback");
}


int ArtsOutputStream::audioPlay(TimeStamp* startStamp,
				TimeStamp* endStamp,char *buffer, int size) {
  int write=size;


  // we call this for stream state handling
  OutputStream::audioPlay(startStamp,endStamp,buffer,size);

  // store sync information
  avSyncer->audioPlay(startStamp,endStamp,buffer,size);
  

  // here we simulate our own blocking audio device
  if (stream->isOpen()==false) {
    audioTime->sleepWrite(size);
  } else {
    write=stream->write(buffer,size,startStamp);
  }
  return write;

}


int ArtsOutputStream::read(char** buffer,int bytes) {
  int back=stream->readRemote(buffer,bytes);
  return back;
}


void ArtsOutputStream::forwardReadPtr(int bytes) {
  audioTime->forwardTime(bytes);
  stream->forwardReadPtr(bytes);

}




int ArtsOutputStream::getPreferredDeliverSize() {
  return avSyncer->getPreferredDeliverSize();
}


int ArtsOutputStream::openWindow(int width, int height,const char *title) {
  threadQueue->waitForExclusiveAccess();
  int back=x11Window->openWindow(width,height,title);
  threadQueue->releaseExclusiveAccess();
  
  return back;
}

int ArtsOutputStream::x11WindowId() {
 return x11Window->x11WindowId();
}

void ArtsOutputStream::closeWindow() {
  threadQueue->waitForExclusiveAccess();
  x11Window->closeWindow();
  threadQueue->releaseExclusiveAccess();
}

void ArtsOutputStream::flushWindow() {
  threadQueue->waitForExclusiveAccess();
  x11Window->flushWindow();
  threadQueue->releaseExclusiveAccess();
}


PictureArray* ArtsOutputStream::lockPictureArray() {
  PictureArray* back;
  threadQueue->waitForExclusiveAccess();
  back=x11Window->lockPictureArray();
  threadQueue->releaseExclusiveAccess();
  return back;
}


void ArtsOutputStream::unlockPictureArray(PictureArray* pictureArray) {


  YUVPicture* pic=pictureArray->getYUVPictureCallback();
  if (avSyncer->syncPicture(pic)==false) {
    return;
  }
  threadQueue->waitForExclusiveAccess();
  x11Window->unlockPictureArray(pictureArray);
  threadQueue->releaseExclusiveAccess();

}

int ArtsOutputStream::getBufferFillgrade() {
  return stream->getFillgrade();
}

int ArtsOutputStream::getFrameusec() {
  return avSyncer->getFrameusec();
}

  

void ArtsOutputStream::setAudioBufferSize(int size) {
  avSyncer->setAudioBufferSize(privateBufferSize+size);
}


void ArtsOutputStream::config(const char* key,const char* value,
			      void* user_data) {
  
  threadQueue->waitForExclusiveAccess();
  avSyncer->config(key,value,user_data);
  x11Window->config(key,value,user_data);
  threadQueue->releaseExclusiveAccess();
}
