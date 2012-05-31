/*
  wrapps calls to usleep, gettimeofday,...
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __TIMEWRAPPER_H
#define __TIMEWRAPPER_H

#if defined WIN32
#include <winsock.h>
#include <sys/timeb.h>
#else
#include <unistd.h>
#include <sys/time.h>
#include <strings.h>
#endif

#include <kdemacros.h>

typedef struct timeval_s {
  long tv_sec;   /* seconds */
  long tv_usec;  /* microseconds */
} timeval_t;

class KDE_EXPORT TimeWrapper {

 public:
  TimeWrapper();
  ~TimeWrapper();
  static void sleep(int sec);
  static void usleep(unsigned long usec);
  static void usleep(timeval_t* time);
  static void gettimeofday(timeval_t* time);

};
#endif
