/*
  reads toc from cdrom (system dependent)
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __CDROMTOC_H
#define __CDROMTOC_H

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
}

struct TocEntry {
  int minute;
  int second;
  int frame;
};


class CDRomToc {

  TocEntry tocEntries[100];
  int maxEntries;
  int startByte;
  int endByte;

 public:
  CDRomToc();
  virtual ~CDRomToc();
  int open(const char* device);
  int getTocEntries();
  TocEntry* getTocEntry(int entry);
  void insertTocEntry(int minute,int second,int frame);
  int getNextTocEntryPos(int minute,int second,int frame);
  int isInRange(int minute,int second,int frame);
  int isElement(int minute,int second,int frame);

  int getEndSecond();

  void print();

 private:
  // platform specific calls.
  int getStartEnd(FILE* file,int& start, int& end);
  int readToc(FILE* file,int num,int& min,int& sec, int& frame);
  int readLeadOut(FILE* file,int& min,int& sec, int& frame);




  int calculateRange();

};

#endif
