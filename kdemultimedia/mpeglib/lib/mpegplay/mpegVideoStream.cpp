/*
  a mpegVideoStream buffer
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "mpegVideoStream.h"

#include <iostream>

using namespace std;

MpegVideoStream::MpegVideoStream(InputStream* input) {


  this->input=input;
  lHasStream=false;

  mpegSystemStream=new MpegSystemStream(input);
  mpegSystemHeader=new MpegSystemHeader();
  mpegVideoBitWindow= new MpegVideoBitWindow();

 
}


MpegVideoStream::~MpegVideoStream() {
  delete mpegSystemStream;
  delete mpegSystemHeader;
  delete mpegVideoBitWindow;
}




int MpegVideoStream::firstInitialize(MpegVideoHeader* mpegHeader) {
  if (lHasStream == false) {
    if (mpegSystemStream->firstInitialize(mpegSystemHeader)==false) {
      return false;
    }
    fill_videoBuffer(mpegSystemHeader);
    lHasStream=true;
  }
  // now find SEQ_START_CODE
  hasBytes(4);
  mpegVideoBitWindow->flushByteOffset();

  if (mpegSystemHeader->getLayer() == _PACKET_SYSLAYER) {
    unsigned int startCode=showBits(32);
    if (startCode != _SEQ_START_CODE) {
      flushBits(8);
      return false;
    }
    flushBits(32);
  }
  if (mpegHeader->parseSeq(this)==false) {
    return false;
  }
  return true;
}


int MpegVideoStream::isStartCode(unsigned int data) {

  switch(data) {
  case SEQ_END_CODE:
  case SEQ_START_CODE:
  case GOP_START_CODE:
  case PICTURE_START_CODE:
  case SLICE_MIN_START_CODE:
  case SLICE_MAX_START_CODE:
  case EXT_START_CODE:
  case USER_START_CODE:
  case SEQUENCE_ERROR_CODE:
  case SYSTEM_HEADER_START_CODE:   
  case ISO_11172_END_CODE:  
  case EOF:  
    return true;
  }
  if ((SLICE_MIN_START_CODE < data) && (data < SLICE_MAX_START_CODE)) {
    return true;
  }
  return false;
}

int MpegVideoStream::next_start_code() {
  flushByteOffset();
  
  unsigned int data;
  data=showBits(32);


  while(eof()==false) {
    data=showBits(32);
    if (isStartCode(data)) {
      return true;
    }
    flushBits(8);
  }
  return true;
}


int MpegVideoStream::nextGOP() {

  mpegVideoBitWindow->flushByteOffset();
  unsigned int data=showBits(32);
  if (data != GOP_START_CODE) {
    flushBits(8);
    return false;
  }
  return true;
}


int MpegVideoStream::nextPIC() {
  mpegVideoBitWindow->flushByteOffset();
  unsigned int data=showBits(32);
  if ( (data != PICTURE_START_CODE) &&
       (data != GOP_START_CODE) &&
       (data != SEQ_START_CODE) ) {
    flushBits(8);
    return false;
  }
  return true;
}





int MpegVideoStream::hasBytes(int byteCnt) {
  if (mpegVideoBitWindow->getLength() < byteCnt) {
    get_more_video_data();
    if (mpegVideoBitWindow->getLength() < byteCnt) {
      return hasBytes(byteCnt);
    }
  }
  return true;
}




int MpegVideoStream::eof() {
  if (input->eof()) {
    return true;
  }
  return false;
}


int MpegVideoStream::getByteDirect() {
  unsigned char byte;
  if (input->read((char*)&byte,1) != 1) {
    return -1;
  }
  return (int)byte;
}


int MpegVideoStream::get_more_video_data() {


  while(1) {
    if (mpegSystemStream->nextPacket(mpegSystemHeader)==false) {
      continue;
    }
    if (mpegSystemStream->eof() == true) {
      printf ("\n");
      mpegVideoBitWindow->fillWithIsoEndCode(1024);
      cout <<"Unexpected read error."<<endl;
      return false;
    }
    if (mpegSystemHeader->getPacketID() == _PAKET_ID_VIDEO) {
      break;
    }

  }

  
  fill_videoBuffer(mpegSystemHeader);
  return true;
}


void MpegVideoStream::fill_videoBuffer(MpegSystemHeader* mpegSystemHeader) {
  int bytes;

  bytes=mpegSystemHeader->getPacketLen();
  unsigned char* packetBuffer= new unsigned char[bytes];
  int didRead;
  didRead=input->read((char*)packetBuffer,bytes);
  if (bytes==0) {
    mpegVideoBitWindow->fillWithIsoEndCode(1024);
    return ;
  }

  mpegVideoBitWindow->appendToBuffer(packetBuffer,didRead);
  if (input->eof()) {
    mpegVideoBitWindow->fillWithIsoEndCode(bytes-didRead);
  }

  delete packetBuffer;
}


TimeStamp* MpegVideoStream::getCurrentTimeStamp() {
  long pos=input->getBytePosition();
  int transfered=4*mpegVideoBitWindow->getLength();
  pos=pos-transfered;

  TimeStamp* timeStamp=input->getTimeStamp(pos);

  return timeStamp;
}

