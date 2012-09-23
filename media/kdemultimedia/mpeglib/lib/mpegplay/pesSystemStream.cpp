/*
  demux pes mpeg stream 
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "pesSystemStream.h"

#include <iostream>

using namespace std;

PESSystemStream::PESSystemStream(InputStream* input) {
  this->input=input;
}


PESSystemStream::~PESSystemStream() {
}



int PESSystemStream::getByteDirect() {
  unsigned char byte;
  if (input->read((char*)&byte,1) != 1) {
    return -1;
  }
  bytes_read++;
  return (int)byte;
}

int PESSystemStream::read(char* pointer,int bytes) {
  if (input->read(pointer,bytes) != bytes) {
    return false;
  }
  bytes_read+=bytes;
  
  return true;
}

int PESSystemStream::processStartCode(unsigned int startCode,
				      MpegSystemHeader* mpegHeader) {
  int lok=true;
  bytes_read=4;           // startcode
  mpegHeader->setPacketLen(0);
  mpegHeader->setPacketID(_PAKET_ID_NUKE);

  // handle default
  bytes_read=processPacket(startCode,mpegHeader);
  return bytes_read;
}


/* Returns:
   0 - no error, but not video packet we want
   -1 - error
   >0 - length of packet
*/
int PESSystemStream::processPacket(unsigned int startCode,
				    MpegSystemHeader* mpegHeader) {

  int ioBytes;
  unsigned short packetLength;
  int packetDataLength;

  /* Leftovers from previous video packets */

  int packetID=startCode & 0xff;
  mpegHeader->setPacketID(packetID);
  int lPacket=startCode & _PACKET_START_CODE_MASK &_PACKET_START_CODE_PREFIX;
  if ((lPacket == false) || (packetID < 0xbc)) {
    //printf("unknown startcode,packet or packetID:%8x\n",startCode);
    return false;
  }
  
  if (packetID == _NOT_PACKET_ID) {
    cout << "(vid_stream->mpegVideoStream)->makeEnd()"<<endl;
  } else if (packetID==_KILL_BUFFER) {
    printf("packetID==_KILL_BUFFER\n");
  }
  
  if (read((char*)&packetLength, 2) == false) return false;
  packetLength = htons(packetLength);

  mpegHeader->setPTSFlag(false);
  mpegHeader->setPacketID(packetID);
  mpegHeader->setPESPacketLen(packetLength);
  switch (packetID>>4) {
  case _PAKET_ID_AUDIO_1>>4:
  case _PAKET_ID_AUDIO_2>>4:
  case _PAKET_ID_VIDEO>>4:
    break;
  default:
    switch(packetID) {
    case _PRIVATE_STREAM_1_ID:
      break;
    default:
      switch (packetID) {
      case _PRIVATE_STREAM_2_ID:
      case _PADDING_STREAM_ID:
      case _RESERVED_STREAM_ID:
      case _ECM_STREAM_ID:
      case _EMM_STREAM_ID:
      case _PROGRAM_STREAM_DIRECTORY_ID:
      case _DSMCC_STREAM_ID:
      case _ITUTRECH222TYPEE_STREAM_ID:
	return bytes_read;
      }
      printf("\nUnknown packet type. (%x) at %ld\n",
	     packetID,input->getBytePosition());
      return bytes_read;
    }
  }
  // this is only processed if audio or video found
  
  if (mpegHeader->getMPEG2()==false) {
    packetDataLength = packetLength-processPacketHeader(mpegHeader);
  } else {
    int len=processMPEG2PacketHeader(mpegHeader);

    if (len < 0) {
      return false;
    }
    packetDataLength = packetLength-len;

    // now check in private stream for AC3
    if ( packetID == _PRIVATE_STREAM_1_ID ) {
      packetDataLength = packetDataLength-processPrivateHeader(mpegHeader);
    }
  }

  if (packetDataLength <= 0) {
    if (mpegHeader->hasPSHeader()) return false;
    // -> buggy TS stream
    packetDataLength=0;
  }
  mpegHeader->setPESPacketLen(packetDataLength);

  return bytes_read;

}


int PESSystemStream::processPrivateHeader(MpegSystemHeader* mpegHeader) {
  char nukeBuffer[30];
  int pos=0;
  int one=getByteDirect();  
  pos++;
  mpegHeader->setSubStreamID(one);
  switch(one>>4) {
  case _SUBSTREAM_AC3_ID>>4:
    if (read(nukeBuffer,3) == false) return false;
    mpegHeader->addAvailableLayer(one);
    cout << "addAvailableLayer:"<<one<<endl;
    pos+=3;
    break;
  case _SUBSTREAM_LPCM_ID>>4:
    if (read(nukeBuffer,6) == false) return false;
    pos+=6;
    break;
  case _SUBSTREAM_SUBPIC_ID>>4:
    if (read(nukeBuffer,3) == false) return false;
    pos+=3;
    break;
  default:
    printf("unknown sub id :%8x\n",one);
  }
  return pos;

}


int PESSystemStream::processMPEG2PacketHeader(MpegSystemHeader* mpegHeader){

  int stdCnt=0;
  int pos=0;

  // 1. Byte
  /*
    FROM FLASK:
    int getbits(2);
    encrypted = getbits(2);   // PES_scrambling_control
    getbits(4);
    //LIVID
    u_char original_or_copy                 : 1;
    u_char copyright                        : 1;
    u_char data_alignment_indicator         : 1;
    u_char pes_priority                     : 1;
    u_char pes_scrambling_control           : 2;
    u_char start_code_prefix                : 2;    // 0x02 

  */
  int first=getByteDirect();
  stdCnt++;
  mpegHeader->setOriginalOrCopy(first&(128)>>7);
  mpegHeader->setCopyRight(first&(64)>>6);
  mpegHeader->setDataAlignmentIndicator(first&(32)>>5);
  mpegHeader->setPesPriority(first&(16)>>4);
  mpegHeader->setEncrypted((first&(8+4))>>2);
  mpegHeader->setStartCodePrefix(first&(1+2));


  // 2. Byte
  /*
   PTS_DTS_flags = getbits(2);
   ESCR_flag = get1bit();
   ES_rate_flag = get1bit();
   DSM_trick_mode_flag = get1bit();
   additional_copy_info_flag = get1bit();
   PES_CRC_flag = get1bit();
   PES_extension_flag = get1bit();
  */
  int second=getByteDirect();
  stdCnt++;

  mpegHeader->setPTSDTSFlag((second&(128+64))>>6);
  mpegHeader->setESCRFlag((second&(32))>>5);
  mpegHeader->setES_RATE_Flag((second%(16))>>4);
  mpegHeader->setDMSTRICKFLAG((second&(8))>>3);
  mpegHeader->setADDITIONAL_COPY_FLAG((second&(4))>>2);
  mpegHeader->setPES_CRC_FLAG((second&(2))>>1);
  mpegHeader->setPES_EXT_FLAG(second&(1));


  // 3. Byte
  /*
    PES_header_data_length = getbits(8);
  */
  int third=getByteDirect();
  stdCnt++;
  mpegHeader->setPES_HEADER_DATA_LENGTH(third);

  
  //
  // PARSING MPEG 2 HEADER FLAGS [START]
  //  
  unsigned char nukeBuffer[300];

  int PTS_DTS_flags=mpegHeader->getPTSDTSFlag();
  if (PTS_DTS_flags == 0) {
    mpegHeader->setPTSFlag(false);
  } else {
    mpegHeader->setPTSFlag(true);
  }
    
  if (PTS_DTS_flags > 0x1) {
    if (read((char*)nukeBuffer,5) == false) return false;
    double pts=GET_MPEG2_PTS(nukeBuffer);
    pts=(pts*300.0)/(double)MPEG2_CLK_REF;
    mpegHeader->setPTSTimeStamp(pts);
    pos+=5;
  }
  if (PTS_DTS_flags > 0x2) {
    if (read((char*)nukeBuffer,5) == false) return false;
    double dts=GET_MPEG2_PTS(nukeBuffer);
    mpegHeader->setDTSTimeStamp((dts*300.0)/(double)MPEG2_CLK_REF);
    pos+=5;
  }

  int ESCRFlag=mpegHeader->getESCRFlag();
  if (ESCRFlag == 1){
    cout << "ESCRFlag == 1"<<endl;
    if (read((char*)nukeBuffer,6) == false) return false;
    pos+=6;
  }
  
  int ES_rate_flag=mpegHeader->getES_RATE_Flag();
  if (ES_rate_flag == 1){
    cout << "ES_rate_flag == 1"<<endl;
    if (read((char*)nukeBuffer,3) == false) return false;
    pos+=3;
  }

  int DSM_trick_mode_flag=mpegHeader->getDMSTRICKFLAG();
  if (DSM_trick_mode_flag == 1){
    cout << "DSM_trick_mode_flag == 1"<<endl;
    if (read((char*)nukeBuffer,1) == false) return false;
    pos++;
  }

  int additional_copy_info_flag=mpegHeader->getADDITIONAL_COPY_FLAG();
  if (additional_copy_info_flag  == 1) {
    cout << "additional_copy_info_flag  == 1"<<endl;
    if (read((char*)nukeBuffer,1) == false) return false;
    pos++;
  }

  int PES_CRC_flag=mpegHeader->getPES_CRC_FLAG();
  if (PES_CRC_flag == 1) {
    cout << "PES_CRC_flag == 1"<<endl;
    if (read((char*)nukeBuffer,2) == false) return false;
    pos+=2;
  }

  //
  // PES Extension [START]
  //

  int PES_extension_flag=mpegHeader->getPES_EXT_FLAG();
  if (PES_extension_flag == 1) {
    /*
      FLASK:
      PES_private_data_flag = get1bit();
      pack_header_field_flag = get1bit();
      program_packet_sequence_counter_flag = get1bit();
      PSTD_buffer_flag = get1bit();
      getbits(3);
      PES_extension_flag_2 = get1bit();
    */
    int extensionByte=getByteDirect();

    pos++;
    mpegHeader->setPrivateDataFlag((extensionByte&(128))>>7);
    mpegHeader->setPackHeaderFieldFlag((extensionByte&(64))>>6);
    mpegHeader->setSequenceCounterFlag((extensionByte&(32))>>5);
    mpegHeader->setSTDBufferFlag((extensionByte&(16))>>4);
    mpegHeader->setPES_EXT_FLAG_2(extensionByte&(1));

    int PES_private_data_flag=mpegHeader->getPrivateDataFlag();
    if (PES_private_data_flag == 1) {
      if (read((char*)nukeBuffer,128) == false) return false;
      pos+=128;
    }

    int pack_header_field_flag=mpegHeader->getPackHeaderFieldFlag();
    if (pack_header_field_flag == 1) {
      printf("pack header field flag value not allowed in program streams\n");
      return false;
    }

    int sequence_counter_flag=mpegHeader->getSequenceCounterFlag();
    if (sequence_counter_flag==1) {
      cout<<"sequence_counter_flag ==1"<<endl;
      if (read((char*)nukeBuffer,2) == false) return false;
      pos+=2;
    }

    int PSTD_buffer_flag=mpegHeader->getSTDBufferFlag();
    if (PSTD_buffer_flag==1) {
      if (read((char*)nukeBuffer,2) == false) return false;
      pos+=2;
    }
    
    int PES_extension_flag_2=mpegHeader->getPES_EXT_FLAG_2();
    if (PES_extension_flag_2 == 1) {
      int extension2_byte=getByteDirect();
      pos++;
      mpegHeader->setPES_EXT_FIELD_LENGTH(extension2_byte&(254));


      int PES_field_length=mpegHeader->getPES_EXT_FIELD_LENGTH();
      int j;
      for (j=0;j<PES_field_length ; j++) {
	cout << "PES_field_length (nuke)"<<endl;
	getByteDirect();
	pos++;
      }
    }
  }
  //
  // PES Extension [END]
  //

  // now nuke remaining bytes from PES DATA Length
  int PES_HEADER_DATA_LENGTH=mpegHeader->getPES_HEADER_DATA_LENGTH();
  int tmp=PES_HEADER_DATA_LENGTH-pos;
  if (tmp>0) {
    if (read((char*)nukeBuffer,tmp) == false) return false;
    pos+=tmp;
  }



  

  //
  // PARSING MPEG 2 HEADER FLAGS [START]
  //  

  int parsed=stdCnt+pos;
  return parsed;
  
}
   

int PESSystemStream::processPacketHeader(MpegSystemHeader* mpegHeader) {
  unsigned char nextByte;
  int pos;
  int val;
  unsigned char scratch[10];


  nextByte=getByteDirect();

  mpegHeader->setPTSFlag(false);

  pos = 1;
  while (nextByte & 0x80) {
    ++pos;
    val=getByteDirect();
    if (val == -1) return false;
    scratch[0]=val;
    
    nextByte=scratch[0];
  }
  if ((nextByte >> 6) == 0x01) {
    pos += 2;
    scratch[1]=getByteDirect();
    scratch[2]=getByteDirect();
    nextByte=scratch[2];
  } 
  if ((nextByte >> 4) == 0x02) {
    scratch[0] = nextByte;                     
    if (read((char*)&scratch[1],4) == false) return false;
    /* presentation time stamps */
    unsigned char hiBit;
    unsigned long low4Bytes;
    double ptsTimeStamp;
    double dtsTimeStamp=0.0;
    readTimeStamp((unsigned char*)scratch,&hiBit,&low4Bytes);
    makeClockTime(hiBit,low4Bytes,&ptsTimeStamp);
    mpegHeader->setPTSFlag(true);
    mpegHeader->setPTSTimeStamp(ptsTimeStamp);
    mpegHeader->setDTSTimeStamp(dtsTimeStamp);
    
    pos += 4;
  }
  else if ((nextByte >> 4) == 0x03) {
    scratch[0] = nextByte;                      
    if (read((char*)&scratch[1],9) == false) return false;
    /* presentation and decoding time stamps */
    unsigned char hiBit;
    unsigned long low4Bytes;
    double ptsTimeStamp;
    double dtsTimeStamp;
    readTimeStamp((unsigned char*)scratch,&hiBit,&low4Bytes);
    makeClockTime(hiBit,low4Bytes,&ptsTimeStamp);

    readTimeStamp((unsigned char*)&(scratch[5]),&hiBit,&low4Bytes);
    makeClockTime(hiBit,low4Bytes,&dtsTimeStamp);
    mpegHeader->setPTSFlag(true);
    mpegHeader->setPTSTimeStamp(ptsTimeStamp);
    mpegHeader->setDTSTimeStamp(dtsTimeStamp);
    
    pos += 9;
  } 
  return pos;
}



void PESSystemStream::readTimeStamp(unsigned char* inputBuffer,
				     unsigned char* hiBit,
				     unsigned long* low4Bytes) {
  *hiBit = ((unsigned long)inputBuffer[0] >> 3) & 0x01;
  *low4Bytes = (((unsigned long)inputBuffer[0] >> 1) & 0x03) << 30; 
  *low4Bytes |= (unsigned long)inputBuffer[1] << 22; 
  *low4Bytes |= ((unsigned long)inputBuffer[2] >> 1) << 15; 
  *low4Bytes |= (unsigned long)inputBuffer[3] << 7; 
  *low4Bytes |= ((unsigned long)inputBuffer[4]) >> 1; 
}


void PESSystemStream::readSTD(unsigned char* inputBuffer,
			       MpegSystemHeader* mpegHeader) {
  int stdBufferScale;
  unsigned long stdBufferSize;
  stdBufferScale = ((int)(inputBuffer[0] & 0x20) >> 5); 
  stdBufferSize = ((unsigned long)inputBuffer[0] & 0x1f) << 8;
  stdBufferSize |= (unsigned long)inputBuffer[1];
  mpegHeader->setStdBufferScale(stdBufferScale);
  mpegHeader->setStdBufferSize(stdBufferSize);
}


int PESSystemStream::makeClockTime(unsigned char hiBit, 
				    unsigned long low4Bytes,
				    double * clockTime) {
  if (hiBit != 0 && hiBit != 1) {
    *clockTime = 0.0;
    return 1;
  }
  *clockTime = (double)hiBit*FLOAT_0x10000*FLOAT_0x10000 + (double)low4Bytes;
  *clockTime /= (double)_STD_SYSTEM_CLOCK_FREQ;
  return 0;
}




