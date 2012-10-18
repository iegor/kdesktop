/*
  system dependent wrapper for access to cdrom (no system)
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "cdromToc.h"
#include "cdromRawAccess.h"

#include <iostream>
using namespace std;


int CDRomToc::getStartEnd(FILE* file,int& startToc,int& endToc) {
  cout << "CDRomToc::getStartEnd not implemented on your system"<<endl;
  return false;
}


int CDRomToc::readToc(FILE* file,int num,int& min,int& sec, int& frame) {
  cout << "CDRomToc::readToc not implemented on your system"<<endl;
  return false;
}


int CDRomToc::readLeadOut(FILE* file,int& min,int& sec, int& frame) {
  cout << "CDRomToc::reatLeadOut not implemented on your system"<<endl;
  return false;
}



int CDRomRawAccess::readDirect(int minute,int second, int frame) {


  cout << "no CDRomRawAccess::read implemented for your system"<<endl;
  lData=false;
  return true;
}

