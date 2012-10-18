/*
  queues frames in a "empty" and "data" queue
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __IOFRAMEQUEUE_H
#define __IOFRAMEQUEUE_H

#include "frameQueue.h"
#include <kdemacros.h>

/**
   This class can store up to <size> frames.
   The frames are marked "empty" these frames can be re-used
   and filled with data.
   The other queue stores "data" frames. These frames can 
   be dequeued and then are be passed to the "empty" queue.

   Note: you need to _fill_ the empty queue with allocated frames,
         after constructions. The pointers then are owned by this
         class (== deleted in destructor)

*/

class KDE_EXPORT IOFrameQueue {


 public:
  IOFrameQueue(int size);
  ~IOFrameQueue();

  //
  // empty queue
  //
  int emptyQueueCanRead();
  int emptyQueueCanWrite();

 
  //
  // data queue
  //
  int dataQueueCanRead();
  int dataQueueCanWrite();
  
 
 protected:
  FrameQueue* empty;
  FrameQueue* data;
 
};
#endif
