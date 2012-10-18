/*
  base class for splay mp3 decoder
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#include "splayPlayObject_impl.h"
#include "../mpeglib/lib/splay/splayDecoder.h"
#include "../mpeglib/lib/frame/audioFrameQueue.h"
#include "../mpeglib/lib/splay/mpegAudioFrame.h"

#include "debug.h"

#define INPUT_SIZE  8192

/**
   Problems with streaming, which must be solved:
   ==============================================


   Deadlocks. I think we cannot have sync streams
   for input/output.

   This example is an OLD style PlayObject. (it uses sync streams)
   The task is to convert this into an async PlayObject.

   What does this PlayObject do:
   -----------------------------

   Current:
   It reads data from a file and insert it
   into a queue.

   Future:
   A "remote" sender of raw data, can test how much
   bytes he can insert into the "framer".
   If the framer produces a frame, we decode it,
   and send an async dataPacket out.
   (Then the frameQueue is not necessary anymore -IMHO)

*/


SplayPlayObject_impl::SplayPlayObject_impl() {

  flpos=0.0;
  
  splay=new SplayDecoder();
  frameQueue= new AudioFrameQueue(10,MP3FRAMESIZE,_FRAME_AUDIO_FLOAT);
  framer=new MpegAudioFrame();

  arts_debug("outputStream created");

  _state=Arts::posIdle;
  file=NULL;
  lStreaming=false;
  
  resampleBuffer=NULL;
  resampleBufferSize=0;
  currentPacket=NULL;
  currentPos=0;
  // ok, we store packets here. Someday I should make this a template.
  // use STL? maybe.
  packetQueue=new FrameQueue(10);
    
  // Note: The inputbuffer must NOT NOT NOT be on the stack!
  inputbuffer=new unsigned char[INPUT_SIZE];

}

SplayPlayObject_impl::~SplayPlayObject_impl() {
  arts_debug("~SplayPlayObject_impl -s");
  delete splay;
  delete frameQueue;
  delete framer; 
  arts_debug("~SplayPlayObject_impl -e");

  if (resampleBuffer != NULL) {
    delete resampleBuffer;
  }
  //
  // arts wants to free the packets and packetQueue too.
  // so we dequeue all packets here, before deleting packetQueue
  // otherwise we call delete on the packets twice.
  //
  while (packetQueue->getFillgrade() > 0) packetQueue->dequeue();
  // now delete the (empty) queue

  delete packetQueue;
  delete [] inputbuffer;
}


bool SplayPlayObject_impl::loadMedia(const string &filename) {
  arts_debug("loadMedia");

  if (file != NULL) {
    arts_fatal("~SplayPlayObject_impl already loaded");
  }

  lStreaming=false;

  file=fopen(filename.c_str(),"r");
  if (file == NULL) {
    arts_debug("splay cannot open file");
    return false;
  }
  
  flpos=0.0;

  return true;
}

bool SplayPlayObject_impl::streamMedia(Arts::InputStream instream) {
  arts_debug("streamMedia");
  lStreaming=true;
  currentStream = instream;
  Arts::StreamPlayObject self = Arts::StreamPlayObject::_from_base(_copy());
  connect(currentStream, "outdata", self);
  return true;
}

void SplayPlayObject_impl::process_indata(Arts::DataPacket<Arts::mcopbyte> *inpacket) {
  arts_debug("receiving packet");


  packetQueue->enqueue((Frame*)inpacket);
  if (packetQueue->getFillgrade() == 1) {
    currentPos=0;
  }

  processQueue();
}


void SplayPlayObject_impl::processQueue() {

  

  unsigned char* ptr;

  if (packetQueue->getFillgrade() == 0) {
    return;
  }
  Arts::DataPacket<Arts::mcopbyte>* currentPacket;

  currentPacket=(Arts::DataPacket<Arts::mcopbyte>*) packetQueue->peekqueue(0);
  int rest=currentPacket->size-currentPos;

  int cnt=0;
  while( (rest > 0)  && (frameQueue->emptyQueueCanRead()) ) {

    int state=framer->getState();
    switch(state) {
    case FRAME_NEED: {
      int bytes=framer->canStore();
      ptr=currentPacket->contents+currentPos;
      // don't read beyond the packet.
      if (bytes >= rest) {
	bytes=rest;
	// we must copy this, because we can commit a packet,
	// which then deletes the pointer to the packet.
	// but the framer still wants to access the data, later!
	if (bytes > INPUT_SIZE) {
	  cout << "inputbuffer too small"<<endl;
	  exit(0);
	}
 	memcpy(inputbuffer,ptr,bytes);
	ptr=inputbuffer;
      }
      framer->store(ptr,bytes);
      currentPos+=bytes;
      rest-=bytes;
      break;
    }
    case FRAME_WORK:
      framer->work();
      break;
    case FRAME_HAS:{
      AudioFrame* emptyFrame= frameQueue->emptyQueueDequeue();
      if (splay->decode(framer->outdata(),framer->len(),emptyFrame)==true) {
         frameQueue->dataQueueEnqueue(emptyFrame);
         cnt++;
      }
      break;
    }
    default:
      cout << "unknown state in mpeg audio framing"<<endl;
      exit(0);
    }
  }
  //cout << "rest-last:"<<rest<<" cnt:"<<cnt<<endl;

  if (rest == 0) {
    arts_debug("packet processed");
    currentPacket->processed();
    packetQueue->dequeue();
    currentPos=0;
  }

}

string SplayPlayObject_impl::description() {
  arts_debug("description [GET1]");
  string back;
  return back;
}

void SplayPlayObject_impl::description(const string &) {
  arts_debug("description [GET2]");
}

Arts::poTime SplayPlayObject_impl::currentTime() {
  Arts::poTime time;
  return time;
}



Arts::poTime SplayPlayObject_impl::overallTime() {
  Arts::poTime time;
  return time;
}

Arts::poCapabilities SplayPlayObject_impl::capabilities() {
  arts_debug("capabilities");
  return (Arts::poCapabilities)(Arts::capPause);
}

string SplayPlayObject_impl::mediaName() {
  arts_debug("mediaName");
  string back;
  return back;
}

Arts::poState SplayPlayObject_impl::state() {
  return _state;
}

void SplayPlayObject_impl::play() {
  arts_debug("play:");
  if (file == NULL) {
    arts_debug("file is NULL:");
    if (lStreaming && _state != Arts::posPlaying) {
      currentStream.streamStart();
      _state = Arts::posPlaying;
    }
    
  } else {
    _state = Arts::posPlaying;
  }
}


void SplayPlayObject_impl::seek(const class Arts::poTime& ) {
  arts_debug("SEEK - RESET in decoder.");
  framer->reset();
  // and remove pre-decoded frame:
  frameQueue->clear();
    
  return;
}


void SplayPlayObject_impl::pause() {
  arts_debug("pause");
  _state=Arts::posPaused;
}
/*
 *
 * halt() (which the normal programmer would probably refer to as stop())
 * should seek to the beginning and go into the posIdle state, like a just
 * opened PlayObject
 *
 */

void SplayPlayObject_impl::halt() {
  arts_debug("halt");
  _state=Arts::posIdle;
}
 

void SplayPlayObject_impl::streamInit() {
  arts_debug("streamInit");
  return;
}


void SplayPlayObject_impl::streamStart() {
  arts_debug("streamStart");
  return;
}



void SplayPlayObject_impl::calculateBlock(unsigned long wantSamples) {
  unsigned long i;
  unsigned long haveSamples=frameQueue->getLen();

  // the wantSamples*2 takes care that there is enough data
  // even if we have stereo.

  
  if (haveSamples < wantSamples*2) {
    if (lStreaming) {
      for(i=0;i<wantSamples;i++) {
	left[i] = right[i] = 0.0;
      }
      return;
    }
    // else (file access)
    getMoreSamples(wantSamples*2);
  }

  // Yep, we have enough data



  // now check if we need resampling.
  AudioFrame* audioFrame=frameQueue->getCurrent();
  double wav_samplingRate=(double)audioFrame->getFrequenceHZ();

  /* difference between the sampling rates in percent */
  float diff = fabs(wav_samplingRate-samplingRateFloat) / samplingRateFloat;
  //cout << "wav_samplingRate:"<<wav_samplingRate<<endl;
  /*
   * efficient optimized case:
   * 1. decoder -> float rendering
   * 2. no resampling (i.e. artsd running @ 44100 Hz, playing an 44100 Hz mp3)
   */

  if(diff < 0.02) {
    // no resample (easy+fast case)
    haveSamples=frameQueue->copy(left,right,wantSamples);

    for(i=haveSamples;i<wantSamples;i++) {
      left[i] = right[i] = 0.0;
    }
    frameQueue->forwardStreamDouble(haveSamples);
    
    if (lStreaming) {
      processQueue();
    }
  } else {
    /** FIXME: you can use this to implement pitchable playobject **/
    double _speed = 1.0; 

    // calculate "how fast" we consume input samples (speed = 2.0 means,
    // we need 2 input samples to generate one output sample)
    double speed = (double)wav_samplingRate/(double)(samplingRateFloat/_speed);

    // calculate how many samples we need to read from the frameQueue to
    // satisfy the request
    long readSamples = long((double)wantSamples*speed+8.0);
    checkResampleBuffer(readSamples*2);

    // read the samples
    //  - readSamplesOk contains how much we really got
    //  - we put the samples non-interleaved into the resampleBuffer,
    //    first the left channel, then the right channel
    long readSamplesOk = frameQueue->copy(&resampleBuffer[0],
					  &resampleBuffer[readSamples],
					  readSamples);
    // check how many output samples we will be able to generate from that
    long samplesToConvert = long((double)readSamplesOk/speed)-4;
    if(samplesToConvert < 0) samplesToConvert = 0;
    if(samplesToConvert > wantSamples) samplesToConvert = wantSamples;

    // do the conversion
    Arts::interpolate_mono_float_float(samplesToConvert,flpos,speed,
				 &resampleBuffer[0],left);
    Arts::interpolate_mono_float_float(samplesToConvert,flpos,speed,
				 &resampleBuffer[readSamples],right);

    // calculate where we are now (as floating point position) in our
    // inputsample buffer
    flpos += (double)samplesToConvert * speed;

    // Good - so how many input samples we won't need anymore (for the
    // next request)? Skip them.
    int skip = (int)floor(flpos);
    if(skip)
    {
      frameQueue->forwardStreamDouble(skip);
      flpos = flpos - floor(flpos);
    }

    for(i=samplesToConvert; i<wantSamples; i++) {
      left[i] = right[i] = 0.0;
    }

    if (lStreaming) {
      processQueue();
    }
  }
 
  // ok, eof stop.
  /*
  if (haveSamples  < wantSamples) {
    halt();
  }
  */
}

void SplayPlayObject_impl::streamEnd() {
  arts_debug("streamEnd");
  if (file != NULL) {
    fclose(file);
    file=NULL;
  }
  return;
}


void SplayPlayObject_impl::checkResampleBuffer(int size) {
 

  if (resampleBufferSize != size) {
    if (resampleBuffer != NULL) {
      delete resampleBuffer;
    }
    resampleBuffer = new float[size];
    resampleBufferSize=size;
  }
 
}



//
// This method shows how we insert data in the framer,
// decode a frame and store it in the queue.
// This damned queue is necessary, becasue a sync out
// stream needs a guranteed streamsize == samples.
// an async stream would simply send a packet
// and we are done.

void SplayPlayObject_impl::getMoreSamples(int needLen) {


  while( (feof(file) == false) && (frameQueue->getLen() < needLen) ) {

    //
    // This switch/case statement "partitions" the fileinput
    // into mp3frames. We directly decode them to pcmFrames.
    //

    //
    // There are three states. One is "I want Input, give me input"
    // The other is "I have enough input, dont' bother me and let
    // me do my work"
    // The last ist "I have a frame here, take care of this, or
    // I will continue with my work"
 
    int state=framer->getState();
    int cnt=0;
    switch(state) {
    case FRAME_NEED: {
      int bytes=framer->canStore();
      int read=fread(inputbuffer,1,bytes,file);
      if (read != bytes) {
	// read error. reset framer
	framer->reset();
	continue;
      }
      // Note: The inputbuffer must NOT NOT NOT be on the stack!
      framer->store(inputbuffer,bytes);
      break;
    }
    case FRAME_WORK:
      framer->work();
      break;
    case FRAME_HAS:{
      AudioFrame* emptyFrame= frameQueue->emptyQueueDequeue();
      if (splay->decode(framer->outdata(),framer->len(),emptyFrame)==true) {
         frameQueue->dataQueueEnqueue(emptyFrame);
         cnt++;
      }
      break;
    }
    default:
      cout << "unknown state in mpeg audio framing"<<endl;
      exit(0);
    }
  }
  if (feof(file)==true) {
    halt();
    return;
  }
}

REGISTER_IMPLEMENTATION(SplayPlayObject_impl);

// vim:ts=8:sw=2:sts=2
