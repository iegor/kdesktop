/*
  simple file access interface.
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "fileAccess.h"


FileAccess::FileAccess() {
  file=NULL;
  length=0;
}


FileAccess::~FileAccess() {
  close();
}


int FileAccess::open(const char* path) {
  close();
  file=fopen(path,"rb");
  length=calcByteLength();
  return (file != NULL);
}


void FileAccess::close() {
  if (file != NULL) {
    fclose(file);
    file=NULL;
    length=0;
  }
}


int FileAccess::read(char* dest,int len) {
  int back=0;
  if (file != NULL) {
    back=fread(dest,1,len,file);
  } else {
    printf("FileAccess::read not open\n");
  }
  return back;
}
						
int FileAccess::eof() {
  int back=true;
  if (file != NULL) {
    back=feof(file);
  }
  return back;
}


int FileAccess::seek(long pos) {
  if (file == NULL) {
    return -1;
  }
  return fseek(file,pos,SEEK_SET);
}


long FileAccess::getBytePosition() {
  if (file == NULL) {
    return 0;
  }
  return ftell(file);
}


long FileAccess::getByteLength() {
  return length;
}

long FileAccess::calcByteLength() {
  if (file == NULL) {
    return 0;
  }
  long pos=getBytePosition();
  fseek(file,0,SEEK_END);
  long back=getBytePosition();
  fseek(file,pos,SEEK_SET);
  return back;
}

