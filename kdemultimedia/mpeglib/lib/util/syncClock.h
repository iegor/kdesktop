/*
  basic synchronisation Classs
  Copyright (C) 2000 Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __SYNCCLOCK_H
#define __SYNCCLOCK_H

#include "timeWrapper.h"
#include "abstract/abs_thread.h"
#include "timeStamp.h"
#include <errno.h>

#define __SYNC_NONE      0
#define __SYNC_AUDIO     1
#define __SYNC_VIDEO     2
#define __SYNC_BOTH      3


/**
   The idea is similar to this:
   We start a clock and selext a synchronisation mode.
   (AUDIO,VIDEO,BOTH,NONE)
   Assume we select AUDIO. When the audio thread delivers an SCR
   and an PTS, we set the SCR from this set, as the new
   time reference fo rthis clock.
   If a video thread enters the class, with an SCR,PTS we wait,
   or directly return if the PTS is in time or out of time,
   but we never set the SCR.
*/

class SyncClock {   


 public:
  SyncClock();
  virtual ~SyncClock();

  virtual int getSyncMode();
  virtual void setSyncMode(int syncMode);

  
  virtual int syncAudio(double pts,double scr);
  virtual int syncVideo(double pts,double scr,
			class TimeStamp* earlyTime,
			class TimeStamp* waitTime);

  virtual void print(char* text);


};
#endif
