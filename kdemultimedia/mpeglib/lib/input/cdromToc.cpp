/*
  reads toc from cdrom (system dependent)
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "cdromToc.h"

#include <iostream>

using namespace std;

CDRomToc::CDRomToc() {
  maxEntries=0;
}


CDRomToc::~CDRomToc() {
}


void CDRomToc::insertTocEntry(int minute,int second,int frame) {
  int i;
  int j;
  if (isElement(minute,second,frame)) {
    return;
  }

  i=getNextTocEntryPos(minute,second,frame);

  // now shift from i to end
  if (maxEntries == 100) {
    cerr << "maximum of toc entries reached"<<endl;
    exit(0);
  }
  for (j=maxEntries;j>i;j--) {
    tocEntries[j].minute=tocEntries[j-1].minute;
    tocEntries[j].second=tocEntries[j-1].second;
    tocEntries[j].frame=tocEntries[j-1].frame;
  }
  maxEntries++;
  
  tocEntries[i].minute=minute;
  tocEntries[i].second=second;
  tocEntries[i].frame=frame;  
  calculateRange();
}


int CDRomToc::getNextTocEntryPos(int minute,int second,int frame) {
  int i;
  if (maxEntries == 0) {
    return 0;
  }

  for (i=0;i<maxEntries;i++) {
    if (tocEntries[i].minute <= minute) {
      continue;
    } else {
      break;
    }
    if (tocEntries[i].second <= second) {
      continue;
    } else {
      break;
    }
    if (tocEntries[i].frame <= frame) {
      continue;
    }
    break;
  } 
  return i;
}

    
    
int CDRomToc::isElement(int minute,int second,int frame) {
  int i;

  for (i=0;i<maxEntries;i++) {
    if (tocEntries[i].minute == minute) {
      if (tocEntries[i].second == second) {
	if (tocEntries[i].frame == frame) {
	  return true;
	}
      }
    }
  }
  return false;
}






int CDRomToc::getTocEntries() {
  return maxEntries;
}


TocEntry* CDRomToc::getTocEntry(int entry) {
  return &(tocEntries[entry]);
}


void CDRomToc::print() {
  int i;
  cerr << "******* printing TOC [START]"<<endl;
  for(i=0;i<maxEntries;i++) {
    cerr << "i:"<<i
	 <<" M:"<<tocEntries[i].minute
         <<" S:"<<tocEntries[i].second
         <<" F:"<<tocEntries[i].frame<<endl;
  }
  cerr << "******* printing TOC [END}"<<endl;
}


int CDRomToc::isInRange(int minute,int second,int) {
  long val=minute*60+second;
  if (val < startByte) {
    return false;
  }
  if (val > endByte) {
    return false;
  }
  return true;
}

int CDRomToc::calculateRange() {


  if (maxEntries < 2) {
    cout << "no two elemts in toc"<<endl;
    return false;
  }
  startByte=tocEntries[0].minute*60*+tocEntries[0].second;
  
  // do a safty end because of the kernel bug
  int minute=tocEntries[maxEntries-1].minute;
  int second=tocEntries[maxEntries-1].second-20;
  if (second < 0) {
    minute--;
    second=60+second;
  }
  if (minute < 0) {
    endByte=0;
    return true;
  }

  endByte=minute*60+second;

  return true;

}


int CDRomToc::getEndSecond() {
  return endByte;
}




int CDRomToc::open(const char* openfile) {
  int i;
  int pos=0;
  maxEntries=0;
  const char* filename=strchr(openfile,'/');
  FILE* file =fopen(filename, "rb");
  if (file == NULL) {
    perror("open");
    return false;
  }
  cout << "reading toc on:"<<filename<<" openfile:"<<openfile<<endl;

  int startToc=0;
  int endToc=0;

  if (getStartEnd(file,startToc,endToc) == false) {
    cout << "getStartEnd in CDRomToc failed"<<endl;
    fclose(file);
    return false;
  }
  cout << "startToc:"<<startToc<<" endToc:"<<endToc<<endl;
  cout << "reading toc -2"<<endl;
  /* read individual tracks */
  int min;
  int sec;
  int frame;

  for (i=startToc; i<=endToc; i++) {
    int min;
    int sec;
    int frame;

    if (readToc(file,i,min,sec,frame) == false) {
      cout << "error in CDRomToc::readToc"<<endl;
      fclose(file);
      return false;
    }
    cout << "min:"<<min<<endl;
    cout << "sec:"<<sec<<endl;
    cout << "frame:"<<frame<<endl;
    
    insertTocEntry(min,sec,frame);
    pos++;
  }


  /* read the lead-out track */
  if (readLeadOut(file,min,sec,frame) == false) {
    cout << "error in CDRomToc::reatLeadOut"<<endl;
    return false;
  }
  pos++;
  insertTocEntry(min,sec,frame);
  
  maxEntries=pos;

  fclose(file);

  return true;
}

