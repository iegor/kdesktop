/*
  wraps an inputStream for the splayFileBuffer.
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "fileAccessWrapper.h"

#include <iostream>

using namespace std;

FileAccessWrapper::FileAccessWrapper(InputStream* input) {
  this->input=input;
}


FileAccessWrapper::~FileAccessWrapper() {
  
}


int FileAccessWrapper::open(const char*) {
  cout << "FileAccessWrapper open not implemented"<<endl;
  exit(0);
}


void FileAccessWrapper::close() {
  cout << "FileAccessWrapper close not implemented"<<endl;
  exit(0);
}


int FileAccessWrapper::read(char* dest,int len) {
  return input->read(dest,len);
}
						
int FileAccessWrapper::eof() {
  return input->eof();
}


int FileAccessWrapper::seek(long pos) {
  return input->seek(pos);
}


long FileAccessWrapper::getBytePosition() {
  return input->getBytePosition();
}


long FileAccessWrapper::getByteLength() {
  return input->getByteLength();
}
 
