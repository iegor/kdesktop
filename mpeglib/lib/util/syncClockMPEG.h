/*
  synchronisation of audio/video (PTS) against system clock stamps (SCR)
  Copyright (C) 2000 Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __SYNCCLOCKMPEG_H
#define __SYNCCLOCKMPEG_H


#include "syncClock.h"

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

class SyncClockMPEG : public SyncClock {   

  int syncMode;
  double lastSCR;
  double lastPTS;
  double jitter;
  double oldjitter;
  timeval_t lastSCR_time;
  timeval_t lastPTS_time;
  timeval_t drift_time;

 public:
  SyncClockMPEG();
  ~SyncClockMPEG();

  int getSyncMode();
  void setSyncMode(int syncMode);

  
  int syncAudio(double pts,double scr);
  int syncVideo(double pts,double scr,
		class TimeStamp* earlyTime,class TimeStamp* waitTime);

  void print(char* text);

  double getPTSTime(double* window);
 private:

  void printTime(timeval_t* a,char* text);
  void markLastPTSTime(double scr,double pts);
  int gowait(double scr,double pts,
	     class TimeStamp* earlyTime,class TimeStamp* waitTime);
  int a_Minus_b_Is_C(timeval_t* a,timeval_t* b,timeval_t* c);
  double timeval2Double(timeval_t* a);
  void double2Timeval(double a,timeval_t* dest);

  void lockSyncClock();
  void unlockSyncClock();

  abs_thread_mutex_t scrMut; 
  abs_thread_mutex_t changeMut; 


};
#endif

