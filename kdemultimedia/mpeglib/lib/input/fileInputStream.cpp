/*
  reads input data
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "fileInputStream.h"

#include <iostream>

using namespace std;

FileInputStream::FileInputStream() {
  file=NULL;
  lopen=false;
  fileLen=0;
}


FileInputStream::~FileInputStream() {
  close();
}


int FileInputStream::open(const char* dest) {

  close();
  if (dest == NULL) {
    return false;
  }
  setUrl(dest);
  if (strlen(dest) == 1) {
    if (strncmp(dest,"-",1)==0) {
      file=::fdopen(0,"rb");
    }
  }
  // load out of current dir if no full path is given
  if (file == NULL) {
    file=fopen(dest,"rb");
  }
  fileLen=0;
  if (file == NULL) {
    cout <<"cannot open file:"<< dest<<endl;

  } else {
    lopen=true;
    struct stat fileStat;
    stat(dest,&fileStat);
    fileLen=(long)fileStat.st_size;
  }
  int back=(file!=NULL);
  return back;
}


void FileInputStream::close() {
  if (isOpen()) {
    ::fclose(file);
    file=NULL;
    lopen=false;
  }
}


int FileInputStream::isOpen() {
  return lopen;
}


int FileInputStream::eof() {
  if (isOpen()==false){
    return true;
  }
  int back=true;
  if (file != NULL) {
    back=feof(file);
  }

  return back;
}


int FileInputStream::read(char* ptr,int size) {
  int bytesRead=-1;
  if (isOpen()) {
    if (size <= 0) {
      cout << "size is <= 0!"<<endl;
      return 0;
    }
    if (file != NULL) {
      bytesRead=fread(ptr,1,size,file);
    }
  } else {
    cerr << "read on not open file want:"<<size<<endl;
    return 0;
  }
  return bytesRead;
}


int FileInputStream::seek(long posInBytes) {
  int back=true;
  if (isOpen()==false) {
    return false;
  }
  long pos=-1;
  if (file != NULL) {
    pos=fseek(file,posInBytes,SEEK_SET);
  }

  if (pos < 0) {
    cout <<"seek error in FileInputStream::seek"<<endl;
    back=false;
  }
  return back;
}


long FileInputStream::getByteLength() {
  return fileLen;
}


long FileInputStream::getBytePosition() {
  int back=0;
  if (isOpen()) {
    if (file != NULL) {
      back=ftell(file);
    }
  }
  return back;
}



void FileInputStream::print() {
  printf("pos in file:%8x\n",(int)getBytePosition());
}


