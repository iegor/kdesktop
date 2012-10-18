/*
  frames raw data into Ogg/Vorbis frames.
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "ovFramer.h"

#define OGG_SYNC_BUFF_SIZE      4096


#define _OV_SETSERIAL      1
#define _OV_STREAMIN       2
#define _OV_STREAMOUT      3

#include <iostream>

using namespace std;


#ifdef OGG_VORBIS

OVFramer::OVFramer(OGGFrame* dest):Framer(1) {
  if (dest == NULL) {
    cout << "OGGFrame NULL in OVFramer"<<endl;
    exit(-1);
  }
  this->dest=dest;
  /********** Decode setup ************/
  ogg_sync_init(&oy); /* Now we can read pages */

  vorbis_state=_OV_SETSERIAL;
}


OVFramer::~OVFramer() {
  /* OK, clean up the framer */
  ogg_sync_clear(&oy);
}


int OVFramer::find_frame(RawDataBuffer* input,RawDataBuffer* store) {
  while(input->eof()==true) {
    cout << "input eof"<<endl;
    return false;
  }
  
  if (vorbis_state == _OV_STREAMOUT) {
    if(ogg_stream_packetout(&os,(ogg_packet*)dest->getData())!=1){ 
      vorbis_state=_OV_STREAMIN;
      return false;
    }
    return true;
  }

  // do we have ogg packets in the ogg framer?
  if (ogg_sync_pageout(&oy,&og) == 0) {
    // no, ok insert some.
    int bytes=input->untilend();
    input->inc(bytes);
    store->inc(bytes);
    ogg_sync_wrote(&oy,bytes);
    // and setup the next buffer
    /* submit a 4k block to libvorbis' Ogg layer */
    buffer=ogg_sync_buffer(&oy,OGG_SYNC_BUFF_SIZE);
    /* override our own dummy buffer with size 1 */
    setRemoteFrameBuffer((unsigned char*)buffer,OGG_SYNC_BUFF_SIZE);
    return false;
  }
  // we have an ogg page
  // now try to build an ogg packet
  switch(vorbis_state) {
  case _OV_SETSERIAL:
    /* Get the serial number and set up the rest of decode. */
    /* serialno first; use it to set up a logical stream */
    ogg_stream_init(&os,ogg_page_serialno(&og));
    vorbis_state=_OV_STREAMIN;
    // yes we need to put this into the "pager"
    // no break.
  case _OV_STREAMIN:
    if(ogg_stream_pagein(&os,&og)<0){ 
      /* error; stream version mismatch perhaps */
      fprintf(stderr,"Error reading first page of Ogg bitstream data.\n");
      exit(1);
    }
    vorbis_state=_OV_STREAMOUT;
    break;
  default:
    cout << "unknow vorbis_state"<<endl;
    exit(-1);
  }
  
  return false;
}

int OVFramer::read_frame(RawDataBuffer* ,RawDataBuffer* ) {
  return true;
}
    
    

void OVFramer::unsync(RawDataBuffer* store,int lReset) {
  if (lReset) {
    store->setpos(0);
    ogg_sync_reset(&oy);
    /* submit a 4k block to libvorbis' Ogg layer */
    buffer=ogg_sync_buffer(&oy,OGG_SYNC_BUFF_SIZE);
    /* override our own dummy buffer with size 1 */
    setRemoteFrameBuffer((unsigned char*)buffer,OGG_SYNC_BUFF_SIZE);
  }

}


void OVFramer::printPrivateStates() {
  cout << "OVFramer::printPrivateStates"<<endl;
}


#endif
