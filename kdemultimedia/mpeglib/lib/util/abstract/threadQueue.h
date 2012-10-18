/*
  fifo waitqueue for threads.(Multi-in, single out)
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __THREADQUEUE_H
#define __THREADQUEUE_H


#include "abs_thread.h"

class  WaitThreadEntry {
 public:
  WaitThreadEntry();
  ~WaitThreadEntry();

  abs_thread_cond_t waitCond;
};

/**
   This class can be used as a general purpuse wrapper to
   make C++ classes thread safe.
   Mpeglib uses for every decoder a single thread which
   reads from the input and write to one output type (video/audio)
   To make the input and output classes thread safe you have
   two solutions. First you can try to do it in every class
   itsself, this is much work and needs understanding of
   threads or you can use this wrapper class.
   Normally you don't need two threads in one class, only
   for the audio/video sync this is necessary, but for
   the inputstream (file,http,..) this not necessary.
   For the output this is the same.
   This class offers two methods. waitForExclusiceAcess()
   and releaseExlusiveAcess. Internally the thread who
   calls waitFor.. in enqueued (if it does not get the exclusive
   access) the thread who have the exclusive access calls
   sometimes release.. with then pass the exclusive access
   to the next thread. 
   Why it is needed?
   Because we access the input/output streams from different
   threads. A user of mpeglib may want to set mpeg video
   in fullscreen mode, this means two threads call
   methods in the output classes including: closing windows,
   resizing windows ... now this is safley possible when
   the threadSafeInputStream / threadSafeoutputStream wrappers
   are used, which forward the calls to the real classes.
*/

class ThreadQueue {

  abs_thread_mutex_t queueMut; 
  int insertPos;
  int removePos;
  int size;
  WaitThreadEntry** waitThreadEntries;

 public:
  ThreadQueue();
  ~ThreadQueue();

  void waitForExclusiveAccess();
  void releaseExclusiveAccess();

};

#endif
