/*
  This class implements a dynamic string buffer
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "dynBuffer.h"

#include <iostream>

using namespace std;


DynBuffer::DynBuffer(int size) {
  nSize=size;
  msg=(char*) malloc(sizeof(char)*(nSize+1));
  msg[nSize]='\0';
  clear();

}


DynBuffer::~DynBuffer() {
  free (msg);
}


void DynBuffer::clear() {
  msg[0]='\0';
}

void DynBuffer::append(int value) {
  DynBuffer buf(30);
  sprintf(buf.getData(),"%d",value);
  append(buf.getData());
}




void DynBuffer::append(char* appendMsg) {
  if (appendMsg == msg) {
    cout << "cannot append to self"<<endl;
    exit(0);
  }
  char* appendPos=getAppendPos();
  int nlen=strlen(appendMsg);
  if (appendPos == NULL) return;
  
  append(appendMsg,nlen);
}
 

void DynBuffer::append(const char* appendMsg) {
  append((char*)appendMsg);
}

 
void DynBuffer::append(char* buffer,int buflen) {
  int nlen=len();
  int nBedarf;
  if (buffer == msg) {
    cout << "cannot append to self"<<endl;
    exit(0);
  }
  if (buflen+nlen <= nSize) {
    char* appendPos=getAppendPos();
    strncpy(appendPos,buffer,buflen);
    appendPos[buflen]='\0';
    return;
  }
  nBedarf=(nlen+buflen)-nSize;
  grow(nBedarf);
  append(buffer,buflen);
}
 
char* DynBuffer::getAppendPos() {
  int i;
  // this Array has nSize+1 entries!
  // and it *is* granted that msg[nSize]=0;  (think so)
  for (i=0;i<=nSize;i++) {
    if (msg[i] == '\0') return &(msg[i]);
  }
  // should never reach this point
  return NULL;
}


void DynBuffer::setData(char* msg) {
  if (strlen(msg) == 0) {
    clear();
    return;
  }
  clear();
  append(msg);
}

char* DynBuffer::getData() {
  return msg;
}


int  DynBuffer::len() {
  return strlen(msg);
}  

int DynBuffer::getSize() {
  return nSize;
}

void DynBuffer::grow(int size) {
  int i;
  int newSize=nSize+size;
  char* tmp=(char*) malloc(sizeof(char)*(newSize+1));
  tmp[newSize]='\0';
  for(i=0;i<=nSize;i++) {
    tmp[i]=msg[i];
  }
 
  nSize=newSize;
  free(msg);
  msg=tmp;

}


int DynBuffer::find(char zeichen) {
  int i;
  int nlen=len();
  for(i=0;i<nlen;i++) {
    if (msg[i] == zeichen) return i;
  }
  return -1;
}



void DynBuffer::forward(int bytes) {
  int i;
  int aktPos;
  int nlen=len();
  if (bytes > nlen) {
    bytes=nlen;
  }
  i=0;
  aktPos=bytes;
  while(aktPos <= nlen) {
    msg[i]=msg[aktPos];
    i++;
    aktPos++;
  }
}



void DynBuffer::print() {
  printf("DynBuffer:%s\n",msg);
}
