/*
  C interface creator for input_plugins
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "inputPlugin.h"

#include <iostream>

using namespace std;


InputPlugin::InputPlugin() {
}


InputPlugin::~InputPlugin() {
}



int InputPlugin::getInputType(const char* dest) {
  return InputDetector::getInputType(dest);
}


InputStream* InputPlugin::createInputStream(int inputType) {
  // make checks which input routine to use
  InputStream* inputStream;
  int method;

  inputStream=NULL;
  method=inputType;

  switch(method) {
  case __INPUT_FILE: {
    inputStream=new FileInputStream();
    break;
  }
  case __INPUT_CDDA: {
    inputStream=new CDDAInputStream();
    break;
  }

  case __INPUT_HTTP: {
    inputStream=new HttpInputStream();
    break;
  }
  case __INPUT_CDI: {
    inputStream=new CDRomInputStream();
    break;
  }  
  default:
    cout << "error cannot create default input stream"<<endl;
    exit(0);
  }

  return inputStream;
}

InputStream* InputPlugin::createInputStream(int inputType,int lThreadSafe) {
  InputStream* input=InputPlugin::createInputStream(inputType);
  if (lThreadSafe == false) {
    return input;
  }
  InputStream* tsInput=new ThreadSafeInputStream(input);
  return tsInput;
}


InputStream* InputPlugin::createInputStream(const char* dest) {
  int method;

  method=InputPlugin::getInputType(dest);
  return (InputPlugin::createInputStream(method));
}


InputStream* InputPlugin::createInputStream(const char* dest,int lThreadSafe) {
  int method;

  method=InputPlugin::getInputType(dest);
  return (InputPlugin::createInputStream(method,lThreadSafe));
}
