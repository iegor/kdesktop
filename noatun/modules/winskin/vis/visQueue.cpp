/*
  queue fft samples
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "visQueue.h"


VISQueue::VISQueue(int elements) {
  this->elements=elements;
  
  visArrayQueue = new std::vector<float>*[elements];
  for(int i=0;i<elements;i++)
  {
     visArrayQueue[i]=new std::vector<float>;
  }

}

VISQueue::~VISQueue() {
  for(int i=0;i<elements;i++) {
    delete visArrayQueue[i];
  }
  delete [] visArrayQueue;
}

std::vector<float>* VISQueue::getElement(int i)
{
  if ( (i < 0) || (i>elements) ) {
    return visArrayQueue[0];
  }
  return visArrayQueue[i];
}


