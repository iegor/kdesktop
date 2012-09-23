/*
  mpg I video/audio player plugin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "mpgPlugin.h"

#include "../mpegplay/mpegSystemStream.h"
#include "../mpgplayer/mpegStreamPlayer.h"
#include "../mpegplay/mpegVideoLength.h"

#include "splayPlugin.h"
#include "mpegPlugin.h"

#include <iostream>

using namespace std;


MpgPlugin::MpgPlugin() {

  mpegSystemHeader=NULL;
  mpegSystemStream=NULL;
  mpegStreamPlayer=NULL;
  mpegVideoLength=NULL;

  // make runtime config easier, no NULL check necessary
  mpegSystemHeader=new MpegSystemHeader();
  timeStamp=new TimeStamp();
  lMono=false;
  lDown=false;
  lWriteStreams=false;
  lDoLength=true;
}


MpgPlugin::~MpgPlugin() {
  delete mpegSystemHeader;
  delete timeStamp;
}


 
void MpgPlugin::decoder_loop() {

  if (input == NULL) {
    cout << "MPGPlugin::decoder_loop input is NULL"<<endl;
    exit(0);
  }
  if (output == NULL) {
    cout << "MPGPlugin::decoder_loop output is NULL"<<endl;
    exit(0);
  }

  // decoder
  SplayPlugin* splayPlugin=new SplayPlugin();
  splayPlugin->config("-c","true",NULL);
  if (lMono) {
    splayPlugin->config("-m","true",NULL);
  }
  if (lDown) {
    splayPlugin->config("-2","true",NULL);
  }
   
  MpegPlugin* mpegplayPlugin=new MpegPlugin();

  mpegSystemStream=new MpegSystemStream(input);
  mpegStreamPlayer=new MpegStreamPlayer(input,output,
					splayPlugin,mpegplayPlugin);
  mpegStreamPlayer->setWriteToDisk(lWriteStreams);
  mpegVideoLength=new MpegVideoLength(input);

 
  int lSysLayer=false;
  int lHasLength=false;

  while(runCheck()) {
    switch(streamState) {
    case _STREAM_STATE_FIRST_INIT :
      if ((lHasLength==false) && (lDoLength)) {
	if (mpegVideoLength->firstInitialize()==false) {
	  continue;
	}
	lHasLength=true;
      }
      if (mpegSystemStream->firstInitialize(mpegSystemHeader)==false) {
	//cout << "still initializing system stream"<<endl;
      } else {
	// if we have found a raw stream,
	// make sure we pass as first argument a SEQ_START_CODE
	if (mpegSystemHeader->getLayer() == _PACKET_SYSLAYER) {
	  lSysLayer=true;
	  mpegStreamPlayer->processSystemHeader(mpegSystemHeader);
	} else {
	  //output->config("-s","false");
	  unsigned int startCodeRaw=htonl(_SEQ_START_CODE);
	  mpegStreamPlayer->
	    insertVideoDataRaw((unsigned char*)&startCodeRaw,4,timeStamp);
	}
	pluginInfo->setLength(mpegVideoLength->getLength());
	output->writeInfo(pluginInfo);
	setStreamState(_STREAM_STATE_INIT);
	
	if (mpegSystemHeader->getMPEG2()==true) {
	  cout << "this plugin does not support MPEG2/VOB/DVD"<<endl;
	  lDecoderLoop=false;
	  if (lWriteStreams == true) {
	    cout << "demux is supported"<<endl;
	    lDecoderLoop=true;
	    
	  }
	  break;
	}

      }
      break;
    case _STREAM_STATE_INIT :
      setStreamState(_STREAM_STATE_PLAY);
      break;
    case _STREAM_STATE_PLAY :
      // syslayer

      if (mpegSystemStream->nextPacket(mpegSystemHeader) == false) {

	break;
      }

      if (mpegStreamPlayer->processSystemHeader(mpegSystemHeader) == false) {
        mpegSystemStream->reset();
	setStreamState(_STREAM_STATE_INIT);
      }
      
      break;
    case _STREAM_STATE_WAIT_FOR_END:
      if (mpegStreamPlayer->hasEnd()==true) {
	// exit while loop
	lDecoderLoop=false;
      }
      TimeWrapper::usleep(100000);
      break;
    default:
      cout << "unknown stream state:"<<streamState<<endl;
    }
  }

  delete mpegStreamPlayer;
  delete mpegSystemStream;
  delete mpegVideoLength;
  delete mpegplayPlugin;
  delete splayPlugin;

  mpegSystemStream=NULL;
  mpegStreamPlayer=NULL;
  mpegVideoLength=NULL;


  output->audioFlush();
  output->flushWindow();

}



// here we can config our decoder with special flags
void MpgPlugin::config(const char* key,const char* value,void* user_data) {
  if(strcmp("VideoLayer", key) == 0) {
    int videoLayerSelect=atoi(value);
    mpegSystemHeader->setVideoLayerSelect(videoLayerSelect);
  }

  if(strcmp("AudioLayer", key) == 0) {
    int audioLayerSelect=atoi(value);
    mpegSystemHeader->setAudioLayerSelect(audioLayerSelect);
  }
  if (strcmp(key,"-2")==0) {
    lDown=true;
  }
  if (strcmp(key,"-m")==0) {
    lMono=true;
  }
  if (strcmp(key,"-c")==0) {
    lDoLength=false;
  }
  if (strcmp(key,"-w")==0) {
    if (strcmp(value,"true")==0) {
      lWriteStreams=true;
    } else {
      lWriteStreams=true;
    }
  }
  // now try to set it if stream is running:
  shutdownLock();
  if (mpegStreamPlayer != NULL) {
    mpegStreamPlayer->setWriteToDisk(lWriteStreams);
  }
  shutdownUnlock();  
    
  
  DecoderPlugin::config(key,value,user_data);
}




int MpgPlugin::getTotalLength() {
  shutdownLock();
  int back=0;
  if (mpegVideoLength != NULL) {
    back=mpegVideoLength->getLength();
  } else {
    cout << "cannot report total length, plugin not initialized"<<endl;
  }
  shutdownUnlock();

  return back;
}





int MpgPlugin::processThreadCommand(Command* command) {

  int id=command->getID();
  int arg;
  switch(id) {
  case _COMMAND_SEEK: {
    //
    // mapping from second to bytePosition
    //
    if (mpegStreamPlayer->isInit()==false) {
      command->print("MPGPLUGIN:ignore command in _STREAM_STATE_FIRST_INIT");
    } else {
      arg=command->getIntArg();
      int pos=mpegVideoLength->getSeekPos(arg);
      Command seek(_COMMAND_SEEK,pos);
      // insert correct values
      mpegStreamPlayer->processThreadCommand(&seek);
    }
    return _RUN_CHECK_CONTINUE;
  }
  
  default:
    // we forward the command to the mpegStreamPlayer
    mpegStreamPlayer->processThreadCommand(command);
  }

  // use default
  return (DecoderPlugin::processThreadCommand(command));
}


