/*
  stores info about system stream and sends data to audio/video
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __MPEGSYSTEMHEADER_H
#define __MPEGSYSTEMHEADER_H



#define _PACKET_SYSLAYER            1
#define _PACKET_NO_SYSLAYER         0
#define _PACKET_UNKNOWN_LAYER      -1
#define _PACKET_HEADER_SIZE 8


// note: packetid 1 & 2 are normalized to _PAKET_ID_AUDIO
// see packet.cpp

#define _PAKET_ID_AUDIO_1    ((unsigned char) 0xc0)
#define _PAKET_ID_AUDIO_2    ((unsigned char) 0xd0)
#define _PAKET_ID_VIDEO      ((unsigned char) 0xe0)
#define _PAKET_ID_NUKE       ((unsigned char) 0x0)
  
#define _STD_SYSTEM_CLOCK_FREQ (unsigned long)90000
#define _MUX_RATE_SCALE_FACTOR 50
#define _MAX_STREAMS 8
#define _NOT_PACKET_ID       ((unsigned char) 0xff)
#define _KILL_BUFFER         ((unsigned char) 0xfe)


  
#define _STD_AUDIO_STREAM_ID          ((unsigned char) 0xb8)
#define _STD_VIDEO_STREAM_ID          ((unsigned char) 0xb9)
#define _MIN_STREAM_ID_ID             ((unsigned char) 0xbc)
#define _RESERVED_STREAM_ID           ((unsigned char) 0xbc)
#define _PRIVATE_STREAM_1_ID          ((unsigned char) 0xbd)
#define _PADDING_STREAM_ID            ((unsigned char) 0xbe)
#define _PRIVATE_STREAM_2_ID          ((unsigned char) 0xbf)
#define _ECM_STREAM_ID                ((unsigned char) 0xf0)
#define _EMM_STREAM_ID                ((unsigned char) 0xf1)
#define _PROGRAM_STREAM_DIRECTORY_ID  ((unsigned char) 0xff)
#define _DSMCC_STREAM_ID              ((unsigned char) 0xf2)
#define _ITUTRECH222TYPEE_STREAM_ID   ((unsigned char) 0xf8)
#define _SUBSTREAM_AC3_ID             ((unsigned char) 0x80)
#define _SUBSTREAM_LPCM_ID            ((unsigned char) 0xA0)
#define _SUBSTREAM_SUBPIC_ID          ((unsigned char) 0x20)


/* Silly Constants.... */
#define _PACK_START_CODE             ((unsigned int)0x000001ba)
#define _SYSTEM_HEADER_START_CODE    ((unsigned int)0x000001bb)
#define _PACKET_START_CODE_MASK      ((unsigned int)0xffffff00)
#define _PACKET_START_CODE_PREFIX    ((unsigned int)0x00000100)
#define _ISO_11172_END_CODE          ((unsigned int)0x000001b9)

#define _SEQ_START_CODE 0x000001b3


// More constants for TS streams 
#define NULL_PID 8191
#define INVALID_PID ((unsigned int)(-1))
#define INVALID_PROGRAM ((unsigned int)(-1))
#define INVALID_CC ((unsigned int)(-1))

#define ISO_11172_VIDEO          1
#define ISO_13818_VIDEO          2
#define ISO_11172_AUDIO          3
#define ISO_13818_AUDIO          4
#define ISO_13818_PRIVATE        5
#define ISO_13818_PES_PRIVATE    6
#define ISO_13522_MHEG           7
#define ISO_13818_DSMCC          8
#define ISO_13818_TYPE_A         9
#define ISO_13818_TYPE_B        10
#define ISO_13818_TYPE_C        11
#define ISO_13818_TYPE_D        12
#define ISO_13818_TYPE_E        13
#define ISO_13818_AUX           14


struct MapPidStream_s {
  // if this structure "is Valid" (association pid->tsType is ok)
  int isValid;
  // a not nearer defined int
  unsigned int pid;
  // the type for the pid defined in Transportstream
  unsigned int tsType;
  // runtime associated "fake" type for PS stream
  unsigned int psType;
  // outstanding bytes for PES packet
  unsigned int pesPacketSize;
};

typedef MapPidStream_s MapPidStream;





// More useful things
 
#define FLOAT_0x10000 (double)((unsigned long)1 << 16)
  
  
// MPEG2 PACK HEADER
// SCRbase and SCRext
//                  32 .......................................0 9.........0
// [PACK_START_CODE][01---1--][--------][-----1--][--------][-----1--][-------1

#define ui64 unsigned long
#define ui32 unsigned int
#define i64  long
#define i32  int

#define MPEG1_CLK_REF  (i64)90000
#define MPEG2_CLK_REF  (i64)27000000

#define ZERO                    (i64)0
#define GET_SCRBASE(buf) ( ((ui64)buf[4]&0xF8)>>3   |  \
                            (ui64)buf[3]<<5         |  \
                           ((ui64)buf[2]&0x03)<<13  |  \
                           ((ui64)buf[2]&0xF8)<<12  |  \
                            (ui64)buf[1]<<20        |  \
                           ((ui64)buf[0]&0x03)<<28  |  \
                            (ui64)buf[0]&0x38 <<27 )

#define GET_SCREXT(buf)  ( ((ui64)buf[5]&0xFE)>>1  |  \
                           ((ui64)buf[4]&0x03)<<7 )

// muxrate
//      22 ......................0         stl
// ... [--------][--------][------11][rrrrr---]
#define GET_MPEG2MUXRATE(buf) ( (ui32)buf[6]<<14  |      \
				((ui32)buf[7])<<6 |      \
				((ui32)buf[8]&0x03)>>2)  \
 
#define GET_MPEG2STUFFING(buf) ((buf[9]&0x07))
  
// MPEG1 PACK HEADER
//                      SCR                                            muxrate
//                     32........................................0    22......
//[PACK_START_CODE][0010---1][--------][-------1][--------][-------1][1-------]
#define GET_SCR(buf)   ( ((ui64)buf[4]&0xFE) >>1     | \
                         ((ui64)buf[3])      <<7     | \
                         ((ui64)buf[2]&0xFE) <<14    | \
                         ((ui64)buf[1])      <<22    | \
                         ((ui64)buf[0]&0x0E) <<29 )

#define GET_MPEG1_PTS(x) (GET_SCR(x)) //they follow the same pattern

#define GET_MPEG1_MUXRATE(x) ( ((ui32)x[7]&0xFE) >>1     | \
                               ((ui32)x[6])          <<7     | \
			       ((ui32)x[5]&0x7F) <<15 )

#define GET_MPEG2_PTS_FLAGS(buf) ( ((ui8)buf[3]&0xC0)>>6  )
// MPEG2 PES packet (optional parameters)
//                         PTS                      
//                         32........................................0
// [PACKET_START_CODE][001x---1][--------][-------1][--------][-------1]
#define GET_MPEG2_PTS(buf)   GET_MPEG1_PTS(buf)



#include <stdio.h>

  
class MpegSystemHeader {

  int lPTSFlag;
  double scrTimeStamp;
  double ptsTimeStamp;
  double dtsTimeStamp;

  int layer;
  unsigned int header;
  int lHasPSHeader;

  int lmpeg2;

  int packetID;
  int packetLen;

  int pesPacketLen;
  int tsPacketLen;
  
  unsigned long rate;
  int stdBufferScale;
  unsigned long stdBufferSize;

  /* This are bitmaps, if bit x is a 1 the stream number x is
     available */
  unsigned long availableAudioLayers;
  unsigned long availableVideoLayers;

  int audioLayerSelect;
  int videoLayerSelect;
  
  //
  // MPEG2 Header Info [START]
  //
  int lOriginal;
  int lCopyRight;
  int lDataAlignmentIndicator;
  int lPesPriority;
  int lEncrypted;
  int startCodePrefix;



  int lPTSDTSFlag;
  int lESCR;
  int lESRateFlag;
  int lDMSTRICKFLAG;
  int lADDITIONAL_COPY_FLAG;
  int lPES_CRC_FLAG;
  int lPES_EXT_FLAG;
  int nPES_HEADER_DATA_LENGTH;

  // EXT FILED [START]

  int lPrivateDataFlag;
  int lPackHeaderFieldFlag;
  int lSequenceCounterFlag;
  int lSTDBufferFlag;
  int lPES_EXT_FLAG_2;
  int nPES_EXT_FIELD_LENGTH;
  // EXT FILED [END]

  int subStreamID;
  //
  // MPEG2 Header Info [END]
  //

  //
  // Transport Stream Header [START]
  //

  int lHasTSHeader;
  
  unsigned int sync_byte;
  unsigned int transport_error_indicator;
  unsigned int payload_unit_start_indicator;
  unsigned int transport_priority;
  unsigned int pid;
  unsigned int transport_scrambling_control;
  unsigned int adaption_field_control;
  unsigned int continuity_counter;

  unsigned int programs;
  unsigned int programNumber;
  unsigned int pmtPID;

  MapPidStream** mapPidStreamArray;
  int currentPos;

  //
  // Transport Stream Header [END]
  //

 public:
  MpegSystemHeader();
  ~MpegSystemHeader();

  void setHeader(unsigned int header);
  unsigned int getHeader();
  int hasRAWHeader();


  int getPTSFlag();
  void setPTSFlag(int lPTSFlag);
    
  double getSCRTimeStamp();
  double getPTSTimeStamp();
  double getDTSTimeStamp();
  

  void setSCRTimeStamp(double timeStamp);
  void setPTSTimeStamp(double ptsTimeStamp);
  void setDTSTimeStamp(double dtsTimeStamp);

  // returns 1 if it is a syslayer 0 if non syslayer -1 if unknown
  int getLayer();
  void setLayer(int layer);
  int hasPSHeader();

  //
  //  MPEG2 Stuff [START]
  //

  int getMPEG2();
  void setMPEG2(int lmpeg2);

  // 1 Byte [Start]
  int getOriginalOrCopy();
  void setOriginalOrCopy(int lOriginal);

  int getCopyRight();
  void setCopyRight(int lCopyRight);

  int getDataAlignmentIndicator();
  void setDataAlignmentIndicator(int lDataAlignmentIndicator);

  int getPesPriority();
  void setPesPriority(int lPesPriority);

  int getEncrypted();
  void setEncrypted(int lEncrypted);

  int getStartCodePrefix();
  void setStartCodePrefix(int startCodePrefix);

  // 1. Byte [End]

  int getPTSDTSFlag();
  void setPTSDTSFlag(int lPTSDTSFlag);

  int getESCRFlag();
  void setESCRFlag(int lESCR);
 
  int getES_RATE_Flag();
  void setES_RATE_Flag(int lESRateFlag);
  

  int getDMSTRICKFLAG();
  void setDMSTRICKFLAG(int lDMSTRICKFLAG);

  int getADDITIONAL_COPY_FLAG();
  void setADDITIONAL_COPY_FLAG(int lADDITIONAL_COPY_FLAG);
  
  int getPES_CRC_FLAG();
  void setPES_CRC_FLAG(int lPES_CRC_FLAG);
    
  int getPES_EXT_FLAG();
  void setPES_EXT_FLAG(int lPES_EXT_FLAG);

  //
  // PES EXTENSION [START]
  //

  int getPrivateDataFlag();
  void setPrivateDataFlag(int lPrivateDataFlag);

  int getPackHeaderFieldFlag();
  void setPackHeaderFieldFlag(int lPackHeaderFieldFlag);
 
  int getSequenceCounterFlag();
  void setSequenceCounterFlag(int lSequenceCounterFlag);

  int getSTDBufferFlag();
  void setSTDBufferFlag(int lSTDBufferFlag);
  
  int getPES_EXT_FLAG_2();
  void setPES_EXT_FLAG_2(int lPES_EXT_FLAG_2);

  int getPES_EXT_FIELD_LENGTH();
  void setPES_EXT_FIELD_LENGTH(int nPES_EXT_FIELD_LENGTH);



  //
  // PES EXTENSION [END]
  //

  int getPES_HEADER_DATA_LENGTH();
  void setPES_HEADER_DATA_LENGTH(int nPES_HEADER_DATA_LENGTH);


  int getSubStreamID();
  void setSubStreamID(int subStreamID);


  //
  // MPEG2 Stuff [END]
  //


  //
  // Transport Stream Header [START]
  //

  // returns true if it is ts header & sets the TS
  // values in this class.
  int hasTSHeader();

  
  unsigned int getSync_byte();
  unsigned int getTransport_error_indicator();
  unsigned int getPayload_unit_start_indicator();
  unsigned int getTransport_priority();
  unsigned int getPid();
  unsigned int getTransport_scrambling_control();
  unsigned int getAdaption_field_control();
  unsigned int getContinuity_counter();

  unsigned int getPrograms();
  void setPrograms(unsigned int programs);
  
  unsigned int getProgramNumber();
  void setProgramNumber(unsigned int programNumber);
  
  unsigned int getPMTPID();
  void setPMTPID(unsigned int pmtPID);

  //
  // manage different pids and their tsTypes and the fake psTypes
  //

  MapPidStream* lookup(unsigned int pid);
  void insert(unsigned int pid,unsigned int tsType,
	      MpegSystemHeader* mpegHeader);


  void printTSHeader();
  void printProgramInfo();
  void printMap(MapPidStream* mapPidStream);

  //
  // Transport Stream Header [END]
  //


   
  void addAvailableLayer(int streamID);
  void resetAvailableLayers();
  
  int getAudioLayerSelect();
  void setAudioLayerSelect(int layer);

  int getVideoLayerSelect();
  void setVideoLayerSelect(int layer);


  int getPacketID();
  void setPacketID(int packetID);

  // data which can be safley read and belongs to packetID
  int getPacketLen();
  void setPacketLen(int packetLen);

  // length of PES packet (normally == getPacketLent)
  int getPESPacketLen();
  void setPESPacketLen(int packetLen);
  
  // rest lenght of TS packet.
  int getTSPacketLen();
  void setTSPacketLen(int packetLen);
  


  int getRate();
  void setRate(int rate);

  int getStdBufferScale();
  void setStdBufferScale(int stdBufferScale);
  
  unsigned long getStdBufferSize();
  void setStdBufferSize(unsigned long stdBufferSize);

  void print();

  // return true is system header in some kind (TS or 1ba/1bb)
  static int isSystemHeader(unsigned int header);

 private:
  void setTSHeader(unsigned int header);
  void setPSHeader(unsigned int header);
  
};

#endif



