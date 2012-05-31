/*
  generic Implementation of a cd-player
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#include "inputDecoderXPlayer.h"
#include "commandTableXPlayer.h"
#include <iostream>
using namespace std;

#define DEBUG cout << "Command:1 Msg:"


InputDecoderXPlayer::InputDecoderXPlayer(YafOutputStream* yafOutput) : 
  InputDecoder() {

  output=new OutputInterface(&cout);
  output->setProtocolSyntax(true);
  this->yafOutput=yafOutput;
  ct=new CommandTableXPlayer();
  
  setDecoderStatus(_DECODER_STATUS_IDLE);
  appendCommandTable(ct);
  majorMode=_PLAYER_MAJOR_MODE_OFF;
  setRuntimeInfo(false);
  // As default we expect a user which does not want to know
  // about the player status.
  // This is *not* true if the user is a controlling GUI.
  // A GUI should alwas send :
  // "MajorModeInfo on"
  setMajorModeInfo(true);   // as default we don't inform about the state!
  isOn=false;
} 

InputDecoderXPlayer::~InputDecoderXPlayer(){
  delete ct;
  delete output;
}

void InputDecoderXPlayer::setMajorModeInfo(int lDisplayMajorMode) {
  this->lDisplayMajorMode=lDisplayMajorMode;
}

int InputDecoderXPlayer::getMajorModeInfo() {
  return lDisplayMajorMode;
}



int InputDecoderXPlayer::getOn() {
  return isOn;
}

 
void InputDecoderXPlayer::setOn(int lOn) {
  isOn=lOn;
}




  
void InputDecoderXPlayer::setMajorMode(int mode) {
  const char* ptr;
  majorMode=mode;

  if (lDisplayMajorMode==false) {
    cout << "lDisplayMajorMode false"<<endl;
    return;
  }
  output->lock();
  output->clearBuffer();
  output->appendBuffer("Command:0 Msg:player-status ");
  if (majorMode == _PLAYER_MAJOR_MODE_OFF) {
    ptr="off";
  } else if (majorMode == _PLAYER_MAJOR_MODE_ON) {
    ptr="on";
  } else if (majorMode == _PLAYER_MAJOR_MODE_OPEN_TRACK) {
    ptr="open";
  } else if (majorMode == _PLAYER_MAJOR_MODE_CLOSE_TRACK) {
    ptr="close";
  } else if (majorMode == _PLAYER_MAJOR_MODE_PLAYING) {
    ptr="playing";
  } else if (majorMode == _PLAYER_MAJOR_MODE_PAUSE) {            
    ptr="pause";
  } else {
    ptr="unknown";
  }
  output->appendBuffer(ptr);
  if (majorMode == _PLAYER_MAJOR_MODE_OFF) {
    char val[40];
    long bytes=yafOutput->getBytesCounter();
    long allWrite=yafOutput->getAllWriteCounter();
    
    snprintf(val,40,"%ld %ld",bytes,allWrite);
    output->appendBuffer(" ");
    output->appendBuffer(val);
  }
  output->flushBuffer();  
  output->unlock();
}



int InputDecoderXPlayer::getMajorMode() {
  return majorMode;
}



void InputDecoderXPlayer::doSomething(){
  DEBUG << "Decoder did something" << endl;
  // after decoding is ready we close the file

  InputDecoder::doSomething();
}



const char* InputDecoderXPlayer::processCommand(int command,const char* args){

  if (command == _PLAYER_OFF) {
    if (isOn == true) {
      isOn=false;
      processCommand(_PLAYER_PAUSE,"");
      processCommand(_PLAYER_CLOSE,"");
     
      setMajorMode(_PLAYER_MAJOR_MODE_OFF);
      yafOutput->setBytesCounter(0);
    }
    return"";
  }
  
  if (command == _PLAYER_ON) {
    if (isOn == false) {
      setMajorMode(_PLAYER_MAJOR_MODE_ON);
      isOn=true;
    }
    return"";
  }


  if (command == _PLAYER_OPEN) {
    setMajorMode(_PLAYER_MAJOR_MODE_OPEN_TRACK);
    return"";
  }
  
  if (command == _PLAYER_CLOSE) {
    setMajorMode(_PLAYER_MAJOR_MODE_CLOSE_TRACK);
    return"";
  }
 
  if (command == _PLAYER_PLAY) {
    setDecoderStatus(_DECODER_STATUS_WORKING);
    setMajorMode(_PLAYER_MAJOR_MODE_PLAYING);
    return"";
  }
 
  if (command == _PLAYER_PAUSE) {
    setMajorMode(_PLAYER_MAJOR_MODE_PAUSE);
    return"";
  }   

  if (command == _PLAYER_VERBOSE) {
    if (strcmp(args,"off")==0) {
      setMajorModeInfo(false); 
    } else {
      setMajorModeInfo(true); 
    }
    return"";
  } 

  if (command == _PLAYER_OUTPUTFILE) {
    int ret;
    if (yafOutput->isOpenStream() == true) {
      return "already output file selected";
    }    
    yafOutput->setStreamFile(args);
    // now we have set the fifo. But we must say the parent
    // process that we start with waiting

    cout << "Command:0 Msg:fileopen before"<<endl;
    ret=yafOutput->openStream();
    cout << "Command:0 Msg:fileopen after"<<endl;

    if (ret < 0) {
      return "cannot open outfile";
    }
    return "";
  }
  if (command == _PLAYER_CLOSEOUTPUTFILE) {
    if (yafOutput->isOpenStream() == false) {
      return "no output file selected";
    }
    yafOutput->closeStream();
    return "";
  }

  if (command == _PLAYER_INTERNALAUDIO) {
    if (strcmp("on",args)==0) {
      yafOutput->internalDevice(true);
      return "";
    }
    yafOutput->internalDevice(false);
    return "";
  }
   
  if (command == _PLAYER_SLEEP) {
    int nSec;
    sscanf(args,"%d",&nSec);  // convert string to int
    sleep(nSec);
    return "";
  }
  if (command == _PLAYER_CLEAR) {
    yafOutput->setBytesCounter(0);
    return "";
  }
    
  return InputDecoder::processCommand(command,args);
}









