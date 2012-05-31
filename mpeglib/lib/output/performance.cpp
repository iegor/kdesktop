/*
  measures picture/second
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "performance.h"

#include <iostream>

using namespace std;


Performance::Performance() {
  picCnt=0;
  startTime=new TimeStamp();
  endTime=new TimeStamp();
 
}


Performance::~Performance() {
  delete startTime;
  delete endTime;
}


void Performance::incPictureCount() {
  if (picCnt==0) {
    startTime->gettimeofday();
  }
  picCnt++;
  if (picCnt==200) {
    endTime->gettimeofday();
    TimeStamp diffTime;
    endTime->minus(startTime,&diffTime);
    double secs=(double)diffTime.getAsSeconds();

    double picSec=(double)picCnt/secs;
    cout << "picPerSec:"<<picSec<<" secs:"<<secs<<endl;
    picCnt=0;
  }
}
