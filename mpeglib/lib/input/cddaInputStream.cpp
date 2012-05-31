/*
  cdda input class based on cdparanoia
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef CDDA_PARANOIA

#include <iostream>

using namespace std;

#include "cddaInputStream.h"
#include "inputDetector.h"


void paranoiaCallback(long, int) {
  //cout << "long:"<<a<<" int:"<<b<<endl;
}

CDDAInputStream::CDDAInputStream() {
  drive = NULL;
  paranoia=NULL;
  device=NULL;
  track=1;
}


CDDAInputStream::~CDDAInputStream() {
  close();
}

// here we should encdoe the track Nr. as well
// eg: /dev/cdrom#1
int CDDAInputStream::getTrackAndDevice(const char* url) {
  int matches=0;
  // dest = "cdda:/dev/cdrom/track01.cda"
  char* noprotoString=InputDetector::removeProtocol(url);
  // noprotoString="/dev/cdrom/track01.cda"
  char* filename=InputDetector::getFilename(noprotoString);
  // filename="track01.cda"
  char* filenameNoExt=InputDetector::getWithoutExtension(filename);
  // filenameNoExt="track01"
  char* dir=InputDetector::removeExtension(noprotoString,filename);
  // dir="/dev/cdrom/"
  device=InputDetector::removeSlash(dir);
  track=1;
  if (filenameNoExt != NULL) {
    matches=sscanf(filenameNoExt,"track%02d",&track);
  }
  if (matches == 0) {
    cout << "no trackNumber found using default"<<endl;
  } 
  cout << "device:"<<device<<" track:"<<track<<endl;

  if (noprotoString != NULL) {
    delete noprotoString;
  }
  if (filename != NULL) {
    delete filename;
  }
  if (filenameNoExt != NULL) {
    delete filenameNoExt;
  }
  if (dir != NULL) {
    delete dir;
  }
  if (device == NULL) {
    cout << "no device found, using any"<<endl;
    return false;
  }
  return true;
}

int CDDAInputStream::open(const char* dest) {
  if (getTrackAndDevice(dest) == true) {
    drive = cdda_identify(device, CDDA_MESSAGE_PRINTIT, 0);
  }

  if (drive == NULL) {
    cout << "cdda_identify failed trying to find a device"<<endl;
    drive=cdda_find_a_cdrom(CDDA_MESSAGE_PRINTIT, 0);
  }
  if (drive == NULL) {
    cout << "nope. nothing found. give up"<<endl;
    return false;
  }
  cout << "cdda_open -s"<<endl;
  if (cdda_open(drive) != 0) {
    cout << "cdda_open(drive) failed"<<endl;
    close();
    return false;
  }
  cout << "cdda_open -e"<<endl;

  // debug things a bit
  int trackCount = drive->tracks;
  for (int i = 1; i <= trackCount; i++) {
    if (IS_AUDIO(drive, i)) {
      printf("track%02d.cda\n", i);
    } else {
      printf("no audio:%d\n",i);
    }
  }

  paranoia = paranoia_init(drive);
  if (paranoia == NULL) {
    cout << "paranoia init failed"<<endl;
    close();
    return false;
  }

  firstSector=cdda_track_firstsector(drive, track);
  lastSector=cdda_track_lastsector(drive, track);
  currentSector=firstSector;
  // paranoia && drive != NULL -> initialized!

  int paranoiaLevel = PARANOIA_MODE_FULL ^ PARANOIA_MODE_NEVERSKIP;
  paranoia_modeset(paranoia, paranoiaLevel);
  cdda_verbose_set(drive, CDDA_MESSAGE_PRINTIT, CDDA_MESSAGE_PRINTIT);
  paranoia_seek(paranoia, firstSector, SEEK_SET);

  return true;
}


void CDDAInputStream::close() {
  if (isOpen() == false) {
    return;
  }
  cdda_close(drive);
  drive=NULL;
  if (paranoia != NULL) {
    paranoia_free(paranoia);
    paranoia = 0;
  }
  if (device != NULL) {
    delete device;
    device=NULL;
  }
}


int CDDAInputStream::isOpen() {
  return (drive != NULL); 
}


int CDDAInputStream::eof() {
  if (isOpen()==false) {
    return true;
  }
  if (currentSector >= lastSector) {
    return true;  
  }
  return false;
}


int CDDAInputStream::read(char* dest,int len) {
  if (len != 2*CD_FRAMESIZE_RAW) {
    cout << "len must be 2*CD_FRAMESIZE_RAW"<<endl;
    exit(0);
  }
  int16_t * buf = paranoia_read(paranoia, paranoiaCallback);
  currentSector++;
  if (buf == NULL) {
    cout << "paranoia_read failed"<<endl;
    close();
    return 0;
  }
  memcpy(dest,buf,sizeof(int16_t)*CD_FRAMESIZE_RAW);
  return CD_FRAMESIZE_RAW;  
}


int CDDAInputStream::seek(long bytePos) {
  int byteLength=getByteLength();
  float ratio=(float)bytePos/(float)(byteLength+1);
  float wantSector=ratio*(float)((lastSector-firstSector));
  if (isOpen()) {
    currentSector=(int)wantSector;
    cout << "paranoia_seek:"<<currentSector<<endl;
    paranoia_seek(paranoia, currentSector, SEEK_SET);
  }
  return true;  
}

void CDDAInputStream::clear() {
  cout << "direct virtual call CDDAInputStream::clear:"<<endl;
}


long CDDAInputStream::getByteLength() {
  int sectors=lastSector-firstSector;
  int bytes=sectors*CD_FRAMESIZE_RAW*sizeof(int16_t);
  cout << "getByteLength:"<<bytes<<endl;
  return bytes;
}


long CDDAInputStream::getBytePosition() {
  int readSectors=currentSector-firstSector;
  int bytes=readSectors*CD_FRAMESIZE_RAW*sizeof(int16_t);
  return bytes;
}

#endif
//CDDA_PARANOIA





