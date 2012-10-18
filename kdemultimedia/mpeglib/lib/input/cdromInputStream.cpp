/*
  reads input data from cdrom
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "cdromInputStream.h"
#include "cdromRawAccess.h"
#include "cdromToc.h"
#include <string.h>
#include "inputDetector.h"

#include <iostream>

using namespace std;

CDRomInputStream::CDRomInputStream() {
  cdRomToc=new CDRomToc();
  cdRomRawAccess=new CDRomRawAccess(cdRomToc);

  buflen=0;
  bufCurrent=NULL;

  currentMinute=0;
  currentSecond=2;
  currentFrame=0;
  byteCounter=0;
}


CDRomInputStream::~CDRomInputStream() {
  delete cdRomRawAccess;
  delete cdRomToc;
}


int CDRomInputStream::readCurrent() {
  int ok=cdRomRawAccess->read(currentMinute,currentSecond,currentFrame);
  if (ok==false) {
    if (cdRomRawAccess->eof() == false) {
      int pos=cdRomToc->getNextTocEntryPos(currentMinute,
					   currentSecond,
					   currentFrame);

      // now try to read a few sectors
      int cnt=0;
      int back=false;

      while(1) {
	// jump forward
	int i;
	for(i=0;i<_CDROM_FRAMES-currentFrame;i++) {
	  next_sector();
	}
	cout << "trying next ..."<<endl;
	ok=cdRomRawAccess->read(currentMinute,currentSecond,currentFrame);
	if (ok) {
	  bufCurrent=cdRomRawAccess->getBufferStart();
	  buflen=cdRomRawAccess->getBufferLen();
	  return true;
	}
	cnt++;
	if (cnt > 100) {
	  break;
	}
      }
      cout << "last possible jump"<<endl;
      if (pos > 1) {
	TocEntry* tocEntry=cdRomToc->getTocEntry(pos-1);
	currentMinute=tocEntry->minute;
	currentSecond=tocEntry->second;
	currentFrame=tocEntry->frame;
	back=cdRomRawAccess->read(currentMinute,currentSecond,currentFrame);
	if (back) {
	  bufCurrent=cdRomRawAccess->getBufferStart();
	  buflen=cdRomRawAccess->getBufferLen();
	}
      }
      return back;
    }
    return false;
  }
  bufCurrent=cdRomRawAccess->getBufferStart();
  buflen=cdRomRawAccess->getBufferLen();
  return true;
}


int CDRomInputStream::getByteDirect() {
  int back;
  if (buflen==0) {
    fillBuffer();
  }
  if (buflen==0){
    return EOF;
  }
  back=*bufCurrent;
  buflen--;
  bufCurrent++;
  byteCounter++;
  return back;
}


int CDRomInputStream::read(char* ptr,int size) {
  char* dest=(char*)ptr;
  int bytesRead=0;
  int doRead=size;
  int canRead;

  while(eof() == false) {
    if (buflen == 0) {
      if (fillBuffer() == false) {
	return 0;
      }
      continue;
    }
    canRead=buflen;
    if (doRead < canRead) {
      canRead=doRead;
    }
    memcpy((void*)dest,(void*)bufCurrent,canRead);
    buflen-=canRead;
    bufCurrent+=canRead;
    bytesRead+=canRead;
    dest+=canRead;
    doRead-=canRead;
    if (doRead == 0) {
      byteCounter+=bytesRead;
      return bytesRead;
    }
  }
  return 0;
}

int CDRomInputStream::eof() {
  return cdRomRawAccess->eof();
}




long CDRomInputStream::getBytePosition() {
  return byteCounter;
}


void CDRomInputStream::print() {
}


void CDRomInputStream::next_sector() {
  currentFrame++;
  if (currentFrame>=_CDROM_FRAMES) {
    currentFrame = 0;
    currentSecond++;
    if (currentSecond>=_CDROM_SECS) {
      currentSecond = 0;
      currentMinute++;
    }
  }
}

int CDRomInputStream::open(const char* file) {
  cout << "CDRomInputStream::open:"<<file<<endl;
  char* noExtension=InputDetector::getWithoutExtension(file);
  cout << "CDRomInputStream::noExt:"<<noExtension<<endl;
  if (noExtension == NULL) {
    return false;
  }
  cdRomToc->open(noExtension);
  cdRomRawAccess->open(noExtension);
  if (isOpen()==false) {
    return false;
  }
  setUrl(noExtension);
  int entries=cdRomToc->getTocEntries();
  cdRomToc->print();
  if (entries == 1) {
    cerr << "only lead out"<<endl;
  }
  
  //  cdRomRawAccess->insertTocEntry(0,2,0);
  //cdRomToc->insertTocEntry(1,13,5);

  TocEntry* tocEntry=cdRomToc->getTocEntry(0);
  currentMinute=tocEntry->minute;
  currentSecond=tocEntry->second;
  currentFrame=tocEntry->frame;
  delete noExtension;
   
  return readCurrent();
}


void CDRomInputStream::close() {
  cdRomRawAccess->close();
  byteCounter=0;
  setUrl(NULL);
}


int CDRomInputStream::isOpen() {
  return cdRomRawAccess->isOpen();
}




long CDRomInputStream::getBytePos(int min,int sec) {
  long back;
  // 2324 is the size of a cdfram
  back=sec*_CDROM_FRAMES*2324;
  back=back+min*60*_CDROM_FRAMES*2324;
  cout << "CDRomInputStream::getByteLength"<<back<<endl;
  return back; 
}

long CDRomInputStream::getByteLength() {
  // we get the length out of the toc and then multiply like hell
  long totalSecs=cdRomToc->getEndSecond();
  long min=totalSecs/60;
  long sec=totalSecs-60*min;

  long back=getBytePos(min,sec);

  return back; 
}


int CDRomInputStream::seek(long posInBytes) {
  int entries=cdRomToc->getTocEntries();

  TocEntry* firstEntry;
  if (entries == 0) {
    return false;
  }
  if (posInBytes < 0) {
    return false;
  }
  firstEntry=cdRomToc->getTocEntry(0);
  
  long startByte=getBytePos(firstEntry->minute,firstEntry->second+1);

  posInBytes=posInBytes+startByte;


  float fmin=(float)posInBytes/(float)(60*_CDROM_FRAMES*2324);
  int min=(int)fmin;


  long sec=posInBytes-min*(60*_CDROM_FRAMES*2324);
  sec=sec/(_CDROM_FRAMES*2324);

  byteCounter=posInBytes;
  if (cdRomRawAccess->read(min,sec,0)==false) {
    return false;
  }
  setTimePos(min*60+sec);

  return true;
}


int CDRomInputStream::setTimePos(int second) {
  currentFrame=0;
  currentMinute=second/60;
  currentSecond=second%60;
  
  return fillBuffer();
}





int CDRomInputStream::fillBuffer() {
  int maxNoData=30;
  int cnt=0;
  if (buflen==0) {
    while (cnt < maxNoData) {
      next_sector();
      if (readCurrent() == false) {
	return false;
      }
      if (cdRomRawAccess->isData() == false) {
	//	cerr << "no data"<<endl;
	cnt++;
      } else {
	return true;
      }
    }
    return false;
  }
  return true;
}




    
  
