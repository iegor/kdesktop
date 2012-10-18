/*
  queue fft samples
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __VISQUEUE_H
#define __VISQUEUE_H


#include <vector>

class VISQueue {

  int elements;
  std::vector<float> **visArrayQueue; 

 public:
  VISQueue(int elements);
  ~VISQueue();

  std::vector<float>* getElement(int i);

};
#endif
