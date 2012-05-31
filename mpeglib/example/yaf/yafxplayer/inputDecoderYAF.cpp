/*
  base Implementation of a yaf decoder
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#define DEBUG cout << "Command:1 Msg:"

#include "inputDecoderYAF.h"
#include "xplayerCommand.defs"
#include "../yafcore/yafCommand.defs"
#include <iostream>
  
using namespace std;

InputDecoderYAF::InputDecoderYAF(DecoderPlugin* plugin,
				 YafOutputStream* output) 
  : InputDecoderXPlayer(output) {

  this->plugin=plugin;
  this->output=output;
  input=NULL;

  lAutoPlay=true;
  lFileSelected=false;
  songPath=new Buffer(256);
}


InputDecoderYAF::~InputDecoderYAF(){
  delete songPath;
}


/** 
    This method is entered by the global loop. (xplayer_control.c)
    This method should never (!) block or it
    is unpossible to read commands on stdin!


    Note: YAF is not a threaded player this is why we need
    this doSomething method. Other player may already be threaded and
    offer a "control"/"working" channel. 
    These players won't need this loop.

*/

  
void InputDecoderYAF::doSomething(){

}


const char* InputDecoderYAF::processCommand(int command,const char* args){

  if (command == _PLAYER_UPDATE) {
    int state=plugin->getStreamState();
    if (state == _STREAM_STATE_EOF) {
      if (getMajorMode() != _PLAYER_MAJOR_MODE_OFF) {
	return processCommand(_PLAYER_OFF,"");
      }
    }
    return "";
  }
 

  if (command == _PLAYER_OPEN) {
    if (strlen(args) == 0) {
      return "no file";
    }
    if (lFileSelected == true) {
      processCommand(_PLAYER_CLOSE,"");
    }

    if (getOn() == true) {
      processCommand(_PLAYER_OFF,"");
    }
            
    if (getOn() == false) {
      processCommand(_PLAYER_ON,"");
    }
    output->setBytesCounter(0);
  
    input=InputPlugin::createInputStream(args,_INPUT_THREADSAFE);
    if (input == NULL) {
      cout << "createInputStream failed"<<endl;
      return processCommand(_PLAYER_OFF,"");
    }     
    lFileSelected=true;
    setMajorMode(_PLAYER_MAJOR_MODE_OPEN_TRACK);
    input->open(args);
    if (plugin->setInputPlugin(input) == false) {
      return processCommand(_PLAYER_OFF,"");
    }
    songPath->clear();
    songPath->append(args);
    if (lAutoPlay) {
      return processCommand(_PLAYER_PLAY,"");
    }
    return "";
  }
  
  if (command == _PLAYER_CLOSE) {
    if (lFileSelected) {
      processCommand(_PLAYER_PAUSE,"");
      plugin->close();
      
      delete input;
      input=NULL;
      setMajorMode(_PLAYER_MAJOR_MODE_CLOSE_TRACK);
      lFileSelected=false;
    }
    return "";
  }
    
    
  if (command == _PLAYER_PLAY) {
    if (lFileSelected) {
      setMajorMode(_PLAYER_MAJOR_MODE_PLAYING);
      plugin->play();
      return "";
    }
    return "no file";
  }
  
  if (command == _PLAYER_PAUSE) {
    int mode=getMajorMode();
    if (mode == _PLAYER_MAJOR_MODE_PLAYING) {
      plugin->pause();
      setMajorMode(_PLAYER_MAJOR_MODE_PAUSE);
    }

    return ""; 
  }

  if (command == _YAF_I_PLAYTIME) {
    int current=plugin->getTime(true);
    int total=plugin->getTime(false);
      
    cout << "Command:0 Msg:playtime current:"<<current<<" total:"<<total<<endl;
    return "";
  }

  // another YAF part:
  if (command == _PLAYER_JUMP) {
    if (lFileSelected) {
      int nSec=0;
      int mode=getMajorMode();
      processCommand(_PLAYER_PAUSE,"");
      sscanf(args,"%d",&nSec);  // convert string to int
      /* if the value is signed, do a relative jump */
      if(index(args, '-') || index(args, '+')) {
      	nSec += plugin->getTime(true);
      }
      // nPos is the time in seconds!!!!
      plugin->seek(nSec);
      output->setBytesCounter(0);

      if (mode == _PLAYER_MAJOR_MODE_PLAYING) {
	processCommand(_PLAYER_PLAY,"");
      }
      return "";
    }
    return "no file";
  }


  if (command == _YAF_I_RUNTIME) {
    if (strcmp("off",args)==0) {
      plugin->config("runtime","off",NULL);
    } else {
      plugin->config("runtime","on",NULL);
    }
  } 
  if (command == _PLAYER_MUSICINFO) {
    PluginInfo* plugInfo=plugin->getPluginInfo();
    output->writeInfo(plugInfo);
    return "";
  }
  
  if (command == _YAF_I_SELECT_A_LAYER) {
    plugin->config("AudioLayer",args,NULL);
    return "";
  }
  if (command == _YAF_I_SELECT_V_LAYER) {
    plugin->config("VideoLayer",args,NULL);
    return "";
  }

  if (command == _YAF_I_WRITE) {
    if (strcmp(args,"on")==0) {
      plugin->config("-w","true",NULL);
    } else {
      plugin->config("-w","false",NULL);
    }
    return "";
  }


  return (InputDecoderXPlayer::processCommand(command,args));
}




int InputDecoderYAF::getAutoPlay() {
  return lAutoPlay;
}

void InputDecoderYAF::setAutoPlay(int lAutoPlay) {
  this->lAutoPlay=lAutoPlay;
}
