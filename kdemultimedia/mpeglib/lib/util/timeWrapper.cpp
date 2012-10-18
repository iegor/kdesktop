/*
  wrapps calls to usleep, gettimeofday,...
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "timeWrapper.h"

#if defined WIN32
// usec goes from 0 -> 1000000 (one Million) under Unix
int gettimeofday(struct timeval *tv, struct timezone *tz) {
  if(tv) {
    struct _timeb tb;                         
    _ftime(&tb);                              
    tv->tv_sec=tb.time;                       
    tv->tv_usec=1000*tb.millitm;   
  }
  return(0);
}

void abs_usleep(const timeval* tm) {
  _sleep((tm->tv_usec / 1000) + (tm->tv_sec * 1000));
}
#endif

#ifndef WIN32

void abs_usleep(struct timeval* tm) {
  select(0,NULL,NULL,NULL,tm);
}

#endif

TimeWrapper::TimeWrapper() {
}


TimeWrapper::~TimeWrapper() {
}

void TimeWrapper::sleep(int sec) {
  timeval_t time;
  time.tv_sec=sec;
  time.tv_usec=0;
  TimeWrapper::usleep(&time); 
}

void TimeWrapper::usleep(unsigned long usec) {
  timeval_t time;
  time.tv_sec=0;
  time.tv_usec=usec;
  TimeWrapper::usleep(&time);
}

void TimeWrapper::usleep(timeval_t* time) {
  struct timeval waitTime;
  waitTime.tv_sec=time->tv_sec;
  waitTime.tv_usec=time->tv_usec;
  /*Threads and usleep does not work, select is very portable*/
  ::abs_usleep(&waitTime);
}


void TimeWrapper::gettimeofday(timeval_t* time) {
  struct timeval waitTime;
  ::gettimeofday(&waitTime,NULL);
  time->tv_sec=waitTime.tv_sec;
  time->tv_usec=waitTime.tv_usec;
}
