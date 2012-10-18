/*
  converts raw mpeg audio stream data into mpeg I encoded audio frames/packets
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "mpegAudioFrame.h"

#define MPEGAUDIOFRAMESIZE 4096

#define FRAME_SYNC            0
#define FRAME_CHECK_HEADER_1  1
#define FRAME_CHECK_HEADER_2  2

#include <iostream>

using namespace std;

MpegAudioFrame::MpegAudioFrame():Framer(MPEGAUDIOFRAMESIZE) {
  mpegAudioHeader=new MpegAudioHeader();
}


MpegAudioFrame::~MpegAudioFrame() {
  delete mpegAudioHeader;
}


void MpegAudioFrame::unsync(RawDataBuffer* store,int ) {
  // invalidate header in buffer
  unsigned char* start=store->ptr();
  start[0]=0x0;
  start[1]=0x0;
  store->setpos(0);
  find_frame_state=FRAME_SYNC;
  framesize=0;
}


/**
   here we parse byte by byte the raw input, first
   we search for the magic bytes: 0xfffx, then
   we read the remaining 2 bytes for the header,
   check if they are "wrong" in a mpeg I audio special way.
   If everything is fine, we continue with the next state.

   Note: we can be sure here, that the "store" can at least 
         store 4 bytes.
*/
int MpegAudioFrame::find_frame(RawDataBuffer* input_buffer,
			       RawDataBuffer* store_buffer) {
  unsigned char* store=store_buffer->current();
  int back=false;

  if (find_frame_state == FRAME_SYNC) {
    if (store_buffer->pos() != 0) {
      cout << "store buffer not at beginning MpegAudioFrame::find_frame"<<endl;
      cout << "current state requires this"<<endl;
      exit(0);
    }
  }
  
  while(input_buffer->eof()==false) {
    // search for sync bytes
    unsigned char* input=input_buffer->current();
    if (find_frame_state == FRAME_SYNC) {
      while(input_buffer->eof() == false) {
	input=input_buffer->current();
	// shift
	store[0]=store[1];
	store[1]=input[0];
	input_buffer->inc();

	if (store[0] != 0xff) {
	  continue;
	}
	// upper 4 bit are syncword, except bit one
	// which is layer 2.5 indicator.
	if ( (store[1] & 0xe0) != 0xe0) {
	  continue;
	}
	  
	// found header, now check if other info is ok, too.
	store_buffer->setpos(2);
	find_frame_state=FRAME_CHECK_HEADER_1;
	break;
      }
      // back to "main loop"
      continue;
    }
    // ok, try to read two other bytes and then validate the header
    if (find_frame_state == FRAME_CHECK_HEADER_1) {
      store[2]=input[0];
      input_buffer->inc();
      find_frame_state=FRAME_CHECK_HEADER_2;
      // back to main loop
      continue;
    }
    if (find_frame_state == FRAME_CHECK_HEADER_2) {
      store[3]=input[0];
      input_buffer->inc();
    }

    // ok, read 4 header bytes. Now check the validity of the 
    // header.
    int lHeaderOk;
    lHeaderOk=mpegAudioHeader->parseHeader(store);
    if (lHeaderOk == false) {
      find_frame_state=FRAME_SYNC;
      store_buffer->setpos(0);
      // go back to "sync for frame"
      continue;
    }
    framesize=mpegAudioHeader->getFramesize();
    // if framesize > max mepg framsize its an error
    if (framesize+4 >= store_buffer->size()) {
      find_frame_state=FRAME_SYNC;
      store_buffer->setpos(0);
      // go back to "sync for frame"
      continue;
    }
    // don't allow stupid framesizes:
    if (framesize <= 4) {
      find_frame_state=FRAME_SYNC;
      store_buffer->setpos(0);
      continue;
    }
    // yup! found valid header. forward states
    store_buffer->setpos(4);
    back=true;
    break;
  }
  return back;
}


/**
   here we read data until len=framesize.
   Then we go to the HAS_FRAME state.
*/
int MpegAudioFrame::read_frame(RawDataBuffer* input_buffer,
			       RawDataBuffer* store_buffer) {
  unsigned char* store=store_buffer->current();

  while(input_buffer->eof()==false) {
    int has=store_buffer->pos();
    int need=framesize-has;
    if(need == 0) {
      // yup! frame fully read. forward states.
      // go to end state for main_state.
      return true;
    }
    // try to do a memcpy for speed.
    int can=input_buffer->untilend();
    if(can > need) {
      can=need;
    }
    unsigned char* input=input_buffer->current();
    memcpy(store,input,can);
    store_buffer->inc(can);
    input_buffer->inc(can);
  }
  int need=framesize-store_buffer->pos();

  if(need == 0) {
    // yup! frame fully read. forward states.
    // go to end state for main_state.
    return true;
  }  
  return false;
}



void MpegAudioFrame::printPrivateStates() {
  cout << "MpegAudioFrame::printPrivateStates"<<endl;
  switch(find_frame_state) {
  case FRAME_SYNC:
    cout << "frame_state: FRAME_SYNC"<<endl;
    break;
  case FRAME_CHECK_HEADER_1:
    cout << "frame_state: FRAME_CHECK_HEADER_1"<<endl;
    break;
  case FRAME_CHECK_HEADER_2:
    cout << "frame_state: FRAME_CHECK_HEADER_2"<<endl;
    break;
  default:
    cout << "unknown illegal frame_state:"<<find_frame_state<<endl;
  }
  

}

