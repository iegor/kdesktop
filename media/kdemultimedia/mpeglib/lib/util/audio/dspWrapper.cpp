/*
  a wrapper for the audioDevice.
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "dspWrapper.h"
#include "audioIO.h"


#include "../../frame/pcmFrame.h"
#include "../../frame/floatFrame.h"

#include <iostream>

using namespace std;

DSPWrapper::DSPWrapper() {
  currentFormat=new PCMFrame(0);
  lopenDevice=false;
  lopenMixer=false;

  audioConstruct();
}

DSPWrapper::~DSPWrapper() {
  if (lopenDevice) {
    audioClose();
  }
  if (lopenMixer) {
    mixerClose();
  }
  audioDestruct(); 
  delete currentFormat;
}
 

int DSPWrapper::isOpenDevice() {
  return lopenDevice;
}

int DSPWrapper::openDevice() {
  if (lopenDevice==true) {
    return true;
  }
  lopenDevice=audioOpen();
  return lopenDevice;
}  
 
int DSPWrapper::closeDevice() {
  if (isOpenDevice() == true) {
    audioClose();
    currentFormat->setFrameFormat(-1,-1);
    lopenDevice=false;
  }
  return true;
}


int DSPWrapper::isOpenMixer() {
  return lopenMixer;
}


int DSPWrapper::getAudioBufferSize() {
  return ::getAudioBufferSize();
}


int DSPWrapper::openMixer() {
  lopenMixer=mixerOpen();
  return lopenMixer;
}  
 
int DSPWrapper::closeMixer() {
  if (isOpenMixer() == true) {
    mixerClose();
    lopenMixer=false;
  }
  return true;
}

    
int DSPWrapper::audioPlay(char *buf, int len) {
  return audioWrite(buf,len);
}

int DSPWrapper::audioSetup(int stereo,int sampleSize,int lSigned,
			   int lBigEndian,int freq) {

  
  if (isOpenDevice()==false) {
    cout << "device not open"<<endl;
    exit(-1);
  }
  /*
    cout << "sampleSize:"<<sampleSize<<endl;
    cout << "freq:"<<freq<<endl;
    cout << "stereo:"<<stereo<<endl;
    cout << "lSigned:"<<lSigned<<endl;
    cout << "lBigEndian:"<<lBigEndian<<endl;
  */
  audioInit(sampleSize,freq,stereo,lSigned,lBigEndian);
  if (currentFormat->getSampleSize() != sampleSize) {
    cout << "FIXME: pcmFrame with sampleSize:"<<sampleSize<<endl;
  }
  currentFormat->setFrameFormat(stereo,freq);
  return true;
}

int DSPWrapper::audioSetup(AudioFrame* audioFrame) {
  if (audioFrame == NULL) {
    cout << "audioFrame NULL: DSPWrapper:audioSetup"<<endl;
    exit(0);
  }
  if(audioFrame->isFormatEqual(currentFormat)==false) {
    audioSetup(audioFrame->getStereo(),audioFrame->getSampleSize(),
	       audioFrame->getSigned(),audioFrame->getBigEndian(),
	       audioFrame->getFrequenceHZ());
  }
  return true;
}


int DSPWrapper::audioPlay(PCMFrame* pcmFrame) {
  if (pcmFrame == NULL) {
    cout << "pcmFrame NULL: DSPWrapper:audioPlay"<<endl;
    exit(0);
  }
  if(pcmFrame->isFormatEqual(currentFormat)==false) {
    audioSetup(pcmFrame->getStereo(),pcmFrame->getSampleSize(),
	       pcmFrame->getSigned(),pcmFrame->getBigEndian(),
	       pcmFrame->getFrequenceHZ());
  }
  int len=pcmFrame->getLen()*2;
  int played=audioPlay((char*)pcmFrame->getData(),len);
  return (len == played);
}


//
// Misuse our internal currentFormat for the float->int conversion.
//

int DSPWrapper::audioPlay(FloatFrame* floatFrame) {
  if (floatFrame == NULL) {
    cout << "floatFrame NULL: DSPWrapper:audioPlay"<<endl;
    exit(0);
  }
  if(floatFrame->isFormatEqual(currentFormat)==false) {
    audioSetup(floatFrame->getStereo(),floatFrame->getSampleSize(),
	       floatFrame->getSigned(),floatFrame->getBigEndian(),
	       floatFrame->getFrequenceHZ());
  }

  int tmpLen=currentFormat->getLen();
  if (tmpLen < floatFrame->getLen()) {
    delete currentFormat;
    currentFormat=new PCMFrame(floatFrame->getLen());
    floatFrame->copyFormat(currentFormat);
  }
  currentFormat->clearrawdata();
  currentFormat->putFloatData(floatFrame->getData(),floatFrame->getLen());
  return audioPlay(currentFormat);
}

void DSPWrapper::audioFlush() {
  closeDevice();
}


void DSPWrapper::setVolume(float leftPercent,float rightPercent) {
  if (isOpenMixer()) {
    mixerSetVolume((int)leftPercent,(int)rightPercent);
  } else {
    cout << "cannot set Mixer settings:not open!"<<endl;
  }   
}


void DSPWrapper::print() {
  cout<<"lopenDevice:"<<lopenDevice<<endl;
  cout<<"lopenMixer:"<<lopenMixer<<endl;
  currentFormat->print("currentFormat");

}
