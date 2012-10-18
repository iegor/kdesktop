/*
  base class for converting raw data(stream) into some frametype.
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "framer.h"



#define PROCESS_FIND          0
#define PROCESS_READ          1

#include <iostream>

using namespace std;

Framer::Framer(int outsize) {
  unsigned char* outptr=new unsigned char[outsize];
  init(outsize,outptr,true);
}


Framer::Framer(int outsize,unsigned char* outptr) {
  if (outptr == NULL) {
    cout << "Framer::Framer outptr NULL"<<endl;
    exit(0);
  }
  if (outsize <= 0) {
    cout << "Framer::Framer size <= 0"<<endl;
    exit(0);
  }
  init(outsize,outptr,false);
}


void Framer::init(int outsize,unsigned char* outptr,int lDeleteOutPtr) {
  buffer_data=outptr;
  this->lDeleteOutPtr=lDeleteOutPtr;
  input_info=new RawDataBuffer(NULL,0);
  buffer_info=new RawDataBuffer(buffer_data,outsize);
  lConstruct=true;
  reset();
}


Framer::~Framer() {
  if (lDeleteOutPtr) {
    delete[] buffer_data;
  }
  delete buffer_info;
  delete input_info;
}


int Framer::canStore() {
  return buffer_info->untilend();
}

int Framer::restBytes() {
  return (input_info->size()-input_info->pos());
}


void Framer::store(unsigned char* start,int bytes) {
  if (buffer_info->pos()+bytes > buffer_info->size()) {
    cout << "too much bytes inserted. cannot store that"<<endl;
    exit(0);
  }
  if (main_state != FRAME_NEED) {
    cout << "cannot store data, when not in MPEGAUDIOFRAME_NEED"<<endl;
    exit(0);
  }
  input_info->set(start,bytes,0);
  int fillgrade=input_info->untilend();
  if (fillgrade > 0) {
    main_state=FRAME_WORK;
  }
}


int Framer::work() {
  if (main_state != FRAME_WORK) {
    cout << "cannot find_frame, when not in MPEGAUDIOFRAME_WORK"<<endl;
    exit(0);
  }
  if (lAutoNext) {
    next();
  }
  switch(process_state) {
  case PROCESS_FIND:
    if (find_frame(input_info,buffer_info) == true) {
      setState(PROCESS_READ);
    }
    break;
  case PROCESS_READ:
    if (read_frame(input_info,buffer_info) == true) {
      main_state=FRAME_HAS;
    }
    break;
  default:
    cout << "unknown process state in work.  "<<endl;
    printMainStates("printing states");
    exit(0);
  }
  // do not go to NEED if we have a frame and now the input is empty
  if (main_state == FRAME_WORK) {
    if (input_info->eof()) {
      main_state=FRAME_NEED;
    }
  }
  if (main_state == FRAME_HAS) return true;
  return false;
}


void Framer::reset() {
  unsync(buffer_info,true);
  lAutoNext=false;
  main_state=FRAME_NEED;
  input_info->set(NULL,0,0);
  buffer_info->setpos(0);
  setState(PROCESS_FIND);
}


void Framer::next() {
  unsync(buffer_info,false);
  lAutoNext=false;
  main_state=FRAME_WORK;
  setState(PROCESS_FIND);
}


int Framer::getState() {
  int back=main_state;
  if (main_state == FRAME_HAS) {
    // autonext when we devlivered one frame
    lAutoNext=true;
    main_state=FRAME_WORK;
    setState(PROCESS_FIND);
  }
  if (lConstruct == true) {
    lConstruct=false;
    unsync(buffer_info,true);
  }
  return back;
}


void Framer::setState(int state) {
  this->process_state=state;
}



unsigned char* Framer::outdata() {
  return buffer_info->ptr();
}


unsigned char* Framer::indata() {
  return buffer_info->current();
}

int Framer::len() {
  return buffer_info->pos();
}



void Framer::printMainStates(const char* msg) {
  cout << msg<<endl;
  switch(main_state) {
  case FRAME_NEED:
    cout << "main_state: FRAME_NEED"<<endl;
    break;
  case FRAME_WORK:
    cout << "main_state: FRAME_WORK"<<endl;
    break;
  case FRAME_HAS:
    cout << "main_state: FRAME_HAS"<<endl;
    break;
  default:
    cout << "unknown illegal main_state:"<<main_state<<endl;
  }

  switch(process_state) {
  case PROCESS_FIND:
    cout << "process_state: PROCESS_FIND"<<endl;
    break;
  case PROCESS_READ:
    cout << "process_state: PROCESS_READ"<<endl;
    break;
  default:
    cout << "unknown illegal process_state:"<<process_state<<endl;
  }
  printPrivateStates();
  

}




int Framer::find_frame(RawDataBuffer* ,RawDataBuffer* ) {
  cout << "direct virtual call Framer::find_frame"<<endl;
  return false;
}


int Framer::read_frame(RawDataBuffer* ,RawDataBuffer* ) {
  cout << "direct virtual call Framer::read_frame"<<endl;
  return false;
}

void Framer::unsync(RawDataBuffer* ,int ) {
  if (lConstruct == false) {
    // invalidate header in buffer
    cout << "direct virtual call Framer::unsync"<<endl;
  }
}


void Framer::printPrivateStates() {
  cout << "direct virtual call Framer::printPrivateStates"<<endl;
}


void Framer::setRemoteFrameBuffer(unsigned char* outptr,int size) {

  input_info->set(NULL,0,0);
  buffer_info->set(outptr,size,0);
}
