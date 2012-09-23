/*
  reads paket input data
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "mpegSystemStream.h"

#define _RESYNC_STATE_INIT                0
#define _RESYNC_STATE_NEED_PACKET         1
#define _RESYNC_STATE_NEED_START          2

#include <iostream>

using namespace std;


MpegSystemStream::MpegSystemStream(InputStream* input) {
  this->input=input;
  syncCode=0xff;  // invalid
  lState=_RESYNC_STATE_INIT;
  tsSystemStream=new TSSystemStream(input);
  psSystemStream=new PSSystemStream(input);
  pesSystemStream=new PESSystemStream(input);
}


MpegSystemStream::~MpegSystemStream() {
  delete tsSystemStream;
  delete psSystemStream;
  delete pesSystemStream;
}


int MpegSystemStream::getByteDirect() {
  unsigned char byte;
  if (input->read((char*)&byte,1) != 1) {
    return -1;
  }
  return (int)byte;
}

int MpegSystemStream::readSyncCode() {
  int byte;
  byte=getByteDirect();
  if (byte==-1) {
    return false;
  } 

  syncCode<<=8;
  syncCode|=byte;
  syncCode&=0xffffffff;
  return true;

}

int MpegSystemStream::firstInitialize(MpegSystemHeader* mpegHeader) {

  if (readSyncCode() == false) {
    return false;
  }
  mpegHeader->setHeader(syncCode);
  
  // SEQ_START_CODE: 1b3
  if (mpegHeader->hasRAWHeader()) {
    // If we found a seq_heade_start code we assume
    // that we are a video only stream
    // we must directly insert it.

    // here we set for all future calls fixed
    // parameter
    mpegHeader->setLayer(_PACKET_NO_SYSLAYER);     // no syslayer
    mpegHeader->setPacketID(_PAKET_ID_VIDEO);
    mpegHeader->setPacketLen(8192);
    mpegHeader->resetAvailableLayers();
    return true;
  }
  // no 1bb
  if (syncCode == _SYSTEM_HEADER_START_CODE) return false;
  
  // TS 1ba 
  if (processSystemHeader(mpegHeader) == true) {
    // have init. we are a syslayer
    mpegHeader->setLayer(_PACKET_SYSLAYER);       // syslayer
    lState=_RESYNC_STATE_NEED_START;
    return true;
  }
  return false;

}

void MpegSystemStream::reset() {
  lState=_RESYNC_STATE_NEED_PACKET;
}


int MpegSystemStream::nextPacket(MpegSystemHeader* mpegHeader) {
  int layer=mpegHeader->getLayer();
  if (layer == _PACKET_NO_SYSLAYER) {
    return true;
  }
  // seek to packet
  if (readSyncCode() == false) {
    return false;
  }


  // default to "HLT" operation
  mpegHeader->setPacketID(_PAKET_ID_NUKE);
  mpegHeader->setPacketLen(0);


  // here we know that we are a SYSLAYER (TS or 1ba)
  if (lState == _RESYNC_STATE_NEED_PACKET) {
    // no 1bb codes in resync state
    if (syncCode == _SYSTEM_HEADER_START_CODE) return false;
  }
      
  // is this a syslayer _STARTCODE_ ?
  if (MpegSystemHeader::isSystemHeader(syncCode)==true) {
    // set startcode & process it
    mpegHeader->setHeader(syncCode);
    // TS 1ba 1bb
    if (processSystemHeader(mpegHeader) == true) {
      lState=_RESYNC_STATE_NEED_START;
      return true;
    }
    reset();
    return false;
  }
  
  if (lState != _RESYNC_STATE_NEED_START) return false;
  
  // no syslayer startcode. PES processing
  if ((syncCode & 0xFFFFFF00) == 0x00000100) {
    int bytes=pesSystemStream->processStartCode(syncCode,mpegHeader);
    if (bytes == 0) {
      reset();      
      return false;
    }
    // if we are a TS layer we cannot read getPESPacketLength byte
    // but only the bytes given by getTSPacketLength
    // additionally we must store the mapping between pid and real packetID.
    if (mpegHeader->hasTSHeader()) {
      unsigned int pid=mpegHeader->getPid();
      unsigned int id =mpegHeader->getPacketID();
      printf("current PID:%x current PacketID:%x\n",pid,id);
      MapPidStream* mapPidStream=mpegHeader->lookup(pid);
      mapPidStream->psType=id;
      mapPidStream->pesPacketSize=mpegHeader->getPESPacketLen();

      int ts=mpegHeader->getTSPacketLen();
      if (bytes > ts) {
	cout << "ERROR PES READ MORE than TS HAS"<<endl;
	return false;
      }
      mpegHeader->setTSPacketLen(ts-bytes);
      return demux_ts_pes_buffer(mpegHeader);
    }
    // 1ba stream
    mpegHeader->setPacketLen(mpegHeader->getPESPacketLen());
    return true;
  }  
  
  // something unknown/else went wrong
  return false;
}

int MpegSystemStream::demux_ts_pes_buffer(MpegSystemHeader* mpegHeader) {

  
  if (lState == _RESYNC_STATE_NEED_PACKET) {
    cout << "NO ts_pes because of resync"<<endl;
    return false;
  }
  MapPidStream* mapPidStream=mpegHeader->lookup(mpegHeader->getPid());
	
  int pes=mapPidStream->pesPacketSize;
  int ts=mpegHeader->getTSPacketLen();
  int wantRead=pes;

  // bug workaround for PES packetlength == 0
  if (pes > 0) {
    if (ts < wantRead) {
      cout << "TS is less setting wantRead:"<<ts<<endl;
      mapPidStream->pesPacketSize=pes-ts;
      wantRead=ts;
    } else {
      // pes < ts
      mpegHeader->setTSPacketLen(ts-pes);
      wantRead=pes;
    }
  } else {
    // in case of bug, always put the whole packet for pid in
    wantRead=ts;
  }
  mpegHeader->setPacketID(mapPidStream->psType);
  mpegHeader->setPacketLen(wantRead);
  return true;
}


int MpegSystemStream::processSystemHeader(MpegSystemHeader* mpegHeader) {
  if (mpegHeader->hasTSHeader()) {
    if (tsSystemStream->processStartCode(mpegHeader) == true) {
      if (mpegHeader->getPacketID() == _PAKET_ID_NUKE) {
	// nuke the data by signalling how much bytes to nuke
	mpegHeader->setPacketLen(mpegHeader->getTSPacketLen());
	return true;
      }  
      unsigned int pus=mpegHeader->getPayload_unit_start_indicator();
      if (pus == false) {
	// we need to append data
	return demux_ts_pes_buffer(mpegHeader);
      }
      return true;
    }
    return false;
  }
  if (mpegHeader->hasPSHeader()) {
   return (psSystemStream->processStartCode(mpegHeader));
  }
  return false;

}



