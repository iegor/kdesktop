/*
  raw yuv stream player
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "yuvPlugin.h"

#include <iostream>

using namespace std;


YUVPlugin::YUVPlugin() {
  init();
}


YUVPlugin::~YUVPlugin() {
}


void YUVPlugin::init() {
  lCalcLength=false;
  nWidth=0;
  nHeight=0;
  imageType=PICTURE_YUVMODE_CR_CB;
  picPerSec=1.0;
}


void YUVPlugin::decoder_loop() {



  cout << "YUVPlugin::decoder_loop() 1"<<endl;
  if (input == NULL) {
    cout << "YUVPlugin::decoder_loop input is NULL"<<endl;
    exit(0);
  }
  if (output == NULL) {
    cout << "YUVPlugin::decoder_loop output is NULL"<<endl;
    exit(0);
  }


  PictureArray* pictureArray;


  int bytes=nWidth*nHeight;

  if ( (imageType == PICTURE_YUVMODE_CR_CB) ||
       (imageType == PICTURE_YUVMODE_CB_CR) ) {
    bytes=bytes+bytes/2;
  }
  if ( (imageType == PICTURE_RGB) ||
       (imageType == PICTURE_RGB_FLIPPED) ) {
    bytes=bytes*4;
  }

  


  // decode loop
  while(runCheck()) {
    switch(streamState) {
    case _STREAM_STATE_FIRST_INIT :
      output->openWindow(nWidth,nHeight,(char*)"yuv Viewer");
      pictureArray=output->lockPictureArray();
      cout << "pictureArray->setImageType"<<endl;
      pictureArray->setImageType(imageType);
      
      setStreamState(_STREAM_STATE_INIT);
      break;
    case _STREAM_STATE_INIT :
      setStreamState(_STREAM_STATE_PLAY);
      break;
    case _STREAM_STATE_PLAY : {
      // we read from input raw yuv bytes 
      // and display them.
      pictureArray=output->lockPictureArray();
      // now we have the color info
      YUVPicture* pic=pictureArray->getPast();
      unsigned char* dest=pic->getImagePtr();
      input->read((char*)dest,bytes);
      pic->setPicturePerSecond(picPerSec);
      // now inform subsystem about the frame to display
      pictureArray->setYUVPictureCallback(pic);
      //  display it
      output->unlockPictureArray(pictureArray);

      pictureArray->setYUVPictureCallback(NULL);
      break;
    }
    case _STREAM_STATE_WAIT_FOR_END:
      // exit while loop
      lDecoderLoop=false;
      cout << "mpegplugin _STREAM_STATE_WAIT_FOR_END"<<endl;
      break;
    default:
      cout << "unknown stream state:"<<streamState<<endl;
    }
  }

  cout << "*********mpegPLugin exit"<<endl;

  output->flushWindow();
  // copy sequence back if needed

  cout << "delete mpegVideoStream"<<endl;

}





// here we can config our decoder with special flags
void YUVPlugin::config(const char* key,const char* value,void* user_data) {
  if (strcmp(key,"-c")==0) {
    lCalcLength=false;
  }

  if (strcmp(key,"height")==0) {
    nHeight=atoi(value);
  }    
  if (strcmp(key,"width")==0) {
    nWidth=atoi(value);
  }    
  if (strcmp(key,"imageType")==0) {
    imageType=atoi(value);
    cout << "imageType:"<<imageType<<endl;
  }      
  if (strcmp(key,"picPerSec")==0) {
    picPerSec=atoi(value);
  }
  DecoderPlugin::config(key,value,user_data);
}



int YUVPlugin::getSongLength() {
  int back=0;
  return back;
}






