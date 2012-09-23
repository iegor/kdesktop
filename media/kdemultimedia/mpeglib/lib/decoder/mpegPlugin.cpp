/*
  mpeg player plugin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "mpegPlugin.h"

#include "../mpegplay/mpegVideoStream.h"
#include "../mpegplay/proto.h"
#include "../mpegplay/mpegVideoHeader.h"

using namespace std;


MpegPlugin::MpegPlugin() {
  init();
}


MpegPlugin::~MpegPlugin() {
}


void MpegPlugin::init() {
  lCalcLength=false;
  mpegVideoHeader=NULL;
  mpegVideoStream=NULL;
}


void MpegPlugin::decoder_loop() {


  VideoDecoder* video=NULL;
  if (input == NULL) {
    cout << "MpegPlugin::decoder_loop input is NULL"<<endl;
    exit(0);
  }
  if (output == NULL) {
    cout << "MpegPlugin::decoder_loop output is NULL"<<endl;
    exit(0);
  }

  mpegVideoHeader=new MpegVideoHeader();
  mpegVideoStream=new MpegVideoStream(input);

  PictureArray* pictureArray;
  YUVPicture* pic;
  int skipMode=_SYNC_TO_NONE;
  // decode loop

  while(runCheck()) {
    switch(streamState) {
    case _STREAM_STATE_FIRST_INIT :
      if (mpegVideoStream->firstInitialize(mpegVideoHeader)==false) {

      } else {
	pluginInfo->setLength(getSongLength());
	
	// now create pictureArray from the sequence
	int width=mpegVideoHeader->getMB_Width()*16;
	int height=mpegVideoHeader->getMB_Height()*16;

	output->openWindow(width,height,(char*)"kmpg");
	video=new VideoDecoder(mpegVideoStream,mpegVideoHeader);
	setStreamState(_STREAM_STATE_INIT);
      }
      break;
    case _STREAM_STATE_INIT :
      //      cout << "mpeg _STREAM_STATE_INI"<<endl;
      if (skipMode==_SYNC_TO_GOP) {
	if (mpegVideoStream->nextGOP()==false) {
	  continue;
	}
	video->resyncToI_Frame();
      }
      if (skipMode==_SYNC_TO_PIC) {
	if (mpegVideoStream->nextPIC()==false) {
	  continue;
	}
      }
      skipMode=_SYNC_TO_NONE;
      setStreamState(_STREAM_STATE_PLAY);
      break;
    case _STREAM_STATE_PLAY :
      pictureArray=output->lockPictureArray();
      skipMode=video->mpegVidRsrc(pictureArray);

      if (skipMode != _SYNC_TO_NONE) {
	setStreamState(_STREAM_STATE_INIT);
      }
      pic=pictureArray->getYUVPictureCallback();
      if (pic == NULL) {
	// nothin to display
	break;
      }

      output->unlockPictureArray(pictureArray);
      pictureArray->setYUVPictureCallback(NULL);
      break;
    case _STREAM_STATE_WAIT_FOR_END:
      // exit while loop
      lDecoderLoop=false;
      break;
    default:
      cout << "unknown stream state:"<<streamState<<endl;
    }
  }

  output->flushWindow();
  // copy sequence back if needed
  if (video != NULL) {
    delete video;
  }
  delete mpegVideoStream;
  delete mpegVideoHeader;
  mpegVideoStream=NULL;
  mpegVideoHeader=NULL;
}





// here we can config our decoder with special flags
void MpegPlugin::config(const char* key,const char* value,void* user_data) {
  if (strcmp(key,"-c")==0) {
    lCalcLength=false;
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



int MpegPlugin::getSongLength() {
  int back=0;
  return back;
}






