/*
  defines the format of an audio stream
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "audioTime.h"

#include <iostream>

using namespace std;


AudioTime::AudioTime(){
  time=0.0;
}


AudioTime::~AudioTime() {
}


void AudioTime::setFormat(int stereo,int samplesize,int speed,int sign, int bigendian) {
  setStereo(stereo);
  setSampleSize(samplesize);
  setSpeed(speed);
  setSign(sign);
  setBigendian(bigendian);
}


int AudioTime::getStereo() {
  return stereo;
}


int AudioTime::getSampleSize() {
  return samplesize;
}


int AudioTime::getSpeed() {
  return speed;
}

int AudioTime::getSign() {
  return _sign;
}

int AudioTime::getBigendian() {
  return _bigendian;
}

void AudioTime::setStereo(int stereo) {
  this->stereo=stereo;
}


void AudioTime::setSampleSize(int samplesize) {
  this->samplesize=samplesize;
}


void AudioTime::setSpeed(int speed) {
  this->speed=speed;
}

void AudioTime::setSign(int sign) {
  this->_sign=sign;
}

void AudioTime::setBigendian(int bigendian) {
  this->_bigendian = bigendian;
}

float AudioTime::getTime() {
  return time;
}


void AudioTime::setTime(float time) {
  this->time=time;
}


void AudioTime::forwardTime(int bytes){
  time+=calculateTime(bytes);
}


float AudioTime::calculateTime(int bytes) {
  float back=0;

  bytes=bytes/(samplesize/8);
  if (stereo==1) {
    bytes=bytes/2;
  }
  if (speed != 0) {
    back=(float)bytes/(float)speed;
  }
  return back;
}


/**
   How much byte we need for time usecs?
*/
int AudioTime::calculateBytes(float time) {
  float back=time;

  if (speed != 0) {
    back=back*(float)speed;
  }
  back=back*(float)(samplesize/8);
  if (stereo==1) {
    back=back*2.0;
  }
  return (int)back;
}



void AudioTime::sleepWrite(int size) {
  timeval_t time;
  float timeLength=calculateTime(size);
  time.tv_sec=(long)timeLength;
  time.tv_usec=(long)(1000000*(timeLength-time.tv_sec));
  TimeWrapper::usleep(&time);
}


void AudioTime::print() {
  cout << "AudioTime-begin-"<<endl;
  cout << "stereo:"<<getStereo()<<" sampleSize:"<<getSampleSize()
       << " speed: "<<getSpeed()<<endl;

  cout << "AudioTime-end-"<<endl;
 
}


void AudioTime::copyTo(AudioTime* dest) {
  dest->setStereo(getStereo());
  dest->setSampleSize(getSampleSize());
  dest->setSpeed(getSpeed());
}

