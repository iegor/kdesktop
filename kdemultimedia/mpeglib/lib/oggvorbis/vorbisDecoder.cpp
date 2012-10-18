/*
  converts ogg frames into audioFrames
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "vorbisDecoder.h"

#ifdef OGG_VORBIS

#define _VORBIS_NEED_SYNTHHEADER_1     1
#define _VORBIS_NEED_SYNTHHEADER_2     2
#define _VORBIS_NEED_SYNTHHEADER_3     3

#define _VORBIS_DECODE_SETUP           4
#define _VORBIS_DECODE_LOOP            5

#include <iostream>

using namespace std;


VorbisDecoder::VorbisDecoder() {
  vorbis_info_init(&vi);
  vorbis_comment_init(&vc);
  reset();
}


VorbisDecoder::~VorbisDecoder() {
}

int VorbisDecoder::hasHeader() {
  return (initState>=_VORBIS_DECODE_LOOP);
}

int VorbisDecoder::decode(RawFrame* rawFrame,AudioFrame* dest) {
  
  if ((rawFrame == NULL) || (dest == NULL)) {
    cout << "VorbisDecoder::decode NULL pointer!"<<endl;
    exit(-1);
  }
  if (rawFrame->getFrameType() != _FRAME_RAW_OGG) {
    cout << "VorbisDecoder::decode not _FRAME_RAW_OGG"<<endl;
    exit(-1);
  }

  ogg_packet* op=(ogg_packet*) rawFrame->getData();
  switch(initState) {
  case _VORBIS_NEED_SYNTHHEADER_1:
  case _VORBIS_NEED_SYNTHHEADER_2:
  case _VORBIS_NEED_SYNTHHEADER_3:
    cout << "_VORBIS_NEED_SYNTHHEADER:"<<initState<<endl;
    if(vorbis_synthesis_headerin(&vi,&vc,op)<0){ 
      /* error case; not a vorbis header */
      fprintf(stderr,"This Ogg bitstream does not contain Vorbis "
              "audio data.\n");
      exit(1);
    }
    initState++;
    break;
  case _VORBIS_DECODE_SETUP:
    cout << "_VORBIS_DECODE_SETUP"<<endl;
    vorbis_synthesis_init(&vd,&vi); /* central decode state */
    vorbis_block_init(&vd,&vb);     /* local state for most of the decode
                                       so multiple block decodes can
                                       proceed in parallel.  We could init
                                       multiple vorbis_block structures   
                                       for vd here */
    initState=_VORBIS_DECODE_LOOP;
    // yes right, we must decode the packet!
    // so there is no break here.
  case _VORBIS_DECODE_LOOP: {
    if(vorbis_synthesis(&vb,op)==0) {/* test for success! */
      vorbis_synthesis_blockin(&vd,&vb);
    } else {
      cout << "vorbis_synthesis error"<<endl;
      exit(0);
    }
    float **pcm;
    /*

    **pcm is a multichannel float vector.  In stereo, for
    example, pcm[0] is left, and pcm[1] is right.  samples is
    the size of each channel.  Convert the float values
    (-1.<=range<=1.) to whatever PCM format and write it out 

    */
    int samples=vorbis_synthesis_pcmout(&vd,&pcm);
    if (samples > 0) {
      int maxSamples=dest->getSize();
      if (samples > maxSamples) {
	cout << "more samples in vorbis than we can store"<<endl;
	exit(0);
      }
      dest->clearrawdata();
      dest->setFrameFormat(vi.channels-1,vi.rate);
      
      if (vi.channels == 2) {
	dest->putFloatData(pcm[0],pcm[1],samples);
      } else {
	dest->putFloatData(pcm[0],NULL,samples);
      }

      vorbis_synthesis_read(&vd,samples); /* tell libvorbis how
					     many samples we
					     actually consumed */
      return true;
    }
    
    return false;
  }
  default:
    cout << "unknown state in vorbis decoder"<<endl;
    exit(0);
  }
  return false;
}

void VorbisDecoder::reset() {
  initState=_VORBIS_NEED_SYNTHHEADER_1;
}

void VorbisDecoder::config(const char* ,const char* ,void* ) {
  
}

#endif
