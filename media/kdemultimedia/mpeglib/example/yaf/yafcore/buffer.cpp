/*
  This class implements a dynamic string buffer
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "buffer.h"



Buffer::Buffer(int size) {
  nSize=size;
  msg=(char*) malloc(sizeof(char)*(nSize+1));
  msg[nSize]='\0';
  clear();

}


Buffer::~Buffer() {
  free (msg);
}


void Buffer::clear() {
  msg[0]='\0';
}

void Buffer::append(int value) {
  Buffer buf(30);
  sprintf(buf.getData(),"%d",value);
  append(buf.getData());
}




void Buffer::append(char* appendMsg) {
  char* appendPos=getAppendPos();
  int nlen=strlen(appendMsg);
  if (appendPos == NULL) return;
  
  append(appendMsg,nlen);
}
 

void Buffer::append(const char* appendMsg) {
  append((char*)appendMsg);
}

 
void Buffer::append(char* buffer,int buflen) {
  int nlen=len();
  int nBedarf;

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
 
char* Buffer::getAppendPos() {
  int i;
  // this Array has nSize+1 entries!
  // and it *is* granted that msg[nSize]=0;  (think so)
  for (i=0;i<=nSize;i++) {
    if (msg[i] == '\0') return &(msg[i]);
  }
  // should never reach this point
  return NULL;
}


void Buffer::setData(char* msg) {
  if (strlen(msg) == 0) {
    clear();
    return;
  }
  clear();
  append(msg);
}

char* Buffer::getData() {
  return msg;
}


int  Buffer::len() {
  return strlen(msg);
}  

int Buffer::getSize() {
  return nSize;
}

void Buffer::grow(int size) {
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


int Buffer::find(char zeichen) {
  int i;
  int nlen=len();
  for(i=0;i<nlen;i++) {
    if (msg[i] == zeichen) return i;
  }
  return -1;
}



void Buffer::forward(int bytes) {
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



void Buffer::print() {
  printf("Buffer:%s\n",msg);
}
