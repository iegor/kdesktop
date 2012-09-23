/*
  basic synchronisation Classs
  Copyright (C) 2000 Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "syncClock.h"

#include <iostream>

using namespace std;

SyncClock::SyncClock() {
}


SyncClock::~SyncClock() {
}



int SyncClock::getSyncMode() {
  cout << "direct virtual call SyncClock::getSyncMode"<<endl;
  return __SYNC_NONE;
}


void SyncClock::setSyncMode(int) {
  cout << "direct virtual call SyncClock::setSyncMode"<<endl;
}



int SyncClock::syncAudio(double ,double ) {
  cout << "direct virtual call SyncClock::syncAudio"<<endl;
  return true;
}


int SyncClock::syncVideo(double,double,
			 TimeStamp*,
			 TimeStamp*) {
  cout << "direct virtual call SyncClock::syncVideo"<<endl;
  return true;
}



void SyncClock::print(char*) {
  cout << "direct virtual call print"<<endl;
}

