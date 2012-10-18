/*
  reads raw input data from cdrom (system dependent)
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <iostream>

#include "cdromRawAccess.h"

using namespace std;

CDRomRawAccess::CDRomRawAccess(CDRomToc* cdromToc) {
  cdfile=NULL;
  lOpen=false;
  leof=true;
  this->cdromToc=cdromToc;
  lData=false;
  dataStart=0;
  len=0;
}


CDRomRawAccess::~CDRomRawAccess() {
  close();
}



char* CDRomRawAccess::getBufferStart() {
  return (char*)(data+dataStart);
}

int CDRomRawAccess::getBufferLen() {
  if (eof()) {
    return 0;
  }
  return len;
}

int CDRomRawAccess::open(const char* filename) {
  if (isOpen()) {
    close();
  }
  if (filename==NULL) {
    filename=(char*)"/dev/cdrom";
  }
  if (strlen(filename) <= 1) {
    filename="/dev/cdrom";
  } 
  char* openfile=strchr(filename,'/');
  cout << "openfile:"<<openfile<<endl;
  cdfile=fopen(openfile, "rb");
  lOpen=false;

  if (cdfile == NULL) {
    perror("open CDRomRawAccess");
  } else {
    lOpen=true;  
    leof=false;
  }
  return lOpen;
}


int CDRomRawAccess::eof() {
  return leof;
}


int CDRomRawAccess::read(int minute,int second, int frame) {
  if (isOpen()==false) {
    cerr << "CDRomRawAccess not open"<<endl;
    return false;
  }

  int lInRange=cdromToc->isInRange(minute,second,frame);
  if (lInRange == false) {
    if (minute*60+second+1 > cdromToc->getEndSecond()) {
      leof=true;
    }
    return false;
  }
  return readDirect(minute,second,frame);
}


void CDRomRawAccess::close() {
  if (isOpen()) {
    fclose(cdfile);
    lOpen=false;
    leof=true;
  }
}

int CDRomRawAccess::isData() {
  return lData;
}

int CDRomRawAccess::isOpen() {
  return lOpen;
}




