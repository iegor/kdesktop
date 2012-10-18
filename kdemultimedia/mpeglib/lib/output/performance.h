/*
  measures picture/second
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __PERFORMANCE_H
#define __PERFORMANCE_H

#include "../util/timeStamp.h"

class Performance {

  int picCnt;
  TimeStamp* startTime;
  TimeStamp* endTime;
 

 public:

  Performance();
  ~Performance();
  void incPictureCount();

};
#endif
