/*
  audio wrapper for SDL
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */
#include "../../input/bufferInputStream.h"
#include <assert.h>
#include <iostream.h>
#if defined WIN32
#include <SDL.h>
#include <SDL_audio.h>
#else
#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>
#endif

//static  SDL_AudioSpec actual;
static  BufferInputStream* audioRing;
static  TimeStamp* dummy;
static  int lOpen=false;



int audioConstruct() {
  cout << "audioConstruct ********* SDL"<<endl;
  if ( SDL_Init(SDL_INIT_AUDIO) < 0 ) {
    fprintf(stderr, "Warning: Couldn't init SDL audio: %s\n",
	    SDL_GetError());
    exit(0);
  }
  atexit(SDL_Quit);
  audioRing=new BufferInputStream(1024*65,1024*8,"audioSDL");
  audioRing->open("audioSDL");
  dummy=new TimeStamp();
  lOpen=false;
  return true;
}


void audioDestruct() {
  delete audioRing;
  delete dummy;
}




int audioOpen() {
  return true;
}


void audioClose() {
  lOpen=false;
  SDL_CloseAudio();
}


void audioCallback(void *, Uint8 *stream, int len) {
  char* startPtr;
  TimeStamp* start;
  int bytePos;

  int read=audioRing->readRemote(&startPtr,len);
  SDL_MixAudio(stream, (Uint8*) startPtr, read, SDL_MIX_MAXVOLUME);
  
  audioRing->forwardReadPtr(read);
  // dequeue time stamps
  bytePos=audioRing->getBytePosition();
  start=audioRing->getTimeStamp(bytePos);

}



void audioInit(int sampleSize,int frequency, int stereo, int sign, int big) {
  if( sign == 0 )
  {
      fprintf(stderr,
              "%s, %d: expecting signed audio data, "
              "initialized unsigned (ignored)\n",
              __FILE__, __LINE__ );
  }
  if( big != 0 )
  {
      fprintf(stderr,
              "%s, %d: expecting little endian audio data, "
              "initialized big endian (ignored)\n",
              __FILE__, __LINE__ );
  }

  cout << "SDL audioInit: "
       << " sampleSize:"<<sampleSize
       << " frequency:"<<frequency
       << " stereo:"<<stereo<<endl;
  if (lOpen==true) {
    cout << "SDL is buggy, because open != init -> return"<<endl;
    return;
  }
  lOpen=true;
  SDL_AudioSpec wanted;
  //SDL_AudioSpec actual;
  if (sampleSize == 16) {
    wanted.format= AUDIO_S16LSB;
  } else {
    wanted.format= AUDIO_S8;
  }

  wanted.freq=frequency;
  wanted.channels=stereo+1;
  wanted.samples = 1024;
  wanted.callback = audioCallback;
  wanted.userdata = NULL;

  int err=SDL_OpenAudio(&wanted, NULL);
  if (err != 0) {
    cout << "SDL_OpenAudio not ok"<<endl;
    cout << "error is:"<<SDL_GetError()<<endl;
    exit(0);
  }
  SDL_PauseAudio(0);

}


int mixerOpen() {
  return true;
}


void mixerClose() {
}


void mixerSetVolume(int volumeLeft,int volumeRight) {
  cout << "volumeLeft:"<<volumeLeft
       << " volumeRight:"<<volumeRight<<endl;
}


int audioWrite(char *buffer, int count) {

  audioRing->write(buffer,count,dummy);

  
  return count;
}


int getAudioFd() {
  return false;
}


int getAudioBufferSize() {
  int buf=1024*65;
  return buf;
}
