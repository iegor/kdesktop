/*
  this plugin nukes the input data. 
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "nukePlugin.h"

#include <iostream>

using namespace std;

NukePlugin::NukePlugin() {
}


NukePlugin::~NukePlugin() {
}



void NukePlugin::decoder_loop() {
  if (input == NULL) {
    cout << "NukePlugin::decoder_loop input is NULL"<<endl;
    exit(0);
  }
  if (output == NULL) {
    cout << "NukePlugin::decoder_loop output is NULL"<<endl;
    exit(0);
  }
  char nukeBuffer[8192];

  while(runCheck()) {
    
    switch(streamState) {
    case _STREAM_STATE_FIRST_INIT :
    case _STREAM_STATE_INIT :
    case _STREAM_STATE_PLAY :
      input->read(nukeBuffer,8192);
      break;
    case _STREAM_STATE_WAIT_FOR_END:
      // exit while loop
      cout << "nukePlugin _STREAM_STATE_WAIT_FOR_END"<<endl;
      lDecoderLoop=false;
      break;
    default:
      cout << "unknown stream state:"<<streamState<<endl;
    }
  }
}


void NukePlugin::config(const char* key,const char* value,void* user_data) {
  DecoderPlugin::config(key,value,user_data);
}
