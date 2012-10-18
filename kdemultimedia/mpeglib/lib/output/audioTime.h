/*
  defines the format of an audio stream
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __AUDIOTIME_H
#define __AUDIOTIME_H

#include "../util/timeWrapper.h"

class AudioTime  {

  int stereo;
  int samplesize;
  int speed;
  int _sign;
  int _bigendian;
  float time;

 public:
  AudioTime();
  ~AudioTime();

  //cd-quality:true,16,44100
  void setFormat(int stereo,int samplesize,int speed,int sign=true, int bigendian=false);

  int getStereo();
  int getSampleSize();
  int getSpeed();
  int getSign();
  int getBigendian();

  void setStereo(int stereo);
  void setSampleSize(int samplesize);
  void setSpeed(int speed);
  void setSign(int sign);
  void setBigendian(int bigendian);


  float getTime();
  void setTime(float time);
  void forwardTime(int bytes);

  // do not modify the internal time
  float calculateTime(int bytes);
  int calculateBytes(float time);

  void sleepWrite(int size);

  
  void copyTo(AudioTime* audioTime);
  void print();
  
};



#endif
