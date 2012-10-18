/*
  generic output class
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "outputStream.h"
#include "../util/mmx/mmx.h"

#include <iostream>

using namespace std;

OutputStream::OutputStream() {
  // we call mm_support() here because it is the only position
  // where we gurantee that not threads are
  // running (the call is not thread safe)
  // afer the call we never execute the asm part again
  // and everything is fine
  mm_support();
  abs_thread_mutex_init(&stateChangeMut);
  abs_thread_cond_init(&stateChangeCond);

  audioState=0;
  videoState=0;
  audioInit();
  videoInit();
}


OutputStream::~OutputStream() {
  audioInit();
  videoInit();
  abs_thread_cond_destroy(&stateChangeCond);
  abs_thread_mutex_destroy(&stateChangeMut);
}


int OutputStream::audioInit() {
  sendSignal(_STREAM_MASK_IS_INIT,false,_STREAMTYPE_AUDIO);
  sendSignal(_STREAM_MASK_IS_EOF,false,_STREAMTYPE_AUDIO);
  sendSignal(_STREAM_MASK_IS_DATA,false,_STREAMTYPE_AUDIO);
  return true;
}


int OutputStream::audioSetup(int ,int ,
			     int ,int ,int ) {
  sendSignal(_STREAM_MASK_IS_INIT,true,_STREAMTYPE_AUDIO);
  return true;
}


int OutputStream::audioPlay(TimeStamp* ,
			    TimeStamp* ,char* , int len) {
  sendSignal(_STREAM_MASK_IS_DATA,true,_STREAMTYPE_AUDIO);
  return len;
}

void OutputStream::audioFlush() {
  sendSignal(_STREAM_MASK_IS_EOF,true,_STREAMTYPE_AUDIO);
}


void OutputStream::audioClose() {
  cerr << "direct virtual call OutputStream::audioClose"<<endl;
  exit(0);
}

void OutputStream::audioOpen() {
  cerr << "direct virtual call OutputStream::audioOpen"<<endl;
  exit(0);
}



void OutputStream::sendSignal(int signal,int value,int streamType) {
  abs_thread_mutex_lock(&stateChangeMut);
  int* modifyState=NULL;
  switch(streamType) {
  case _STREAMTYPE_AUDIO:
    modifyState=&audioState;
    break;
  case _STREAMTYPE_VIDEO:
    modifyState=&videoState;
    break;
  default:
    cout << "unknown streamType:"<<streamType
	 <<" in OutputStream::sendSignal"<<endl;
    exit(0);
  }
  // should we set the bit?
  if (value==true) {
    // set it with "or"
    *modifyState|=signal;
  } else {
    // we should remove the bit
    // is it currently set?
    if (*modifyState & signal) {
      // remove it
      *modifyState-=signal;
    }
  }


  abs_thread_cond_signal(&stateChangeCond);
  abs_thread_mutex_unlock(&stateChangeMut);
 
}


int OutputStream::getPreferredDeliverSize() {
  cerr << "direct virtual call OutputStream::getPreferredDeliverSize()"<<endl;
  return 4096;
}

int OutputStream::videoInit() {
  sendSignal(_STREAM_MASK_IS_INIT,false,_STREAMTYPE_VIDEO);
  sendSignal(_STREAM_MASK_IS_EOF,false,_STREAMTYPE_VIDEO);
  sendSignal(_STREAM_MASK_IS_DATA,false,_STREAMTYPE_VIDEO);
  return true;
}

int OutputStream::openWindow(int , int ,const char* ) {
  sendSignal(_STREAM_MASK_IS_INIT,true,_STREAMTYPE_VIDEO);
  return true;
}

int OutputStream::x11WindowId() {
 cout << "direct virtual call OutputStream::x11WindowId()" << endl;
 return -1;
}

void OutputStream::closeWindow() {
  cerr << "direct virtual call OutputStream::closeWindow"<<endl;
  exit(0);
}

void OutputStream::flushWindow() {
  sendSignal(_STREAM_MASK_IS_EOF,true,_STREAMTYPE_VIDEO);
} 


PictureArray* OutputStream::lockPictureArray() {
  cerr << "direct virtual call OutputStream::lockPictureArray"<<endl;
  exit(0);
  return NULL;
}


void OutputStream::unlockPictureArray(PictureArray* ) {
  sendSignal(_STREAM_MASK_IS_DATA,true,_STREAMTYPE_VIDEO);
}





int OutputStream::getOutputInit() {
  cerr << "direct virtual call OutputStream::getOutputInit"<<endl;
  exit(0);
  return false;
}


void OutputStream::setOutputInit(int lInit) {
  cerr << "direct virtual call OutputStream::setOutputInit:"<<lInit<<endl;
  exit(0);
}



void OutputStream::writeInfo(PluginInfo* ) {

}


void OutputStream::config(const char* key,
			  const char* value,void* user_data) {
  cerr << "direct virtual call OutputStream::config"<<endl;
  printf("key:%s\n",key);
  printf("value:%s\n",value);
  printf("user_data:%p\n",user_data);
  exit(0);
}
 
int OutputStream::getFrameusec() {
  cerr << "direct virtual call OutputStream::getFrameusec"<<endl;
  return 0;
}
 


int OutputStream::waitStreamState(int method,int mask,int streamType) {

  int* waitState=NULL;
  switch(streamType) {
  case _STREAMTYPE_AUDIO:
    waitState=&audioState;
    break;
  case _STREAMTYPE_VIDEO:
    waitState=&videoState;
    break;
  default:
    cout << "unknown streamType:"<<streamType
	 <<" in OutputStream::waitStreamState"<<endl;
    exit(0);
  }

  if (method == _OUTPUT_WAIT_METHOD_BLOCK) {
    abs_thread_mutex_lock(&stateChangeMut);
    while ((*waitState &= mask)==0) {
      cout << "waitStreamState:"<<waitState<<endl;
      cout << "mask:"<<mask<<endl;
      abs_thread_cond_wait(&stateChangeCond,&stateChangeMut);
    }
    abs_thread_mutex_unlock(&stateChangeMut);
    return true;
  }
    
  int back=false;
  if (method == _OUTPUT_WAIT_METHOD_POLL) {
    abs_thread_mutex_lock(&stateChangeMut);
    back=*waitState;
    abs_thread_mutex_unlock(&stateChangeMut);
    return back;
  }    
  cout << " OutputStream::waitStreamState method not implemented"<<endl;
  exit(0);
  return 0; 
}
