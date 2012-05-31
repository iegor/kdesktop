/*
  length detection etc.. for mpeg audio
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#ifndef _FROM_SOURCE
#define _FROM_SOURCE 1
#endif
#include "dxHead.h"
#include <string.h>
#include "mpegAudioInfo.h"
#include "mpegAudioHeader.h"
#include "mpegAudioStream.h"
#include "mpegAudioFrame.h"

#include <iostream>

using namespace std;

#define _NEED_LENGTH  1
#define _NEED_ID3     2
#define _NEED_NOTHING 3


MpegAudioInfo::MpegAudioInfo(FileAccess* input) {
  xHeadData=new XHEADDATA();
  xHeadData->toc=new unsigned char[101];
  lXingVBR=false;
  id3=new ID3TAG();
  this->input=input;
  mpegAudioFrame =new MpegAudioFrame();
  mpegAudioStream=new MpegAudioStream();
  mpegAudioHeader=new MpegAudioHeader();
  reset();
}


MpegAudioInfo::~MpegAudioInfo() {
  delete[] (xHeadData->toc);
  delete xHeadData;
  delete id3;
  delete mpegAudioStream;
  delete mpegAudioHeader;
  delete mpegAudioFrame;
}


int MpegAudioInfo::getByteDirect() {
  unsigned char byte;
  if (input->read((char*)&byte,1) != 1) {
    leof=true;
    return -1;
  }
  return (int)byte;
}

void MpegAudioInfo::reset() {
  length=0;
  initState=_NEED_LENGTH;
  lNeedInit=true;
}


int MpegAudioInfo::getNeedInit() {
  return lNeedInit;
}

void MpegAudioInfo::setNeedInit(int lNeedInit) {
  this->lNeedInit=lNeedInit;
}


int MpegAudioInfo::initialize() {
  long fileSize=input->getByteLength();
  switch(initState) {
  case _NEED_NOTHING:
    return true;
    break;
  case _NEED_LENGTH:
    if (initializeLength(fileSize) == true) {
      initState=_NEED_ID3;
    }
    return false;
  case _NEED_ID3:
    if (initializeID3(fileSize) == true) {
      initState=_NEED_NOTHING;
      return true;
    }
    return false;
  default:
    cout << "unknown initState in MpegAudioInfo::initialize"<<endl;
    exit(0);
  }
  // never happens
  return true;
}


int MpegAudioInfo::initializeLength(long fileSize) {
  // if we are streaming don't touch the stream for length detection
  if (fileSize == 0) {
    return true;
  }
  int back=getFrame(mpegAudioFrame);
  if (back != true) {
    return back;
  }
  // found a valid frame (back == true)
  // store information in header.
  if (mpegAudioHeader->parseHeader(mpegAudioFrame->outdata()) == false) {
    cout << "parse header false"<<endl;
    return false;
  }
  calculateLength(fileSize);
  return back;
}

int MpegAudioInfo::initializeID3(long fileSize) {
  int pos=input->getBytePosition();
  if (input->seek(fileSize-128)<0) {
    return true;
  }
  parseID3();
  input->seek(pos);
  return true;
}


long MpegAudioInfo::getLength() {
  return length;
}


void MpegAudioInfo::calculateLength(long fileSize) {
  
  int totalframe=0;
  int framesize=mpegAudioHeader->getFramesize();
  if (framesize > 0) {
    totalframe=fileSize/framesize;
    
    if (parseXing(mpegAudioFrame->outdata(),mpegAudioFrame->len()) == true) {
      lXingVBR=true;
      totalframe=xHeadData->frames;
    }
  }
  
  float pcm=mpegAudioHeader->getpcmperframe();
  float wavfilesize=(totalframe*pcm);
  float frequence=(float)mpegAudioHeader->getFrequencyHz();
  length=0;
  if (frequence != 0) {
    length=(int)(wavfilesize/frequence);
  }
}


int MpegAudioInfo::parseXing(unsigned char* frame,int size) {

  int back=false;
  if (size < 152) {
    return false;
  }
  back=GetXingHeader(xHeadData,(unsigned char*)frame);
  return back;

}



long MpegAudioInfo::getSeekPosition(int second) {
  float length=getLength();
  long fileSize=input->getByteLength();
  long pos=0;
  if (length<1.0) {
    return 0;
  }
  float percent=(float)second/length;
  
  if (lXingVBR) {
    pos=SeekPoint(xHeadData->toc,(int)fileSize,100.0*percent);
    return pos;
  }
  pos=(long)(percent*(float)fileSize);
  return pos;
}


void MpegAudioInfo::parseID3() {
   
  id3->name    [0]=0;
  id3->artist  [0]=0;
  id3->album   [0]=0;
  id3->year    [0]=0;
  id3->comment [0]=0;
  id3->genre      =0;
  
  leof=false;

  while(leof == false) {
    if(getByteDirect()==0x54)
      if(getByteDirect()==0x41)
        if(getByteDirect()==0x47) {
	  input->read((char*)&id3->name    ,30);id3->name[30]=0;
	  input->read((char*)&id3->artist  ,30);id3->artist[30]=0;
	  input->read((char*)&id3->album   ,30);id3->album[30]=0;
	  input->read((char*)&id3->year    , 4);id3->year[4]=0;
	  input->read((char*)&id3->comment ,30);id3->comment[30]=0;
	  input->read((char*)&id3->genre   ,1);
          return;
        }
  }
}



void MpegAudioInfo::print(const char* msg) {
  cout << "MpegAudioInfo:"<<msg<<endl;
  cout << "Length (sec):"<<length<<endl;
  cout << "VBR:"<<lXingVBR<<endl;
  cout << "ID3: Name:"<<id3->name<<endl;
  cout << "ID3: Artist:"<<id3->artist<<endl;
  cout << "ID3: Album:"<<id3->album<<endl;
  cout << "ID3: year:"<<id3->year<<endl;
  cout << "ID3: genre:"<<(int)(id3->genre)<<endl;
  cout << "ID3: comment:"<<id3->comment<<endl;

}


int MpegAudioInfo::getFrame(MpegAudioFrame* frame) {
  int state=frame->getState();
  switch(state) {
  case FRAME_NEED: {
    int bytes=frame->canStore();
    int read=input->read((char*)inputbuffer,bytes);
    if (read <= 0) {
      // read error. reset framer
      frame->reset();
      break;
    }
    frame->store(inputbuffer,bytes);
    break;
  }
  case FRAME_WORK:
    frame->work();
    break;
  case FRAME_HAS:
    return true;
    break;
  default:
    cout << "unknown state in mpeg audio framing"<<endl;
    exit(0);
  }
  return false;
}
