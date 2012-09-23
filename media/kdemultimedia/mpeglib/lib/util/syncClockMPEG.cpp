/*
  synchronisation of audio/video (PTS) against system clock stamps (SCR)
  Copyright (C) 2000 Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "syncClockMPEG.h"


#include <iostream>

using namespace std;


SyncClockMPEG::SyncClockMPEG() {
  syncMode=__SYNC_NONE;
  abs_thread_mutex_init(&scrMut);
  abs_thread_mutex_init(&changeMut);
  lastSCR=0.0;
  lastPTS=0.0;
  jitter=0.0;
  oldjitter=0.0;

  TimeWrapper::gettimeofday(&lastPTS_time);


}


SyncClockMPEG::~SyncClockMPEG() {
  abs_thread_mutex_destroy(&changeMut);
  abs_thread_mutex_destroy(&scrMut);
}



int SyncClockMPEG::getSyncMode() {
  return syncMode;
}


void SyncClockMPEG::setSyncMode(int syncMode) {
  this->syncMode=syncMode;
}



int SyncClockMPEG::syncAudio(double pts,double scr) {
  switch(syncMode) {
  case __SYNC_AUDIO:
    markLastPTSTime(scr,pts);
    break;
  default:
    cout << "syncMode not implemented:"<<syncMode<<endl;
  }
  return true;
}


int SyncClockMPEG::syncVideo(double pts,double scr,
			     TimeStamp* earlyTime,
			     TimeStamp* waitTime) {
  int back;
  switch(syncMode) {
  case __SYNC_AUDIO:
    back=gowait(scr,pts,earlyTime,waitTime);
    return back;
    break;
  case __SYNC_NONE:  
    return true;
  default:
    cout << "syncMode not implemented"<<endl;
  } 
  return true;
}


void SyncClockMPEG::lockSyncClock() {
  abs_thread_mutex_lock(&changeMut);
  abs_thread_mutex_lock(&scrMut);
  abs_thread_mutex_unlock(&changeMut);
  
}


void SyncClockMPEG::unlockSyncClock() {
  abs_thread_mutex_unlock(&scrMut);
}


double SyncClockMPEG::getPTSTime(double* window) {
  lockSyncClock();
  double currentPTS;
  timeval_t current_time;
  timeval_t passed_time;
  TimeWrapper::gettimeofday(&current_time);

  a_Minus_b_Is_C(&current_time,&lastPTS_time,&passed_time);
  currentPTS=lastPTS+timeval2Double(&passed_time);
  *window=jitter+oldjitter;


  unlockSyncClock();
  return currentPTS;
}


void SyncClockMPEG::markLastPTSTime(double ,double pts) {
  double tmp;
  double expect_time=getPTSTime(&tmp);

  lockSyncClock();
  oldjitter=jitter;
  jitter=expect_time-pts;

  TimeWrapper::gettimeofday(&lastPTS_time);
  lastPTS=pts;


  unlockSyncClock();
  /*
  cout << "(audio) pts stream:"<<pts
       << " expect:"<<expect_time<<" jitter:"<<jitter<<endl;
  */

}


int SyncClockMPEG::a_Minus_b_Is_C(timeval_t* a,timeval_t* b,timeval_t* c){
  c->tv_usec=a->tv_usec;
  c->tv_sec=a->tv_sec;

  c->tv_usec-=b->tv_usec;
  c->tv_sec-=b->tv_sec;

  if (c->tv_usec <= 0) {
    c->tv_usec=c->tv_usec+1000000;
    c->tv_sec--;
  }
  if (c->tv_usec >= 1000000) {
    c->tv_usec=c->tv_usec-1000000;
    c->tv_sec++;
  }  
  return true;
}


double SyncClockMPEG::timeval2Double(timeval_t* a) {
  return (double)a->tv_sec+(double)(a->tv_usec)/1000000.0;
}


void SyncClockMPEG::double2Timeval(double a,timeval_t* dest) {
  dest->tv_sec=(int)a;
  dest->tv_usec=(int)(1000000.0*(double)(a-(double)dest->tv_sec));
}


int SyncClockMPEG::gowait(double ,double pts,
			  TimeStamp* earlyTime,TimeStamp* ) {
  double window;
  double currentPTS=getPTSTime(&window);
  double diff_time;

  // in window we have the jitter
  // diff_time is positive when we are in the future
  diff_time=pts-(currentPTS+window);

    

  // tolerate one frame (for 25 frames/s) in the future
  // but wait a bit
  if (diff_time > 0.0) {
    diff_time=diff_time/4.0;
    
    double2Timeval(diff_time,earlyTime->getTime());
    if (diff_time > 1) {
      // cannot be, we assume stange clock error
      earlyTime->set(1,0);
    }
    
    return true;
  }

  earlyTime->set(0,0);
  // one frame late, display it, without waiting
  // and hope we catch up
  if (diff_time > -0.04) {
    return true;
  }
  /*
  cout << "(video) pts stream:"<<pts
       << " pts expect:"<<currentPTS
       << " window:"<<window<<endl;
  */
  return false;
}
    

void SyncClockMPEG::printTime(timeval_t* a,char* text) {
  cout << text
       << "time(sec):"<<a->tv_sec 
       << "time(usec)"<<a->tv_usec<<endl;
}

void SyncClockMPEG::print(char* text) {
  cout << text
       << " lastPTS:"<<lastPTS
       << " lastSCR:"<<lastSCR
       << " jitter:"<<jitter;
  printTime(&lastPTS_time,(char*)"lastPTS_time");
  printTime(&lastSCR_time,(char*)"lastSCR_time");
}
