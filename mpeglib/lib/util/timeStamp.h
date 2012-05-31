/*
  class for managing byte positions and associated time positions
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __TIMESTAMP_H
#define __TIMESTAMP_H


#include "syncClock.h"


class TimeStamp {

  long key;
  int keylen;

  int videoFrameCounter;

  timeval_t time;

  int lPTSFlag;

  double ptsTimeStamp;
  double scrTimeStamp;
  double dtsTimeStamp;

  class SyncClock* syncClock;

 public:
  TimeStamp();
  ~TimeStamp();
  
  void gettimeofday();
  void set(long sec,long usec);

  void addOffset(TimeStamp* stamp);
  void addOffset(int sec,long usec);
  void copyTo(TimeStamp* dest);

  long getKey();
  int getKeyLen();
  void setKey(long key,int keylen);

  int getPTSFlag();
  void setPTSFlag(int lPTSFlag);
 

  double getPTSTimeStamp();
  void setPTSTimeStamp(double ptsTimeStamp);

  double getDTSTimeStamp();
  void setDTSTimeStamp(double dtsTimeStamp);

  double getSCRTimeStamp();
  void setSCRTimeStamp(double scrTimeStamp);

  int getVideoFrameCounter();
  void setVideoFrameCounter(int nr);

  SyncClock* getSyncClock();
  void setSyncClock(class SyncClock* syncClock);


  void print(const char* name);
  void minus(TimeStamp* stamp,TimeStamp* dest);
  void minus(int sec,long usec);
  int lessThan(TimeStamp* stamp);
  int lessThan(int sec,long usec);
  double getAsSeconds();

  void waitForIt();
  timeval_t* getTime();

  int isPositive();
  int isNegative();

 private:
  void setTime(timeval_t* newTime);
  void normalize();


};
#endif
