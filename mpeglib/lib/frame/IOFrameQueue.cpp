/*
  queues frames in a "empty" and "data" queue
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "IOFrameQueue.h"


IOFrameQueue::IOFrameQueue(int size) {
  empty=new FrameQueue(size);
  data=new FrameQueue(size);
 
}


IOFrameQueue::~IOFrameQueue() {
  delete empty;
  delete data;
}


//
// empty queue
//
int IOFrameQueue::emptyQueueCanRead() {
  return empty->canRead();
}


int IOFrameQueue::emptyQueueCanWrite() {
  return empty->canWrite();
}




//
// data queue
//
int IOFrameQueue::dataQueueCanRead() {
  return data->canRead();
}


int IOFrameQueue::dataQueueCanWrite() {
  return data->canWrite();
}




 
