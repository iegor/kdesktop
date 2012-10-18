/*
  demux transport stream
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "tsSystemStream.h"


#define PKT_SIZE 188


TSSystemStream::TSSystemStream(InputStream* input) {
  this->input=input;

}


TSSystemStream::~TSSystemStream() {
}

int TSSystemStream::read(char* ptr,int bytes) {
  if (input->read(ptr,bytes) != bytes) {
    return false;
  }
  paket_read+=bytes;
  
  return true;
}

int TSSystemStream::getByteDirect() {
  unsigned char byte;
  if (input->read((char*)&byte,1) != 1) {
    return -1;
  }
  paket_read++;
  return (int)byte;
}


// nuke bytes modulo 10
int TSSystemStream::nukeBytes(int bytes) {
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


int TSSystemStream::skipNextByteInLength() {
  int length=getByteDirect();
  if (length < 0) return false;

  /*
   * Skip read byte in length, but check paket_size
   */
  if (paket_read+length > PKT_SIZE) {
    printf ("demux error! invalid payload size %d\n",length);
    return false;
  }
  if (nukeBytes(length) == false) return false;
  return true;
}




int TSSystemStream::processStartCode(MpegSystemHeader* mpegHeader) {
  paket_len=PKT_SIZE;  
  paket_read=4;                     // startcode=4 bytes

  mpegHeader->setTSPacketLen(0);
  mpegHeader->setPacketID(_PAKET_ID_NUKE);

  unsigned int pid=mpegHeader->getPid();
  unsigned int pmtPID=mpegHeader->getPMTPID();
  if ( (pmtPID == INVALID_PID) && (pid != 0)) {
    return false;
  }

  if ((mpegHeader->getAdaption_field_control() & 0x1)==0) {
    return true;
  }

  /*
   * Has a payload! Calculate & check payload length.
   */
  if (mpegHeader->getAdaption_field_control() & 0x2) {
    if (skipNextByteInLength() == false) return false;
  }
  
  /*
   * Do the demuxing in based on the pids
   */

  if (pid == mpegHeader->getPMTPID()) {
    return demux_ts_pmt_parse(mpegHeader);
  }

  if (pid == 0) {
    return demux_ts_pat_parse(mpegHeader);
  }
  //
  // ok, no the only things left to do is the
  // decision what to do with the packet
  // 
  
  mpegHeader->setTSPacketLen(paket_len-paket_read);

  if (pid == 0x1fff) {
    printf("Nuke Packet\n");
    return true;
  }
  

  MapPidStream* mapPidStream=mpegHeader->lookup(pid);

  if (mapPidStream->isValid == true) {
    // set to something different from "NUKE_PAKET"
    mpegHeader->setPacketID(_PAKET_ID_AUDIO_1);
    return true;
  }
  // well the raw stream has a TS header and a PID, but we have not a valid
  // mapping pid->tsType.
  // we return false here to have a recovery if our
  // previous decision that we actually have a TS stream was wrong.
  
  // force resync
  return false;
}

/*
 * NAME demux_ts_pmt_parse
 *
 * Parse a PMT. The PMT is expected to be exactly one section long,
 * and that section is expected to be contained in a single TS packet.
 *
 * In other words, the PMT is assumed to describe a reasonable number of
 * video, audio and other streams (with descriptors).
 */

int TSSystemStream::demux_ts_pmt_parse(MpegSystemHeader* mpegHeader) {
  int sectionLength=processSection(mpegHeader);
  if (sectionLength == 0) return false;

  //?
  if (nukeBytes(2) == false) return false;
  sectionLength-=2;

  /*
   * ES definitions start here...we are going to learn upto one video
   * PID and one audio PID.
   */


  unsigned char pkt[2];
  if (read((char*)pkt,2) == false) return false;
  sectionLength-=2;
  
  unsigned int programInfoLength;

  programInfoLength=(((unsigned int)pkt[0] & 0x0f) << 8) | pkt[1];
  if (paket_read+programInfoLength > paket_len) {
    printf ("demux error! PMT with inconsistent progInfo length\n");
    return false;
  }

  if (nukeBytes(programInfoLength) == false) return false;
  sectionLength-=programInfoLength;

  return processElementary(sectionLength,mpegHeader);
 

}

/**
   return false on error or section length info on success
*/
int TSSystemStream::processSection(MpegSystemHeader* mpegHeader) {
  unsigned int pus=mpegHeader->getPayload_unit_start_indicator();

  /*
   * A PAT in a single section should start with a payload unit start
   * indicator set.
   */
  if (pus==0) {
    printf ("demux error! PAT without payload unit start\n");
    return false;
  }
  /*
   * PAT packets with a pus start with a pointer. Skip it!
   */
  if (skipNextByteInLength() == false) return false;

  // ??
  if (nukeBytes(1) == false) return false;

  // read sectionLength
  unsigned char pkt[2];
  if (read((char*)pkt,2) ==false) return false;


  int sectionLength=(((unsigned int)pkt[0] & 0x3) << 8) | pkt[1];
  if (paket_read+sectionLength > PKT_SIZE) {
    printf ("demux error! invalid section size %d\n",sectionLength);
    return false;
  }

  // ??
  if (nukeBytes(2) == false) return false;

  int byte=getByteDirect();
  if (byte < 0) return false;
  
  if ((byte & 0x01) == false) {
    /*
     * Not current!
     */
    return false;
  }
  if (read((char*)pkt,2) == false) return false;
  
  if ((pkt[0]) || (pkt[1])) {
    printf ("demux error! PAT with invalid section %02x of %02x\n",
	    pkt[0], pkt[1]);
    return false;
  }
  
  /*
   * TBD: at this point, we should check the CRC. Its not that expensive, and
   * the consequences of getting it wrong are dire!
   */ 
  return sectionLength-5;
}

/*
 * NAME demux_ts_pat_parse
 *
 *  Parse a PAT. The PAT is expected to be exactly one section long,
 *  and that section is expected to be contained in a single TS packet.
 *
 *  The PAT is assumed to contain a single program definition, though
 *  we can cope with the stupidity of SPTSs which contain NITs.
 */

int TSSystemStream::demux_ts_pat_parse(MpegSystemHeader* mpegHeader) {
  int sectionLength=processSection(mpegHeader);
  if (sectionLength == 0) return false;
  
  return processPrograms(sectionLength,mpegHeader);

}


/*
 * Process all programs in the program loop.
 */
int TSSystemStream::processPrograms(int sectionLength,
				    MpegSystemHeader* mpegHeader) {
  int programs=sectionLength / 4;
  int i;
  // what happens with the last 4 byte?
  // seems they have no meaning?
  programs--;

  for(i=0;i<programs;i++) {
    unsigned char program[4];
    if (read((char*)program,4) == false) return false;
    

    unsigned int programNumber;
    unsigned int pmtPid;
    unsigned int programCount;
    programNumber = ((unsigned int)program[0] << 8) | program[1];

    /*
     * Skip NITs completely.
     */
    if (!programNumber)
      continue;

    pmtPid = (((unsigned int)program[2] & 0x1f) << 8) | program[3];

    
    /*
     * If we have yet to learn our program number, then learn it.
     */

    if (mpegHeader->getProgramNumber() == INVALID_PROGRAM) {
      mpegHeader->setProgramNumber(programNumber);
      mpegHeader->setPMTPID(pmtPid);
    }
    
    if (mpegHeader->getProgramNumber() != programNumber) {
      printf("demux error! MPTS: programNumber=%u pmtPid=%04x\n",
	     programNumber, pmtPid);
    }

    if (mpegHeader->getPMTPID() != pmtPid) {
      printf("pmtPid changed %04x\n", pmtPid);
      mpegHeader->setPMTPID(pmtPid);
    }
    
  }
  // nuke last four bytes
  if (nukeBytes(4) == false) return false;

  //
  // now we can nuke the rest of the PAKET_SIZE (188 byte)
  //
  mpegHeader->setTSPacketLen(paket_len-paket_read);
  return true;
}



int TSSystemStream::processElementary(int sectionLength,
				     MpegSystemHeader* mpegHeader) {

  
  /*
   * Extract the elementary streams.
   */
  int mediaIndex=0;
  // what happens with the last 4 byte?
  // seems they have no meaning?
  while (sectionLength > 4) {
    unsigned int streamInfoLength;
    unsigned char stream[5];
    if (read((char*)stream,5) == false) return false;
    sectionLength-=5;

    unsigned int pid;
    pid = (((unsigned int)stream[1] & 0x1f) << 8) | stream[2];
 
    streamInfoLength = (((unsigned int)stream[3] & 0xf) << 8) | stream[4];
    if(paket_read+streamInfoLength > paket_len) {
      printf ("demux error! PMT with inconsistent streamInfo length\n");
      return false;
    }
    mpegHeader->insert(pid,stream[0],mpegHeader);
    
  }
    
  // nuke last four bytes
  if (nukeBytes(4) == false) return false;
  //
  // now we can nuke the rest of the PAKET_SIZE (188 byte)
  //
  mpegHeader->setTSPacketLen(paket_len-paket_read);

  //
  // now we can be sure that we have in fact an TS stream
  // so, switch to MPEG2 PES now
  mpegHeader->setMPEG2(true);

  return true;
}


