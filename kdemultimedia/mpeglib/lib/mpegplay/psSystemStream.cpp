/*
  demux "normal" mpeg stream (does this have a name?)
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "psSystemStream.h"

#include <iostream>

using namespace std;

PSSystemStream::PSSystemStream(InputStream* input) {
  this->input=input;
}


PSSystemStream::~PSSystemStream() {
}


int PSSystemStream::read(char* ptr,int bytes) {
  if (input->read(ptr,bytes) != bytes) {
    return false;
  }
  paket_read+=bytes;
  
  return true;
}

int PSSystemStream::getByteDirect() {
  unsigned char byte;
  if (input->read((char*)&byte,1) != 1) {
    return -1;
  }
  paket_read++;
  return (int)byte;
}


// nuke bytes modulo 10
int PSSystemStream::nukeBytes(int bytes) {
  // nukebuffer
  char nuke[10];

  while(bytes > 0) {
    int doNuke=10;
    if (bytes < 10) doNuke=bytes;
    if (input->read((char*)&nuke,doNuke) != doNuke) {
      return false;
    }
    bytes-=doNuke;
    paket_read+=doNuke;
  }
  return true;
}


int PSSystemStream::processStartCode(MpegSystemHeader* mpegHeader) {
  unsigned int header=mpegHeader->getHeader();
  mpegHeader->setPacketLen(0);
  mpegHeader->setPacketID(_PAKET_ID_NUKE);

  switch(header) {
  case _PACK_START_CODE:
    return processPackHeader(mpegHeader);
  case _SYSTEM_HEADER_START_CODE:
    return processSystemHeader(mpegHeader);
  default:
    cout << "PSSystemStream::processStartCode unknown PS header"<<endl;
    exit(-1);
  }
  // never
  return false;
}


int PSSystemStream::processPackHeader(MpegSystemHeader* mpegHeader) {
  int back=false;
  int numRead;
  unsigned long scrbase;
  unsigned long scrext;
  unsigned long rate;

  double timeStamp;

  unsigned char inputBuffer[_PACKET_HEADER_SIZE+2];
  if (read((char*)inputBuffer, _PACKET_HEADER_SIZE) == false) return false;


  // only set this if we do an initialize
  int layer=mpegHeader->getLayer();
  if (layer == _PACKET_UNKNOWN_LAYER) {
    if ((inputBuffer[0]>>6)==1) {
      mpegHeader->setMPEG2(true);
    }
  }

  if (mpegHeader->getMPEG2()) {
    if (read((char*)inputBuffer+_PACKET_HEADER_SIZE, 2) == false) return false;

    scrbase=GET_SCRBASE(inputBuffer);
    scrext=GET_SCREXT(inputBuffer);

    // BUGGY:
    timeStamp = (double)(scrbase*300 + scrext) / (double)MPEG2_CLK_REF;
    
    rate=GET_MPEG2MUXRATE(inputBuffer);
    
    int stuffing = GET_MPEG2STUFFING(inputBuffer);  // always <= 7

    numRead=stuffing;
    if (stuffing) {
      if (read((char*)inputBuffer, stuffing) == false) return false;
    }
  } else {

    // MPEG 1
    scrbase=GET_SCR(inputBuffer);
    timeStamp=(double)scrbase/MPEG1_CLK_REF;
    rate=GET_MPEG1_MUXRATE(inputBuffer);
    rate*=_MUX_RATE_SCALE_FACTOR;
  }
  mpegHeader->setSCRTimeStamp(timeStamp);
  mpegHeader->setRate(rate);


  return true;
}


int PSSystemStream::processSystemHeader(MpegSystemHeader* mpegHeader) {
  unsigned char *inputBuffer = NULL;
  int numRead;
  unsigned short headerSize;

  
  if (read((char *)&headerSize, 2) == false) return false;
  headerSize = ntohs(headerSize);

  inputBuffer = (unsigned char*) malloc (sizeof(unsigned char)*(headerSize+1));
  inputBuffer[headerSize]=0;
  if (read((char*)inputBuffer, headerSize) == false) return false;

  mpegHeader->resetAvailableLayers();
  int i = 6;
  while (i<headerSize) {
    if (inputBuffer[i] & 0x80) {
      mpegHeader->addAvailableLayer(inputBuffer[i]);
    }
    i += 3;
  }
  free(inputBuffer);
  return true;
}

