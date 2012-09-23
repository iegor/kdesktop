/*
  splay player plugin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "splayPlugin.h"
#include "../splay/mpegsound.h"

#include "../splay/splayDecoder.h"
#include "../frame/floatFrame.h"
#include "../frame/pcmFrame.h"
#include "../splay/mpegAudioFrame.h"
#include "../splay/mpegAudioInfo.h"
#include "../input/fileAccessWrapper.h"

#include <iostream>

using namespace std;

#define INPUTBUFFER_SIZE 8192

SplayPlugin::SplayPlugin() {
  pow(6.0,3.0);            // fixes bug in __math.h
  doFloat = false;
  lnoLength=false;
  inputbuffer=new unsigned char[INPUTBUFFER_SIZE];
  pcmFrame=new PCMFrame(MP3FRAMESIZE);
  floatFrame=new FloatFrame(MP3FRAMESIZE);
  audioFrame=new AudioFrame();
  framer=new MpegAudioFrame();
  splay=new SplayDecoder();
  lenghtInSec=0;
 
  fileAccess=NULL;
  info=NULL; 
  lOutput=true;
}


SplayPlugin::~SplayPlugin() {
  delete [] inputbuffer;
  delete pcmFrame;
  delete floatFrame;
  delete framer;
  delete splay;
  delete audioFrame;
}


// here we can config our decoder with special flags
void SplayPlugin::config(const char* key,const char* value,void* user_data) {
  if (strcmp(key,"dofloat")==0) {
    doFloat=true;
  }
  if (strcmp(key,"-m")==0) {
    splay->config("m","0",NULL);
  }
  if (strcmp(key,"-2")==0) {
    splay->config("2","1",NULL);
  }
  if (strcmp(key,"-c")==0) {
    lnoLength=true;
  }
  if (strcmp(key,"-d")==0) {
    lOutput=false;
  }

  if (strcmp(key,"decode")==0) {
    if (strcmp(value,"true")==0) {
      lDecode=true;
    } else {
      lDecode=false;
    }
  }    
  DecoderPlugin::config(key,value,user_data);
}



void SplayPlugin::decoder_loop() {

  if (input == NULL) {
    cout << "SplayPlugin::decoder_loop input is NULL"<<endl;
    exit(0);
  }
  if (output == NULL) {
    cout << "SplayPlugin::decoder_loop output is NULL"<<endl;
    exit(0);
  }
  // init decoder
  output->audioInit();

  // start decoding

  fileAccess=new FileAccessWrapper(input);
  info= new MpegAudioInfo(fileAccess);


  
  framer->reset();
  lenghtInSec=0;
  resyncCounter=0;

  AudioFrame* audioFrame=pcmFrame;
  if (doFloat) {
    audioFrame=floatFrame;
  }

  output->audioInit();
  while(runCheck()) {

    //
    // check for re-init or for "eof"
    //
    switch(streamState) {
    case _STREAM_STATE_INIT :
      framer->reset();
      resyncCounter=5;
      setStreamState(_STREAM_STATE_PLAY);
      continue;
    case _STREAM_STATE_WAIT_FOR_END:
      // exit while loop
      lDecoderLoop=false;
      continue;
    }
    if (doFrameFind() == true) {
      if (splay->decode(framer->outdata(),framer->len(),audioFrame) == false){
	continue;
      }
      // send data out:

      int rest=framer->restBytes();
      // we have inserted more data already than the 
      // framer has processed. But framer tells us
      // how much he still needs to process.
      long pos=input->getBytePosition();
      TimeStamp* stamp=input->getTimeStamp(pos-rest);
      processStreamState(stamp,audioFrame);

      
      // make this stamp invalid for further synchronisation
      stamp->setPTSFlag(false);
    }
  }

  output->audioFlush();

  delete fileAccess;
  delete info;
  fileAccess=NULL;
  info=NULL;


}


int SplayPlugin::doFrameFind() {

  //
  // fine, we can work on the stream:
  //
  int back=false;
  int framerState=framer->getState();
  switch(framerState) {
  case FRAME_NEED: {
    int bytes=framer->canStore();
    int read=input->read((char*)inputbuffer,bytes);

    if (read <= 0) {
      // read error. reset framer, drop frames
      setStreamState(_STREAM_STATE_INIT);
      break;
    }
    framer->store(inputbuffer,read);
    break;
  }
  case FRAME_WORK:
    back=framer->work();
    break;
  case FRAME_HAS: {
    break;
  }
  default:
    cout << "unknown state in mpeg audio framing"<<endl;
    exit(0);
  }
  return back;
}

void SplayPlugin::audioSetup(AudioFrame* setupFrame) {
  setupFrame->copyFormat(audioFrame);
  output->audioSetup(audioFrame->getFrequenceHZ(),
                     audioFrame->getStereo(),
                     audioFrame->getSigned(),
                     audioFrame->getBigEndian(),
                     audioFrame->getSampleSize()); 
}



void SplayPlugin::processStreamState(TimeStamp* stamp,AudioFrame* playFrame){

  // we always have a frame here, with the correct timestamp here.

  switch(streamState) {
  case _STREAM_STATE_FIRST_INIT :
    output->audioOpen();
   
    audioSetup(playFrame);

    if (lnoLength==false) {
      lenghtInSec=getTotalLength();
      pluginInfo->setLength(lenghtInSec);
      output->writeInfo(pluginInfo);
    }

    setStreamState(_STREAM_STATE_PLAY);
    // yes, here is no break.
    // we want to send the frame to the output.

  case _STREAM_STATE_PLAY :
    if (resyncCounter > 0) {
      resyncCounter--;
      break;
    }
    if (audioFrame->isFormatEqual(playFrame)==false) {
      audioSetup(playFrame);
    }
    if (lOutput == false) {
      break;
    }
    if(doFloat) {
      FloatFrame* floatFrame=(FloatFrame*)playFrame;
      output->audioPlay(stamp,stamp,
			(char*) floatFrame->getData(),
			playFrame->getLen()*sizeof(float) );
    } else {
      PCMFrame* pcmFrame=(PCMFrame*)playFrame;
      output->audioPlay(stamp,stamp,
			(char*)pcmFrame->getData(),
			playFrame->getLen()*sizeof(short int));
    }
    break;
  default:
    cout << "unknown stream state:"<<streamState<<endl;
  }
}



// splay can seek in streams
int SplayPlugin::seek_impl(int second) {


  if (info != NULL) {
    int pos=info->getSeekPosition(second);
    input->seek(pos);
    setStreamState(_STREAM_STATE_INIT);
  } else {
    cout << "cannot seek, plugin not initialized"<<endl;
  }
  return true;
}


int SplayPlugin::getTotalLength() {
  shutdownLock();
  int back=0;

  if (info->getNeedInit()) {
    long pos=input->getBytePosition();
    if (input->seek(0) == true) {
      int bytes=1024;
      // try reading all info, but not more
      // than 1024 iterations.
      info->reset();
      while(bytes>0) {
	if (info->initialize()==true) {
	  break;
	}
	bytes--;
      }
      input->seek(pos);
    }
    // wheter successful or not, never touch
    // the info thing again.
    info->setNeedInit(false);
  }

  back=info->getLength();
  

  shutdownUnlock();
  return back;
}





