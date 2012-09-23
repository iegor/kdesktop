/*
  system dependent wrapper for access to cdrom (Linux)
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#define _ANSI_WAS_HERE_
#endif
#include <linux/types.h>
#include <linux/cdrom.h>
#ifdef _ANSI_WAS_HERE_
#define __STRICT_ANSI__ 
#endif
#include <sys/ioctl.h>

#include "cdromToc.h"
#include "cdromRawAccess.h"

#include <iostream>

using namespace std;

/** 
    here you find an example how to port the access method
    to your system.
*/


int CDRomToc::getStartEnd(FILE* file,int& startToc,int& endToc) {
  struct cdrom_tochdr tochdr;
  int fd=fileno(file);
  if (ioctl(fd, CDROMREADTOCHDR, &tochdr) == -1) {
    perror("ioctl cdromreadtochdr");
    return false;
  }
  
  startToc=tochdr.cdth_trk0;
  endToc=tochdr.cdth_trk1;
  return true;
}


int CDRomToc::readToc(FILE* file,int num,int& min,int& sec, int& frame) {
  struct cdrom_tocentry tocent;
  int fd=fileno(file);
  tocent.cdte_track = num;
  tocent.cdte_format = CDROM_MSF;
  if (ioctl(fd, CDROMREADTOCENTRY, &tocent) == -1 ) {
    perror("ioctl cdromreadtocentry");
    return false;
  }
  min=tocent.cdte_addr.msf.minute;
  sec=tocent.cdte_addr.msf.second;
  frame=tocent.cdte_addr.msf.frame;
  return true;
}


int CDRomToc::readLeadOut(FILE* file,int& min,int& sec, int& frame) {
  struct cdrom_tocentry tocent;
  int fd=fileno(file);
  tocent.cdte_track = CDROM_LEADOUT;
  tocent.cdte_format = CDROM_MSF;
  if (ioctl(fd, CDROMREADTOCENTRY, &tocent) == -1 ) {
    perror("ioctl cdromreadLeadoutn");
    return false;
  }
  min=tocent.cdte_addr.msf.minute;
  sec=tocent.cdte_addr.msf.second;
  frame=tocent.cdte_addr.msf.frame;
  return true;
}



int CDRomRawAccess::readDirect(int minute,int second, int frame) {

  // this comes from smpeg 
  // smpeg is an mpeg I player from lokigames www.lokigames.com

  struct cdrom_msf *msf;
  int fd=fileno(cdfile);

  msf = (struct cdrom_msf*) data;
  msf->cdmsf_min0   = minute;
  msf->cdmsf_sec0   = second;
  msf->cdmsf_frame0 = frame;
  if (ioctl(fd, CDROMREADMODE2, msf) == -1) {
    perror("ioctl cdromreadmode2");
    cout << "min:"<<minute
         << " sec:"<<second
         << " frame:"<<frame<<endl;
    return false;
  } else {
    //cout << "read success ****************"<<endl;
  }

  char* subheader=data+sizeof(int);


  if ((subheader[1]==1) && 
      (((subheader[2]==0x62) &&
	(subheader[3]==0x0f)) || ((subheader[2]==0x64) &&
				  (subheader[3]==0x7f)))) {
    lData=true;
    dataStart=sizeof(int)+4;
  } else {
    lData=false;
  }

  len=2324;


  return true;
}

