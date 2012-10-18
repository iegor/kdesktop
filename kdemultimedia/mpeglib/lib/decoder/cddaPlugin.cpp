/*
  splay player plugin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "cddaPlugin.h"

#ifdef CDDA_PARANOIA

#include <sys/types.h>

#include <iostream>

using namespace std;

typedef int16_t size16;
typedef int32_t size32;

extern "C" {
#include <cdda_interface.h>
#include <cdda_paranoia.h>
}


CDDAPlugin::CDDAPlugin() {
}


CDDAPlugin::~CDDAPlugin() {
}


// here we can config our decoder with special flags
void CDDAPlugin::config(const char* key,const char* value,void* user_data) {
  DecoderPlugin::config(key,value,user_data);
}



void CDDAPlugin::decoder_loop() {

  if (input == NULL) {
    cout << "CDDAPlugin::decoder_loop input is NULL"<<endl;
    exit(0);
  }
  if (output == NULL) {
    cout << "CDDAPlugin::decoder_loop output is NULL"<<endl;
    exit(0);
  }
  // init decoder
  output->audioInit();
  TimeStamp* stamp;
  char buf[CD_FRAMESIZE_RAW*4];
  int len=0;
  // start decoding
  while(runCheck()) {
    
    switch(streamState) {
    case _STREAM_STATE_FIRST_INIT :
      output->audioSetup(44100,1,0,0,16);
      output->audioOpen();
      setStreamState(_STREAM_STATE_PLAY);
      len=getTotalLength();
      pluginInfo->setLength(len);
      output->writeInfo(pluginInfo);
      break;
    case _STREAM_STATE_INIT :
      setStreamState(_STREAM_STATE_PLAY);
      break;
    case _STREAM_STATE_PLAY : {
      // decode 
      int read=input->read(buf,2*CD_FRAMESIZE_RAW);
      int pos=input->getBytePosition();
      stamp=input->getTimeStamp(pos);
      output->audioPlay(stamp,stamp,buf,read);
      break;
    }
    case _STREAM_STATE_WAIT_FOR_END:
      // exit while loop
      lDecoderLoop=false;
      break;
    default:
      cout << "unknown stream state:"<<streamState<<endl;
    }
  }
  // remove decoder
  
  output->audioFlush();
}

// splay can seek in streams
int CDDAPlugin::seek_impl(int second) {
  int bytes_per_second;
  int seconds;
  int back;
  float frequence=44100;
  int bits=16;
  int channels=2;  

  bytes_per_second = (int)(frequence * channels * (bits / 8));


  
  seconds = bytes_per_second * second;
  cout << "seek to :"<<seconds<<endl;
  back=input->seek(seconds);

  return true;
}


int CDDAPlugin::getTotalLength() {
  shutdownLock();
  float wavfilesize;
  int back=0;
  float frequence=44100;
  if (input == NULL) {
    shutdownUnlock();
    return 0;
  }
  wavfilesize = input->getByteLength();
  int bits=16;
  int channels=2;
  if (bits == 16) {
    wavfilesize=wavfilesize/2;
  }
  if (channels==2) {
    wavfilesize=wavfilesize/2;
  }
  if (frequence != 0) {
    back=(int)(wavfilesize/frequence);
  }
  shutdownUnlock();
  return back;
}




#endif
//CDDA_PARANOIA

