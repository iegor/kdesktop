/*
  queues frames
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "frameQueue.h"

#include <iostream>

using namespace std;

FrameQueue::FrameQueue(int maxsize) {
  this->size=maxsize;
  entries = new Frame*[size];
  int i;
  for(i=0;i<size;i++) {
    entries[i]=NULL;
  }
  fillgrade=0;
  writepos=0;
  readpos=0;
}


FrameQueue::~FrameQueue() {
  int i;
  for(i=0;i<size;i++) {
    if (entries[i] != NULL) {
      delete entries[i];
    }
  }
  delete entries;
}

int FrameQueue::canRead() {
  return (fillgrade > 0);
}


int FrameQueue::canWrite() {
  return (fillgrade < size);
}


int FrameQueue::getFillgrade() {
  return fillgrade;
}


void FrameQueue::enqueue(Frame* frame) {
  if(canWrite() == false) {
    cout << "FrameQueue full cannot enqueue"<<endl;
    exit(0);
  }
  fillgrade++;
  entries[writepos]=frame;
  writepos++;
  if (writepos == size) {
    writepos=0;
  }
}


Frame* FrameQueue::dequeue() {
  if(canRead() == false) {
    cout << "FrameQueue empty cannot dequeue"<<endl;
    exit(0);
  }
  Frame* back=entries[readpos];
  // invalide this frame, we do not longer own it!
  entries[readpos]=NULL;
  fillgrade--;
  readpos++;
  if (readpos == size) {
    readpos=0;
  }
  return back;
}

Frame* FrameQueue::peekqueue(int pos) {
  if(fillgrade-pos <= 0) {
    cout << "FrameQueue : cannot peek this positon"<<endl;
    cout << "fillgrade:"<<fillgrade<<endl;
    cout << "pos:"<<pos<<endl;
    exit(0);
  }
  int getpos=(readpos+pos) % size;

  
  Frame* back=entries[getpos];
  return back; 
}

