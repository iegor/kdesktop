/*
  generic output plugin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "outPlugin.h"

#include <iostream>

using namespace std;


OutPlugin::OutPlugin() {
}


OutPlugin::~OutPlugin() {
}



OutputStream* OutPlugin::createOutputStream(int outputType) {

  // make checks which input routine to use
  OutputStream* outputStream;
  int method;

  outputStream=NULL;
  method=outputType;

  switch(method) {
  case _OUTPUT_LOCAL: {
    outputStream=new DspX11OutputStream(1024*64);
    break;
  }
  case _OUTPUT_ARTS: {
    outputStream=new ArtsOutputStream(NULL);
    break;
  }
  case _OUTPUT_EMPTY: {
    outputStream=new OutputStream();
    break;
  }
  default:
    cout << "error cannot create default output stream"<<endl;
    exit(0);
  }

  return outputStream;


}

OutputStream* OutPlugin::createOutputStream(int outputType,int lThreadSafe) {
  OutputStream* output=OutPlugin::createOutputStream(outputType);
  if (lThreadSafe==false) {
    return output;
  }
  OutputStream* tsOutput=new ThreadSafeOutputStream(output);
  return tsOutput;
}
 
