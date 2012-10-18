/*
  stores info about system stream and sends data to audio/video
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "mpegSystemHeader.h"

#include <iostream>

using namespace std;

// we do a fast mod based lookup pid->Map
#define MAX_PIDS 23


MpegSystemHeader::MpegSystemHeader() {

  packetLen=0;
  pesPacketLen=0;
  tsPacketLen=0;

  
  audioLayerSelect=0;
  videoLayerSelect=0;
  lmpeg2=false;
  lPTSFlag=false;
  
  layer=_PACKET_UNKNOWN_LAYER;
  lHasPSHeader=false;
  
  //
  //  MPEG2 Stuff [START]
  //
  
  lOriginal=false;
  lCopyRight=false;
  lDataAlignmentIndicator=false;
  lPesPriority=false;
  lEncrypted=false;
  startCodePrefix=0;
  
  lPTSDTSFlag=false;
  lESCR=false;
  lESRateFlag=false;
  lDMSTRICKFLAG=false;
  lADDITIONAL_COPY_FLAG=false;
  lPES_CRC_FLAG=false;
  lPES_EXT_FLAG=false;
  nPES_HEADER_DATA_LENGTH=0;
  
  // EXT FILED [START]
  
  lPrivateDataFlag=false;
  lPackHeaderFieldFlag=false;
  lSequenceCounterFlag=false;
  lSTDBufferFlag=false;
  lPES_EXT_FLAG_2=false;
  nPES_EXT_FIELD_LENGTH=0;
  // EXT FILED [END]
  
  subStreamID=0;
  //
  // MPEG2 Stuff [END]
  //
  
  // TS Stuff [START]
  lHasTSHeader=false;
  programs=0;
  programNumber=INVALID_PROGRAM;
  pmtPID=INVALID_PID;

  mapPidStreamArray=new MapPidStream*[MAX_PIDS];
  int i;
  for(i=0;i<MAX_PIDS;i++) {
    mapPidStreamArray[i]=new MapPidStream();
    mapPidStreamArray[i]->isValid=false;
  }
  currentPos=0;
  

  
  // TS Stuff [END]
}


MpegSystemHeader::~MpegSystemHeader() {
  int i;
  for(i=0;i<MAX_PIDS;i++) {
    delete mapPidStreamArray[i];
  } 
  delete mapPidStreamArray;
}


void MpegSystemHeader::setHeader(unsigned int header) {
  setPSHeader(header);
  setTSHeader(header);
  this->header=header;
}

int MpegSystemHeader::hasRAWHeader() {
  return header==_SEQ_START_CODE;
}


unsigned int MpegSystemHeader::getHeader() {
  return header;
}

double MpegSystemHeader::getSCRTimeStamp() {
  return scrTimeStamp;
}


double MpegSystemHeader::getPTSTimeStamp() {
  return ptsTimeStamp;
}


int MpegSystemHeader::getPTSFlag() {
  return lPTSFlag;
}


void MpegSystemHeader::setPTSFlag(int lPTSFlag) {
  this->lPTSFlag=lPTSFlag;
}


double MpegSystemHeader::getDTSTimeStamp() {
  return dtsTimeStamp;
}


void MpegSystemHeader::setSCRTimeStamp(double scrTimeStamp) {
  this->scrTimeStamp=scrTimeStamp;
}


void MpegSystemHeader::setPTSTimeStamp(double ptsTimeStamp) {
  this->ptsTimeStamp=ptsTimeStamp;
}


void MpegSystemHeader::setDTSTimeStamp(double dtsTimeStamp) {
  this->dtsTimeStamp=dtsTimeStamp;
}



int MpegSystemHeader::getLayer() {
  return layer;
}


void MpegSystemHeader::setLayer(int layer) {
  this->layer=layer;
}

int MpegSystemHeader::hasPSHeader() {
  return lHasPSHeader;
}


void MpegSystemHeader::addAvailableLayer(int streamID) {
  switch (streamID>>4){
  case _PAKET_ID_AUDIO_1>>4:
  case _PAKET_ID_AUDIO_2>>4:
    availableAudioLayers |= 1<<(streamID - 0xc0);
    break;
  case _PAKET_ID_VIDEO>>4:
    availableVideoLayers |= 1<<(streamID - 0xe0);
    break;
  case _SUBSTREAM_AC3_ID>>4:
    availableAudioLayers |= 1<<(streamID - 0x80);
    break;
  default:
    cout << "unknown streamID MpegSystemHeader::addAvailableLayer"<<endl;
  }


}
void MpegSystemHeader::resetAvailableLayers() {
  availableAudioLayers = 0;
  availableVideoLayers = 0;
}

int MpegSystemHeader::getAudioLayerSelect() {
  if (availableAudioLayers & (1<<audioLayerSelect))
    return audioLayerSelect;
  else
    return 0;
}

void MpegSystemHeader::setAudioLayerSelect(int layer) {
  audioLayerSelect = layer;
}

int MpegSystemHeader::getVideoLayerSelect() {
  if (availableVideoLayers & (1<<videoLayerSelect))
    return videoLayerSelect;
  else
    return 0;
}

void MpegSystemHeader::setVideoLayerSelect(int layer) {
  videoLayerSelect = layer;
}
 




int MpegSystemHeader::getPacketID() {
  return packetID;
}


void MpegSystemHeader::setPacketID(int packetID) {
  this->packetID=packetID;
}

int MpegSystemHeader::getPacketLen() {
  return packetLen;
}


void MpegSystemHeader::setPacketLen(int packetLen) {
  this->packetLen=packetLen;
}
 

int MpegSystemHeader::getPESPacketLen() {
  return pesPacketLen;
}

void MpegSystemHeader::setPESPacketLen(int packetLen) {
  this->pesPacketLen=packetLen;
}

  
int MpegSystemHeader::getTSPacketLen() {
  return tsPacketLen;
}


void MpegSystemHeader::setTSPacketLen(int packetLen) {
  this->tsPacketLen=packetLen;
}
  



int MpegSystemHeader::getRate() {
  return rate;
}


void MpegSystemHeader::setRate(int rate) {
  this->rate=rate;
}


int MpegSystemHeader::getStdBufferScale() {
  return stdBufferScale;
}


void MpegSystemHeader::setStdBufferScale(int stdBufferScale) {
  this->stdBufferScale=stdBufferScale;
}

unsigned long MpegSystemHeader::getStdBufferSize() {
  return stdBufferSize;
}


void MpegSystemHeader::setStdBufferSize(unsigned long stdBufferSize) {
  this->stdBufferSize=stdBufferSize;
}
 

int MpegSystemHeader::getMPEG2() {
  return lmpeg2;
}


//
//  MPEG2 Stuff [START]
//


int MpegSystemHeader::getOriginalOrCopy() {
  return lOriginal;
}


void MpegSystemHeader::setOriginalOrCopy(int lOriginal) {
  this->lOriginal=lOriginal;
}


int MpegSystemHeader::getCopyRight() {
  return lCopyRight;
}


void MpegSystemHeader::setCopyRight(int lCopyRight) {
  this->lCopyRight=lCopyRight;
}



int MpegSystemHeader::getDataAlignmentIndicator() {
  return lDataAlignmentIndicator;
}


void MpegSystemHeader::setDataAlignmentIndicator(int lDataAlignmentIndicator) {
  this->lDataAlignmentIndicator=lDataAlignmentIndicator;
}


int MpegSystemHeader::getPesPriority() {
  return lPesPriority;
}


void MpegSystemHeader::setPesPriority(int lPesPriority) {
  this->lPesPriority=lPesPriority;
}



int MpegSystemHeader::getEncrypted() {
  return lEncrypted;
}


void MpegSystemHeader::setEncrypted(int lEncrypted) {
  this->lEncrypted=lEncrypted;
}


int MpegSystemHeader::getStartCodePrefix() {
  return startCodePrefix;
}


void MpegSystemHeader::setStartCodePrefix(int startCodePrefix) {
  this->startCodePrefix=startCodePrefix;
}


int MpegSystemHeader::getPTSDTSFlag(){
  return lPTSDTSFlag;
}


void MpegSystemHeader::setPTSDTSFlag(int lPTSDTSFlag){
  this->lPTSDTSFlag=lPTSDTSFlag;
}


int MpegSystemHeader::getESCRFlag() {
  return lESCR;
}


void MpegSystemHeader::setESCRFlag(int lESCR) {
  this->lESCR=lESCR;
}


int MpegSystemHeader::getES_RATE_Flag() {
  return lESRateFlag;
}


void MpegSystemHeader::setES_RATE_Flag(int lESRateFlag) {
  this->lESRateFlag=lESRateFlag;
}



int MpegSystemHeader::getDMSTRICKFLAG(){
  return lDMSTRICKFLAG;
}


void MpegSystemHeader::setDMSTRICKFLAG(int lDMSTRICKFLAG) {
  this->lDMSTRICKFLAG=lDMSTRICKFLAG;
}


int MpegSystemHeader::getADDITIONAL_COPY_FLAG() {
  return lADDITIONAL_COPY_FLAG;
}


void MpegSystemHeader::setADDITIONAL_COPY_FLAG(int lADDITIONAL_COPY_FLAG) {
  this->lADDITIONAL_COPY_FLAG=lADDITIONAL_COPY_FLAG;
}



int MpegSystemHeader::getPES_CRC_FLAG() {
  return lPES_CRC_FLAG;
}


void MpegSystemHeader::setPES_CRC_FLAG(int lPES_CRC_FLAG) {
  this->lPES_CRC_FLAG=lPES_CRC_FLAG;
}


int MpegSystemHeader::getPES_EXT_FLAG() {
  return lPES_EXT_FLAG;
}


void MpegSystemHeader::setPES_EXT_FLAG(int lPES_EXT_FLAG) {
  this->lPES_EXT_FLAG=lPES_EXT_FLAG;
}


  //
  // PES EXTENSION [START]
  //

int MpegSystemHeader::getPrivateDataFlag() {
  return lPrivateDataFlag;
}


void MpegSystemHeader::setPrivateDataFlag(int lPrivateDataFlag) {
  this->lPrivateDataFlag=lPrivateDataFlag;
}


int MpegSystemHeader::getPackHeaderFieldFlag() {
  return lPackHeaderFieldFlag;
}


void MpegSystemHeader::setPackHeaderFieldFlag(int lPackHeaderFieldFlag) {
  this->lPackHeaderFieldFlag=lPackHeaderFieldFlag;
}


int MpegSystemHeader::getSequenceCounterFlag() {
  return lSequenceCounterFlag;
}


void MpegSystemHeader::setSequenceCounterFlag(int lSequenceCounterFlag) {
  this->lSequenceCounterFlag=lSequenceCounterFlag;
}



int MpegSystemHeader::getSTDBufferFlag() {
  return lSTDBufferFlag;
}


void MpegSystemHeader::setSTDBufferFlag(int lSTDBufferFlag) {
  this->lSTDBufferFlag=lSTDBufferFlag;
}



int MpegSystemHeader::getPES_EXT_FLAG_2() {
  return lPES_EXT_FLAG_2;
}


void MpegSystemHeader::setPES_EXT_FLAG_2(int lPES_EXT_FLAG_2) {
  this->lPES_EXT_FLAG_2=lPES_EXT_FLAG_2;
}


int MpegSystemHeader::getPES_EXT_FIELD_LENGTH() {
  return nPES_EXT_FIELD_LENGTH;
}


void MpegSystemHeader::setPES_EXT_FIELD_LENGTH(int nPES_EXT_FIELD_LENGTH) {
  this->nPES_EXT_FIELD_LENGTH=nPES_EXT_FIELD_LENGTH;
}



  //
  // PES EXTENSION [END]
  //



int MpegSystemHeader::getPES_HEADER_DATA_LENGTH() {
  return nPES_HEADER_DATA_LENGTH;
}


void MpegSystemHeader::setPES_HEADER_DATA_LENGTH(int nPES_HEADER_DATA_LENGTH){
  this->nPES_HEADER_DATA_LENGTH=nPES_HEADER_DATA_LENGTH;
}


int MpegSystemHeader::getSubStreamID() {
  return subStreamID;
}


void MpegSystemHeader::setSubStreamID(int subStreamID) {
  this->subStreamID=subStreamID;
}

//
// MPEG2 Stuff [END]
//


//
// Transport Stream Header [START]
//

int MpegSystemHeader:: hasTSHeader() {
  return lHasTSHeader;
}

void MpegSystemHeader::setTSHeader(unsigned int header) { 
 
  lHasTSHeader=false;

  int byte3=header & 0xff;
  header>>=8;
  int byte2=header & 0xff;
  header>>=8;
  int byte1=header & 0xff;
  header>>=8;
  int byte0=header & 0xff;


  sync_byte=byte0;
  header=header >> 8;
  transport_error_indicator = (byte1  >> 7) & 0x01;
  payload_unit_start_indicator = (byte1 >> 6) & 0x01;
  transport_priority = (byte1 >> 5) & 0x01;
  pid = ((byte1 << 8) | byte2) & 0x1fff;
  transport_scrambling_control = (byte3 >> 6)  & 0x03;
  adaption_field_control = (byte3 >> 4) & 0x03;
  continuity_counter  = byte3 & 0x0f;

  if (sync_byte != 0x47) {
    return ;
  }
  if (transport_error_indicator) {
    return ;
  }

  
  lHasTSHeader=true;

}

unsigned int MpegSystemHeader::getSync_byte() {
  return  sync_byte;
}


unsigned int MpegSystemHeader::getTransport_error_indicator() {
  return transport_error_indicator;
}


unsigned int MpegSystemHeader::getPayload_unit_start_indicator() {
  return  payload_unit_start_indicator;
}


unsigned int MpegSystemHeader::getTransport_priority() {
  return  transport_priority;
}


unsigned int MpegSystemHeader::getPid() {
  return  pid;
}


unsigned int MpegSystemHeader::getTransport_scrambling_control() {
  return  transport_scrambling_control;
}


unsigned int MpegSystemHeader::getAdaption_field_control() {
  return  adaption_field_control;
}


unsigned int MpegSystemHeader::getContinuity_counter() {
  return  continuity_counter;
}

unsigned int MpegSystemHeader::getPrograms() {
  return programs;
}


void MpegSystemHeader::setPrograms(unsigned int programs) {
  this->programs=programs;
}

 
unsigned int MpegSystemHeader::getProgramNumber() {
  return programNumber;
}


void MpegSystemHeader::setProgramNumber(unsigned int programNumber) {
  this->programNumber=programNumber;
}


unsigned int MpegSystemHeader::getPMTPID() {
  return pmtPID;
}


void MpegSystemHeader::setPMTPID(unsigned int pmtPID) {
  this->pmtPID=pmtPID;
}


MapPidStream* MpegSystemHeader::lookup(unsigned int pid) {
  return mapPidStreamArray[pid % MAX_PIDS];
}


/*
  Here we do a mapping from the pid to the "meaning" for the
  pid (defined in the TS spec) and then we map this to
  a normal mpeg stream ID.
  (Which is used in the MpegSystemHeder to identify the
  kind of packet we have)
*/
void MpegSystemHeader::insert(unsigned int pid,unsigned int tsType,
			      MpegSystemHeader* mpegHeader) {
  if (currentPos >= MAX_PIDS) {
    cout << "error to much pids in stream.TSSystemStream::insert"<<endl;
    return;
  }
  printf("tsType:%x\n",tsType);
  switch(tsType) {
  case ISO_11172_VIDEO:
  case ISO_13818_VIDEO:
  case ISO_11172_AUDIO:
  case ISO_13818_AUDIO:
  case ISO_13818_PRIVATE:
  case ISO_13818_PES_PRIVATE:
  case ISO_13522_MHEG:
  case ISO_13818_DSMCC:
  case ISO_13818_TYPE_A:
  case ISO_13818_TYPE_B:
  case ISO_13818_TYPE_C:
  case ISO_13818_TYPE_D:
  case ISO_13818_TYPE_E:
  case ISO_13818_AUX:
    break;
  default:
    cout << "ignoring unknown tsType in TSSystemStream::insert"<<endl;
    return;
  }
  
  MapPidStream* mapPidStream=lookup(pid);
  int psType=_PAKET_ID_NUKE;

  mapPidStream->pid=pid;
  mapPidStream->tsType=tsType;
  mapPidStream->psType=psType;
  mapPidStream->isValid=true;

  currentPos++;
}


void MpegSystemHeader::printTSHeader() {
  if (hasTSHeader() == false) {
    cout << "MpegSystemHeader::printTSHeader: NO TS HEADER"<<endl;
    return;
  }

  printf("sync:%02X TE:%02X PUS:%02X TP:%02X PID:%04X TSC:%02X "
	 "AFC:%02X CC:%02X\n",
	 sync_byte,
	 transport_error_indicator,
	 payload_unit_start_indicator,
	 transport_priority,
	 pid,
	 transport_scrambling_control,
	 adaption_field_control, 
	 continuity_counter); 
   
}


void MpegSystemHeader::printProgramInfo() {
  if (programs == 0) {
    cout << "MpegSystemHeader::printProgramInfo: NO programs"<<endl;
  } else {
    cout << "MpegSystemHeader::printProgramInfo: programs:"<<programs<<endl;
  }
  printf("MPTS: programNumber=%x pmtPid=%x\n",programNumber, pmtPID);

}

void MpegSystemHeader::printMap(MapPidStream* mapPidStream) {
  if (mapPidStream->isValid == false) {
    cout << "TSSystemStream::printMap ** INVALID** "<<endl;
    return;
  }

  printf("printMap->isValid:%x\n",mapPidStream->isValid);
  printf("printMap->pid:%x\n",mapPidStream->pid);
  printf("printMap->tsType:%x\n",mapPidStream->tsType);
  printf("printMap->psType:%x\n",mapPidStream->psType);
  
}


//
// Transport Stream Header [END]
//



void MpegSystemHeader::setMPEG2(int lmpeg2) {
  this->lmpeg2=lmpeg2;
}


void MpegSystemHeader::print() {
  cout << "MpegSystemHeader [START]"<<endl;
  cout << "layer:"<<getLayer()<<endl;
  cout << "MpegSystemHeader [END]"<<endl;
  
}


void MpegSystemHeader::setPSHeader(unsigned int header) {
  lHasPSHeader=false;
  if (header == _PACK_START_CODE) {
    lHasPSHeader=true;
  }
  if (header == _SYSTEM_HEADER_START_CODE) {
    lHasPSHeader=true;
  }
}

int MpegSystemHeader::isSystemHeader(unsigned int header) {

  if (header == _PACK_START_CODE) return true;
  if (header == _SYSTEM_HEADER_START_CODE) return true;

  int byte3=header & 0xff;
  header>>=8;
  int byte2=header & 0xff;
  header>>=8;
  int byte1=header & 0xff;
  header>>=8;
  int byte0=header & 0xff;

  if (byte0 != 0x47) return false;
  if ((byte1  >> 7) & 0x01) return false;

  return true;
}
