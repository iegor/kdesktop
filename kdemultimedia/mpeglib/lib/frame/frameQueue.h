/*
  queues frames
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __FRAMEQUEUE_H
#define __FRAMEQUEUE_H

#include "frame.h"

class FrameQueue {

  Frame** entries;
  int fillgrade;
  int size;
  int writepos;
  int readpos;
 

 public:
  FrameQueue(int maxsize);
  ~FrameQueue();

  int getFillgrade();
  int canRead();
  int canWrite();
  void enqueue(Frame* frame);
  Frame* dequeue();
  Frame* peekqueue(int pos);
  

};
#endif
