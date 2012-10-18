/*
  fifo for audioData
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "audioDataArray.h"

#include <iostream>

using namespace std;


AudioDataArray::AudioDataArray(int entries) {

  this->entries=entries;
  fillgrade=0;
  readPos=0;
  writePos=0;
  pcmSum=0;
  abs_thread_mutex_init(&writeInMut);
  abs_thread_mutex_init(&changeMut);

  audioDataArray=new AudioData*[entries];

  int i;
  for(i=0;i<entries;i++) {
    audioDataArray[i]=new AudioData();
  }

  abs_thread_mutex_init(&writeInMut);
  abs_thread_mutex_init(&changeMut);

}

AudioDataArray::~AudioDataArray() {

  int i;
  for(i=0;i<entries;i++) {
    delete audioDataArray[i];
  } 

  delete audioDataArray;

  abs_thread_mutex_destroy(&writeInMut);
  abs_thread_mutex_destroy(&changeMut);

}




void AudioDataArray::lockStampArray() {

  abs_thread_mutex_lock(&changeMut);
  abs_thread_mutex_lock(&writeInMut);
  abs_thread_mutex_unlock(&changeMut);

}


void AudioDataArray::unlockStampArray() {
  abs_thread_mutex_unlock(&writeInMut); 
}


void AudioDataArray::internalForward() {
  pcmSum=pcmSum-readAudioData()->getPCMLen();
  readPos++;
  fillgrade--;
  if (readPos == entries-1) {
    readPos=0;
  }
}

int AudioDataArray::getPCMSum() {
  return pcmSum;
}



int AudioDataArray::insertAudioData(AudioData* src) {
  lockStampArray();

  int back=true;
  src->copyTo(audioDataArray[writePos]);
  pcmSum+=src->getPCMLen();

  writePos++;
  fillgrade++;
  if (writePos == entries-1) {
    writePos=0;
  }
  if (fillgrade == entries) {
    cout <<" Audiodata::array overfull forward"<<endl;
    internalForward();
    back=false;
  }
  unlockStampArray();
  return back;
}


AudioData* AudioDataArray::readAudioData() {
  return audioDataArray[readPos];
}


int AudioDataArray::getFillgrade() {
  return fillgrade;
}


void AudioDataArray::forward() {
  lockStampArray();
  internalForward();
  unlockStampArray();
}


void AudioDataArray::clear() {
  lockStampArray();
  fillgrade=0;
  readPos=0;
  writePos=0;
  pcmSum=0;
  unlockStampArray();
}


