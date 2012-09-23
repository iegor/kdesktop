/*
  base class for converting raw data(stream) into some frametype.
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __FRAMER_H
#define __FRAMER_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kdemacros.h>

#define FRAME_NEED     0
#define FRAME_WORK     1
#define FRAME_HAS      2


#include "rawDataBuffer.h"


/**
   If we want to have a "push" interface, this means the decoder
   does not read from an input interface, we must be sure
   that we have a valid mpeg audio frame, before we feed
   this whole frame to the decoder.
   
   This class tells you how many bytes it can read and
   gives you information wether we have a valid frame or not.

   This class has three public states:

                 FRAME_NEED
              /             \
             /               \
            /                 \
   FRAME_WORK  <---------->  FRAME_HAS


   after the "FRAME_HAS" state we devliver _excatly_ one time
   this state, and then automatically go to "FRAME_WORK" state.


   You can reset() the class which empties the buffer and then
   starts searching for a new sync code, or you can
   do a next() which searches for the sync code, without
   emptying the buffer (this is done automatically,
   after the first call, when we was ins state "FRAME_HAS"

   Note: i)  You need a reset() if a "seek" occurs in your stream
         ii) First call to the class must be "getState" which does the
             Post-Constructor setup in derived classes!

   The FRAME_NEED state is entered if the input buffer is empty
   you then need to "push" data in this class.
   The FRAME_NEED state is necessary to avoid an additonal memcpy
   and a ringbuffer.(This safes "overhead"/"cpu cycles")
*/




class KDE_EXPORT Framer {

  // this is our destination buffer for the output frame
  // this buffer must be able to store the maximum size
  // a frame of this type can have.
  // Examples:
  // avi-Chunk : 65KB
  // mpeg audio: 4KB is always enough
  // mpeg video: 224K (spec says this)

  // this can be a remote buffer or a local one
  unsigned char* buffer_data;
  RawDataBuffer* buffer_info;

  // state from FIND->READ->HAS
  int process_state;
  // state between NEED <-> PROCESS
  int main_state;

  RawDataBuffer* input_info;
  int lAutoNext;

  // stores if we have alloceated outdata or not
  int lDeleteOutPtr;
  // internal: unsync not done
  int lConstruct;

 public:
  // allocate local output buffer
  Framer(int outsize);

  // outbuffer is remote.
  Framer(int outsize,unsigned char* outptr);

  virtual ~Framer();

  //
  // process states (transitions) [START]
  //

  // stores pointer to input and len
  void store(unsigned char* start,int bytes);
  int  work();
  void reset();
  void next();
  // returns pointer to outbuffer (frameheader+data)
  unsigned char* outdata();
  
  // returns pointer to inbuffer  (raw data)
  // Note: this ptr is not fixed! It may vary from time to time
  //       Cannot be stores in a variable!
  unsigned char* indata();
  
 
  //
  // process states (transitions) [END]
  //

  
  //
  // state helper functions [START]
  //
  int getState();
  // returns number of bytes.
  int canStore();

  // returns length of frame == len(frameheader+data) 
  int len();

  // returns number of bytes still in input(needed for a/v sync)
  int restBytes();
  //
  // state helper functions [END]
  //

  // debugging
  void printMainStates(const char* msg);

 private:
  void init(int outsize,unsigned char* outptr,int lDeleteOutptr);

  void setState(int state);
  
  //
  // Overload functions for specialized framers [START]
  //

  // return true, if frame header found
  virtual int find_frame(RawDataBuffer* input,RawDataBuffer* store);
  // return true, if frame data read.
  virtual int read_frame(RawDataBuffer* input,RawDataBuffer* store);
  // makes buffer invalid, reset to begin of "find_frame"
  virtual void unsync(RawDataBuffer* store,int lReset);
  // debugging
  virtual void printPrivateStates();
 
  //
  // Overload functions for specialized framers [END]
  //

 protected:
  // this can be used, if the outptr come from a different framer
  // (eg: OGG framer). You then need to call the construtor with
  // some "dummy" size.
  void setRemoteFrameBuffer(unsigned char* outptr,int size);
 
};


#endif
