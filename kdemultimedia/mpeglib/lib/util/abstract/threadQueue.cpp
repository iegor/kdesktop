/*
  fifo waitqueue for threads.(Multi-in, single out)
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#include "threadQueue.h"

#define _MAX_THREAD_IN_QUEUE 5

#include <iostream>

using namespace std;

//
// WaitThreadEntry class [START]
//

WaitThreadEntry::WaitThreadEntry() {
  abs_thread_cond_init(&waitCond);
}


WaitThreadEntry::~WaitThreadEntry() {
  abs_thread_cond_destroy(&waitCond);
}

//
// WaitThreadEntry class [END]
//


ThreadQueue::ThreadQueue() {
  waitThreadEntries=new WaitThreadEntry* [_MAX_THREAD_IN_QUEUE];
  int i;
  for(i=0;i<_MAX_THREAD_IN_QUEUE;i++) {
    waitThreadEntries[i]=new WaitThreadEntry();
  }
  abs_thread_mutex_init(&queueMut);
  insertPos=0;
  removePos=0;
  size=0;
}


ThreadQueue::~ThreadQueue() {
  abs_thread_mutex_lock(&queueMut);
  if (size != 0) {
    cout << "Aieee! Make sure that all threads are out of ThreadQueue"<<endl;
    exit(0);
  }
  int i;
  for(i=0;i<_MAX_THREAD_IN_QUEUE;i++) {
    delete waitThreadEntries[i];
  }
  delete [] waitThreadEntries;
  abs_thread_mutex_unlock(&queueMut);
  abs_thread_mutex_destroy(&queueMut);
}


void ThreadQueue::waitForExclusiveAccess() {
  abs_thread_mutex_lock(&queueMut);
  if (size == 0) {
    abs_thread_mutex_unlock(&queueMut);
    return;
  }
  // wait
  size++;
  if (size == _MAX_THREAD_IN_QUEUE) {
    cout << "Aieee! ThreadQueue can only buffer:"<<_MAX_THREAD_IN_QUEUE<<endl;
    exit(0);
  }
  abs_thread_cond_t* waitCond=&(waitThreadEntries[insertPos]->waitCond);
  insertPos++;
  // wrap counter
  if (insertPos == _MAX_THREAD_IN_QUEUE) {
    insertPos=0;
  }
  abs_thread_cond_wait(waitCond,&queueMut);
  abs_thread_mutex_unlock(&queueMut);
}


void ThreadQueue::releaseExclusiveAccess() {
  abs_thread_mutex_lock(&queueMut);
  if (size == 0) {
    abs_thread_mutex_unlock(&queueMut);
    return;
  } 
  // wake up next thread
  abs_thread_cond_t* waitCond=&(waitThreadEntries[removePos]->waitCond);
  removePos++;
  // wrap counter
  if (removePos == _MAX_THREAD_IN_QUEUE) {
    removePos=0;
  }
  size--;
  abs_thread_cond_signal(waitCond);
  abs_thread_mutex_unlock(&queueMut);
}

