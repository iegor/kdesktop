/*
  describes a paket oriented audioData, because Im fed up
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#include "audioData.h"

#include <iostream>

using namespace std;

AudioData::AudioData() {
  pcmLen=0;
  audioTime=new AudioTime();
  start=new TimeStamp();
  end=new TimeStamp();
  writeStamp=new TimeStamp();
}


AudioData::~AudioData() {
  delete audioTime;
  delete start;
  delete end;
  delete writeStamp;
}
 

void AudioData::copyTo(AudioData* dest) {
  dest->setAudioTime(getAudioTime());
  dest->setStart(getStart());
  dest->setEnd(getEnd());
  dest->setWrite(getWrite());
  dest->setPCMLen(getPCMLen());
}


int AudioData::getPCMLen(){
  return pcmLen;
}

void AudioData::setPCMLen(int pcmLen) {
  this->pcmLen=pcmLen;
}

void AudioData::setAudioTime(AudioTime* aTime) {
  aTime->copyTo(audioTime);
}

AudioTime* AudioData::getAudioTime() {
  return audioTime;
}


void AudioData::setStart(TimeStamp* s) {
  s->copyTo(start);
}


TimeStamp* AudioData::getStart() {
  return start;
}

void AudioData::setEnd(TimeStamp* e) {
  e->copyTo(end);
}

  
TimeStamp* AudioData::getEnd() {
  return end;
}

void AudioData::setWrite(TimeStamp* e) {
  e->copyTo(writeStamp);
}


TimeStamp* AudioData::getWrite() {
  return writeStamp;
}



void AudioData::print() {
  cout << "AudioData::print [START]"<<endl;
  start->print("audioData start");
  end->print("audioData end");
  cout << "pcmlen:"<<pcmLen<<endl;
  cout << "AudioData::print [END]"<<endl;
}
