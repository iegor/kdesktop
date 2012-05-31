/*
  concret OutputClass for yaf text consol player
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "yafOutputStream.h"
#include <iostream>

using namespace std;

#define _AUDIO_DELAY_STAMPS 200




YafOutputStream::YafOutputStream(InputInterface* input) {
  lInternalDevice=false;
  bytes=0;
  allWrite=0;
  writeToBuffer=0;
  lOpen=false;
  directOutput=OutPlugin::createOutputStream(_OUTPUT_LOCAL,_OUTPUT_THREADSAFE);


  this->input=input;
  ::pipe(fd);

  if (errno < 0) {
    perror("YafOutputStream pipe");
    exit(0);
  }
  input->addFileDescriptor(fd[0]);
}


YafOutputStream::~YafOutputStream() {
  delete directOutput;
}


int YafOutputStream::audioSetup(int frequency,int stereo,
				int sign,int big,int bits) {

  cout << "Command:0 Msg:streamInfo-Start"<<endl;
  cout << "Command:0 Msg:streamInfo-Channels "<<stereo+1<<endl;
  
  cout << "Command:0 Msg:streamInfo-SampleSize "<<bits<<endl;
  cout << "Command:0 Msg:streamInfo-Speed "<<frequency<<endl;
  cout << "Command:0 Msg:streamInfo-End"<<endl;

  directOutput->audioSetup(frequency,stereo,sign,big,bits);

  return true;
}

void YafOutputStream::audioOpen() {
  if (lInternalDevice) {
    directOutput->audioOpen();
  }
}

void YafOutputStream::audioClose(void) {
  if (lInternalDevice) {
    // never close internal audio, because
    // its not possible to open it again fast enough
    //directOutput->audioClose();
  } else {
    directOutput->audioClose();
  }
}


int YafOutputStream::audioPlay(TimeStamp* startStamp,
			       TimeStamp* endStamp,char *buffer, int size) {

  int ret=0;
  if (lInternalDevice) {
    directOutput->audioPlay(startStamp,endStamp,buffer,size);
  }


  
  if (lOpen) {
    // if we are doing video, we even have to sync
    // audio writing.
    // Because yaf sends the audio data to another process
    // but the video data _not_ the audio data is directly
    // flushed here, but maybe stay on the other side
    // in a very big buffer
    // until the protocol has no better handle
    // for this, we must make sure to
    // dont get the buffer on the other
    // side too big.
    // we sleep some time until we write the next sample




    directOutput->audioPlay(startStamp,endStamp,buffer,size);
 

    ret=::send(sd,buffer,size,0);


 
 
    

    if (ret < 0) {
      
      closeStream();
      cout << "fifo error in streamWriter";
      perror("writeStream");
      exit(-1);
      return ret;
    }

    bytes=bytes+size;
    allWrite+=size;
    return size;
  }

    


  bytes=bytes+size;
  allWrite+=size;
  return size;
}


void YafOutputStream::audioFlush() {
  // notify command line, that plugin state changed
  input->write(fd[1],"update\n");

  if (lInternalDevice) {
    directOutput->audioFlush();
  }
}

int YafOutputStream::getPreferredDeliverSize() {
  return directOutput->getPreferredDeliverSize();
}




int YafOutputStream::openWindow(int width, int height,const char *title) {
  return directOutput->openWindow(width,height,title);
}


void YafOutputStream::closeWindow() {
  directOutput->closeWindow();
}

void YafOutputStream::flushWindow() {
  directOutput->flushWindow();
} 

PictureArray* YafOutputStream::lockPictureArray() {
  return directOutput->lockPictureArray();
}


void YafOutputStream::unlockPictureArray(PictureArray* pictureArray) {
  directOutput->unlockPictureArray(pictureArray);
}
 

int YafOutputStream::getFrameusec() {
  return directOutput->getFrameusec();
}



int YafOutputStream::getOutputInit() {
  return directOutput->getOutputInit();
}


void YafOutputStream::setOutputInit(int lInit) {
  directOutput->setOutputInit(lInit);
}




void YafOutputStream::writeInfo(PluginInfo* pluginInfo) {
  char* url=pluginInfo->getUrl();
  const char* nameStart=strrchr(url,'/');

  if (nameStart==NULL) {
    nameStart="noname";
  } else {
    nameStart++;
    if (strlen(nameStart) == 0) {
      nameStart="noname";
    }
  }
      
  cout << "Command:0 Msg:musicinfo-Start"<<endl;
  cout << "Command:0 Msg:song_filename "<<pluginInfo->getUrl()<<endl;
  cout << "Command:0 Msg:song_name "<<nameStart<<endl;
  cout << "Command:0 Msg:song_len  "<<pluginInfo->getLength()<<endl;
  cout << "Command:0 Msg:song_jumps 0"<<endl;
  cout << "Command:0 Msg:musicinfo-End"<<endl;
}






void YafOutputStream::setBytesCounter(long value) {
  bytes=value;
  writeToBuffer=value;
}



int YafOutputStream::isOpenStream() {
  return lOpen;
}


void YafOutputStream::setStreamFile(const char* filename) {
  if (lOpen == true) {
    closeStream();
  }
  this->filename=filename;
}


int YafOutputStream::openStream() {
  if (lOpen == true) {
    cout << "stream already open! call ignored"<<endl;
    return sd;
  }
  sd=::socket(AF_UNIX,SOCK_STREAM,0);
  if (sd < 0) {
    perror("sd-Socket StreamWriter");exit(1);
  }
  
  unsigned int i=0;
  sockad.sun_family=AF_UNIX;

  while(i<strlen(filename)) {
        sockad.sun_path[i]=filename[i];
        i++;
  }
  sockad.sun_path[i]=0;
  if (::connect(sd,(sockaddr*)&sockad,strlen(filename)+2)<0) {
    perror("connect StreamWriter");exit(1);
  }
  if (sd > 0) {
    lOpen=true;
  }
  return sd;
}


int YafOutputStream::closeStream() {
  int ret=0;
  if (lOpen) {
    ret=close(sd);
    lOpen=false;
    filename=NULL;
  } else {
    cout << "stream already closed. call ignored!"<<endl;
  }
  return ret;
}


void YafOutputStream::internalDevice(int lInternal){
  lInternalDevice=lInternal;
}



long YafOutputStream::getBytesCounter() {
  return bytes;
}


long YafOutputStream::getAllWriteCounter() {
  return allWrite;
}



void YafOutputStream::config(const char* key,const char* value,
			     void* user_data) {

  directOutput->config(key,value,user_data);
  
}

