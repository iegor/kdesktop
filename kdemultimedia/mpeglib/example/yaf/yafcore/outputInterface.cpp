/*
  This class sends an output to the outputstream
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "outputInterface.h"
#include <iostream>

using namespace std;

OutputInterface::OutputInterface(ostream* out) {
  protocolSyntax=false;
  this->out=out;
  outBuffer=new Buffer(250);
  setlinebuf(stdout);
  pthread_mutex_init(&writeOutMut,NULL);
}


OutputInterface::~OutputInterface() {
  delete outBuffer;
}

void OutputInterface::flushBuffer() {
  if (protocolSyntax) {
    (*out) << outBuffer->getData() << endl;
#ifdef _DEBUG_OUTPUT
  ofstream outfile("outstream.dbg",ios::app);
  outfile << outBuffer->getData() << endl;
  outfile.flush();
  outfile.close();
#endif
  return;
  } else {
    (*out) << "Command:" << nr << " Msg:" << outBuffer->getData() << endl;
#ifdef _DEBUG_OUTPUT
  ofstream outfile("outstream.dbg",ios::app);
  outfile <<  "Command:" << nr << " Msg:" << outBuffer->getData() << endl;
  outfile.flush();
  outfile.close();
#endif
  }
  fflush(NULL);
}



void OutputInterface::setProtocolSyntax(int proto) {
  protocolSyntax=proto;
}


void OutputInterface::setNr(int nr) {
  this->nr=nr;
}

void OutputInterface::clearBuffer() {
  outBuffer->clear();
}


void OutputInterface::appendBuffer(const char* msg) {
  outBuffer->append(msg);
}



void OutputInterface::lock() {
  pthread_mutex_lock(&writeOutMut);
}


void OutputInterface::unlock() {
  pthread_mutex_unlock(&writeOutMut);
}

