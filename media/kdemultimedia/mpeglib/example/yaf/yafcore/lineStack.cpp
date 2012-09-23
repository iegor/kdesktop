/*
  a class which scans a string and converts it into commandLines
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "lineStack.h"
#include <iostream>

using namespace std;


LineStack::LineStack() {
  stack=new Buffer(1);
}


LineStack::~LineStack() {
  delete stack;
}


int LineStack::hasLine() {
  int nPos;
  nPos=stack->find('\n');
  if (nPos == -1) return false;
  return true;

}



void LineStack::nextLine(Buffer* nextLine) {
  int nPos;
  char* retPos;
  int restLen;
  char* data=stack->getData();
  int nSize=stack->getSize();

  nPos=stack->find('\n');
  if (nPos == -1) {
    nextLine->clear();
    return;
  }
  retPos=&(data[nPos]);
  (*retPos)='\0';
  nextLine->clear();
  nextLine->setData(data);
  retPos++;
  restLen=nSize+1-(nPos+1);
  if (strlen(retPos) > 0) {
    strncpy(data,retPos,restLen);
  } else{
    stack->clear();
  }
}


void LineStack::appendBottom(char* buffer) {
  int n=strlen(buffer);
  appendBottom(buffer,n);
}



void LineStack::appendBottom(char* buffer, int buflen) {

  stack->append(buffer,buflen);

}

void LineStack::appendBottom(LineStack* lStack) {
  char* data;
  int len;
  data=lStack->stack->getData();
  len=lStack->stack->len();
  appendBottom(data,len); 
}

  
  
void LineStack::print(char* name) {
  cout << "LineStack:"<<name<<endl;
  stack->print();
}

  
