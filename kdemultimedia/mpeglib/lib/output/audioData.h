/*
  describes a paket oriented audioData, because Im fed up
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __AUDIODATA_H
#define __AUDIODATA_H

#include "../util/timeStamp.h"
#include "audioTime.h"


#define _AUDIODATA_MAX_SIZE      8192


/**
   My attempt to encapsulate all this stupid things which deals
   with timing, synchronisation, length, stamps and everything
   unimaginable else.
*/



class AudioData {


  AudioTime* audioTime;
  TimeStamp* start;
  TimeStamp* end;
  TimeStamp* writeStamp;
 
  int pcmLen;

 public:
  AudioData();
  ~AudioData();
  
  void copyTo(AudioData* dest);

  
  int getPCMLen();
  void setPCMLen(int pcmLen);

  void setAudioTime(AudioTime* audioTime);
  AudioTime* getAudioTime();

  void setStart(TimeStamp* start);
  TimeStamp* getStart();
  
  void setWrite(TimeStamp* writeStamp);
  TimeStamp* getWrite();
  

  void setEnd(TimeStamp* end);
  TimeStamp* getEnd();

  void print();
};
#endif



