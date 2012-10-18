/*
  class for managing byte positions and associated time positions
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "timeStamp.h"

#include <iostream>

using namespace std;


TimeStamp::TimeStamp() {
  key=0;
  keylen=0;
  time.tv_sec=0;
  time.tv_usec=0;

  ptsTimeStamp=0.0;
  scrTimeStamp=0.0;
  videoFrameCounter=0;
  dtsTimeStamp=0.0;
  lPTSFlag=false;

}


TimeStamp::~TimeStamp() {
}


void TimeStamp::copyTo(TimeStamp* dest) {
  dest->setKey(key,keylen);
  dest->setTime(&time);
  dest->setPTSTimeStamp(ptsTimeStamp);
  dest->setVideoFrameCounter(videoFrameCounter);

  dest->setSCRTimeStamp(scrTimeStamp);
  dest->setDTSTimeStamp(dtsTimeStamp);
  dest->setSyncClock(syncClock);
  dest->setPTSFlag(lPTSFlag);
}


long TimeStamp::getKey() {
  return key;
}

int TimeStamp::getKeyLen() {
  return keylen;
}

void TimeStamp::setKey(long key,int len) {
  this->key=key;
  this->keylen=len;
}


void TimeStamp::setTime(timeval_t* newTime) {
  time.tv_sec=newTime->tv_sec;
  time.tv_usec=newTime->tv_usec;
}


void TimeStamp::gettimeofday() {
  TimeWrapper::gettimeofday(&time);
}


timeval_t* TimeStamp::getTime() {
  return &time;
}


void TimeStamp::waitForIt() {
  timeval_t waitTime;

  waitTime.tv_sec=time.tv_sec;
  waitTime.tv_usec=time.tv_usec;

  if (isPositive()) {
    //cout << "wait:"<<waitTime.tv_sec<<" usec:"<<waitTime.tv_usec<<endl;
    TimeWrapper::usleep(&waitTime);
  }
    
}

void TimeStamp::addOffset(TimeStamp* stamp) {
  addOffset(stamp->time.tv_sec,stamp->time.tv_usec);
}

void TimeStamp::addOffset(int sec,long usec) {
  time.tv_sec=time.tv_sec+sec;
  time.tv_usec=time.tv_usec+usec;
  if (time.tv_usec >= 1000000) {
    time.tv_usec-=1000000;
    time.tv_sec++;
  }
  if (time.tv_usec < 0) {
    time.tv_usec+=1000000;
    time.tv_sec--;   
  }  
}

void TimeStamp::minus(int sec,long usec) {

  time.tv_usec-=usec;
  time.tv_sec-=sec;
  if (time.tv_usec <= 0) {
    time.tv_usec=time.tv_usec+1000000;
    time.tv_sec--;
  }
  if (time.tv_usec >= 1000000) {
    time.tv_usec=time.tv_usec-1000000;
    time.tv_sec++;
  }  

}

void TimeStamp::minus(TimeStamp* stamp,TimeStamp* dest) {
  
  long sec=time.tv_sec;
  long usec=time.tv_usec;

  minus(stamp->time.tv_sec,stamp->time.tv_usec);

  dest->set(time.tv_sec,time.tv_usec);
  if (dest != this) {
    time.tv_sec=sec;
    time.tv_usec=usec;
  }
}


int TimeStamp::lessThan(TimeStamp* stamp) {
  return lessThan(stamp->time.tv_sec,stamp->time.tv_usec);
}


int TimeStamp::lessThan(int sec,long usec) {
  int back=false;
  if (time.tv_sec < sec) {
    back=true;
  } else {
    if (time.tv_sec == sec) {
      if (time.tv_usec < usec) {
	back=true;
      }
    }
  }
  return back;
}
 
void TimeStamp::set(long sec,long usec) {
  time.tv_sec=sec;
  time.tv_usec=usec;
}



void TimeStamp::print(const char* name) {
  cout << name
       <<" lPTS:"<<lPTSFlag
       <<" pts:"<<ptsTimeStamp
       <<" dts:"<<dtsTimeStamp
       <<" scr:"<<scrTimeStamp
       <<" key:"<<key
       <<" sec:"<<time.tv_sec
       <<" usec:"<<time.tv_usec
       <<" v-Minor:"<<videoFrameCounter<<endl;


}





int TimeStamp::isPositive() {
  if ((time.tv_sec == 0) && (time.tv_usec == 0)) {
    return false;
  }
  return ((time.tv_sec >= 0) && (time.tv_usec >= 0));
}


int TimeStamp::isNegative() {
  if (time.tv_sec < 0) {
    return true;
  }
  if (time.tv_usec < 0) {
    return true;
  }
  return false;
}


int TimeStamp::getPTSFlag() {
  return lPTSFlag;
}

void TimeStamp::setPTSFlag(int lPTSFlag) {
  this->lPTSFlag=lPTSFlag;
}
 
double TimeStamp::getPTSTimeStamp() {
  return ptsTimeStamp;
}


void TimeStamp::setPTSTimeStamp(double ptsTimeStamp) {
  this->ptsTimeStamp=ptsTimeStamp;
}
 

double TimeStamp::getSCRTimeStamp() {
  return scrTimeStamp;
}


void TimeStamp::setSCRTimeStamp(double scrTimeStamp) {
  this->scrTimeStamp=scrTimeStamp;
}


double TimeStamp::getDTSTimeStamp() {
  return dtsTimeStamp;
}


void TimeStamp::setDTSTimeStamp(double dtsTimeStamp) {
  this->dtsTimeStamp=dtsTimeStamp;
}

int TimeStamp::getVideoFrameCounter() {
  return videoFrameCounter;
}


void TimeStamp::setVideoFrameCounter(int nr) {
  this->videoFrameCounter=nr;
}



double TimeStamp::getAsSeconds() {
  return (double)time.tv_sec+(double)time.tv_usec/(double)1000000;
}

  

SyncClock* TimeStamp::getSyncClock() {
  return syncClock;
}

void TimeStamp::setSyncClock(SyncClock* syncClock) {
  this->syncClock=syncClock;
}







