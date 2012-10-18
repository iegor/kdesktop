/*
  class for managing byte positions and associated time positions
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <iostream>

#include "timeStampArray.h"

using namespace std;


TimeStampArray::TimeStampArray(char* aName,int entries) {

  writePos=0;
  readPos=0;
  fillgrade=0;
  lastWritePos=0;
  this->entries=entries;
  if (entries < 1) {
    cout << "TimeStampArray entries must be >= 1";
    exit(0);
  }


  abs_thread_mutex_init(&writeInMut);
  abs_thread_mutex_init(&changeMut);

  

  name=strdup(aName);
  int i;
  tStampArray=new TimeStamp*[entries];

  for(i=0;i<entries;i++) {
    tStampArray[i]=new TimeStamp();
  }

}


TimeStampArray::~TimeStampArray() {

  int i;
  for(i=0;i<entries;i++) {
    delete tStampArray[i];
  } 
  delete [] tStampArray;
  if (name != NULL) {
    free(name);  // allocated with strdup
  }
  abs_thread_mutex_destroy(&writeInMut);
  abs_thread_mutex_destroy(&changeMut);
}

TimeStamp*  TimeStampArray::getReadTimeStamp() {
  return tStampArray[readPos];
}


int TimeStampArray::getFillgrade() {
  return fillgrade;
}

int TimeStampArray::insertTimeStamp(TimeStamp* src,long key,int len) {
  if (src == NULL) {
     return true;
  }
  lockStampArray();
  int back=true;
  src->copyTo(tStampArray[writePos]);
  tStampArray[writePos]->setKey(key,len);
  /*
  if (fillgrade > 0) {
    if (tStampArray[lastWritePos]->getKey() == key) {
      unlockStampArray();
      return;
    }
  }
  */

  lastWritePos=writePos;
  writePos++;
  fillgrade++;
  if (writePos == entries) {
    writePos=0;
  }
  if (fillgrade == entries) {
    cout << name<<" TimeStampArray::array overfull forward"<<endl;
    internalForward();
    back=false;
  }
  unlockStampArray();
  return back;
}


int TimeStampArray::bytesUntilNext(long key) {
  lockStampArray();
  TimeStamp* current=tStampArray[readPos];
  int back=current->getKey()-key;
  unlockStampArray();
  return back;
}

TimeStamp* TimeStampArray::getTimeStamp(long key) {
  lockStampArray();
  TimeStamp* back=tStampArray[readPos];
  if (key > back->getKey()+back->getKeyLen()) {
    if (fillgrade > 1) {
      internalForward();
      unlockStampArray();
      return getTimeStamp(key);
    }
  } 

  /*
  if (back->getKey() > key) {
    cout << "key "<<key<<" too big"<<back->getKey()-key<<endl;
    back->print("key access");
  } 
  */
  unlockStampArray();
  /* maybe we should return NULL here to indicate
     that there is no valid timestamp */
  /* This would need a check for every getTimeStamp call
     I think returning the last available stamp is ok
  */
  return back;
}


void TimeStampArray::forward() {
  lockStampArray();
  internalForward();
  unlockStampArray();
}



void TimeStampArray::clear() {
  lockStampArray();
  writePos=0;
  readPos=0;
  fillgrade=0;
  unlockStampArray();
}
 

void TimeStampArray::lockStampArray() {

  abs_thread_mutex_lock(&changeMut);
  abs_thread_mutex_lock(&writeInMut);
  abs_thread_mutex_unlock(&changeMut);

}


void TimeStampArray::unlockStampArray() {
  abs_thread_mutex_unlock(&writeInMut); 
}


void TimeStampArray::internalForward() {
  readPos++;
  fillgrade--;
  if (readPos == entries) {
    readPos=0;
  }
}
