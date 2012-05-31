/*
  concret OutputClass
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "dspX11OutputStream.h"
#include "../util/audio/dspWrapper.h"
#include "windowOut.h"
#include "avSyncer.h"
#include "yuvDumper.h"

#include <iostream>

using namespace std;

DspX11OutputStream::DspX11OutputStream(int bufferSize) {
  dspWrapper=new DSPWrapper();
  x11Window=new WindowOut();
  avSyncer=new AVSyncer(bufferSize);
  yuvDumper=new YUVDumper();

  audioTime=new AudioTime();

  lPerformance=false;
  lneedInit=false;
  lVideoInit=false;
  lBufferSet=false;
  lYUVDump=false;
}


DspX11OutputStream::~DspX11OutputStream() {
  delete dspWrapper;
  delete x11Window;

  delete avSyncer;
  delete audioTime;
  delete yuvDumper;
}



int DspX11OutputStream::audioSetup(int frequency,int stereo,
				   int sign,int big,int sixteen) {

  dspWrapper->audioSetup(stereo,sixteen,sign,big,frequency);
  audioTime->setFormat(stereo,sixteen,frequency,sign,big);
  avSyncer->audioSetup(frequency,stereo,sign,big,sixteen);
  if (dspWrapper->isOpenDevice() == true) {
    if (lBufferSet==false) {
      int size=dspWrapper->getAudioBufferSize();
      avSyncer->setAudioBufferSize(size);
    }
  }
  return true;
}


void DspX11OutputStream::audioClose(void) {
  avSyncer->audioClose();
  dspWrapper->closeDevice();
}


void DspX11OutputStream::audioOpen() {
  if (dspWrapper->isOpenDevice() == false) {
    dspWrapper->openDevice();
    if (lBufferSet==false) {
      int size=dspWrapper->getAudioBufferSize();
      avSyncer->setAudioBufferSize(size);
    }    
  }
}




int DspX11OutputStream::audioPlay(TimeStamp* startStamp,
				  TimeStamp* endStamp,char *buffer, int size) {

  if (lneedInit) {
    cout << "FIXME. work on audioFrames!!"<<endl;
    lneedInit=false;
  }

  if (lPerformance==false) {
    //
    // Now feed data smoothly into the dsp/avSyncer
    //
    int pos=0;
    int rest=size;
    int inc=getPreferredDeliverSize();
    int len;

    while (rest > 0) {
      len=rest;
      if (len>inc) {
	len=inc;
      }
      if (dspWrapper->isOpenDevice()) {
	if (dspWrapper->audioPlay(buffer,len) != len) {
	  cout << "write error to dsp"<<endl;
	  lneedInit=true;
	  return size-rest;
	}
      }
      
      avSyncer->audioPlay(startStamp,endStamp,buffer,len);
      buffer+=len;
      rest-=len;
    }
    return size;
  } else {
    return size;
  }
}


int DspX11OutputStream::getPreferredDeliverSize() {
  if (avSyncer->getPreferredDeliverSize() <= 500) {
    return 500;
  }
  return avSyncer->getPreferredDeliverSize();
}


int DspX11OutputStream::openWindow(int width, int height,const char *title) {
  int back=x11Window->openWindow(width,height,title);
  setOutputInit(true);
  if (lYUVDump) {
    yuvDumper->openWindow(width,height,title);
  }
  return back;
}

int DspX11OutputStream::x11WindowId() {
  return x11Window->x11WindowId();
}


void DspX11OutputStream::closeWindow() {
  x11Window->closeWindow();
}

void DspX11OutputStream::flushWindow() {
  x11Window->flushWindow();
}


PictureArray* DspX11OutputStream::lockPictureArray() {
  return x11Window->lockPictureArray();
}


void DspX11OutputStream::unlockPictureArray(PictureArray* pictureArray) {


  
  YUVPicture* pic=pictureArray->getYUVPictureCallback();

  if (lYUVDump) {
    yuvDumper->unlockPictureArray(pictureArray);
  }
  if (avSyncer->syncPicture(pic)==false) {
    return;
  }

  x11Window->unlockPictureArray(pictureArray);


}

int DspX11OutputStream::getFrameusec() {
  return avSyncer->getFrameusec();
}

  

int DspX11OutputStream::getOutputInit() {
  return lVideoInit;
}


void DspX11OutputStream::setOutputInit(int lInit) {
  this->lVideoInit=lInit;
}
 
 

void DspX11OutputStream::config(const char* key,
				const char* value,void* user_data) {

  cout << "key:"<<key<<endl;
  if (strcmp(key,"-s")==0) {
    avSyncer->config(key,value,user_data);
  }
  if (strcmp(key,"-b")==0) {
    lBufferSet=true;
    int size=strtol(value,(char **)NULL,10);
    cout << "simulated audio buffersize:"<<size<<" bytes"<<endl;
    avSyncer->setAudioBufferSize(size);
  }
  if (strcmp(key,"-p")==0) {
    lPerformance=true;
    avSyncer->config(key,value,user_data);
  }  
  if (strcmp(key,"yufDump")==0) {
    int method=atoi(value);
    switch(method) {
    case 2:
      yuvDumper->setMethod(_DUMP_YUV_AS_STREAM);
      break;
    default:
      cout << "unknown dump method"<<endl;
    }
    lYUVDump=true;
  }  
  x11Window->config(key,value,user_data);
}


AVSyncer* DspX11OutputStream::getAVSyncer() {
  return avSyncer;
}



