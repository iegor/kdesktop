/*
  base class for the player plugin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#include "decoderPlugin.h"

#include <iostream>

using namespace std;


static void *playerThread(void *arg){
  ((DecoderPlugin*)arg)->idleThread();
  return NULL;
}   

static int instanceCnt=0;

DecoderPlugin::DecoderPlugin(){
  input=NULL;
  output=NULL;
  commandPipe=new CommandPipe();
  threadCommand=new Command(_COMMAND_NONE);
  abs_thread_cond_init(&streamStateCond);
  abs_thread_mutex_init(&streamStateMut);
  abs_thread_mutex_init(&shutdownMut);

  lCreatorLoop=true;
  lDecoderLoop=false;
  linDecoderLoop=false;
  streamState=_STREAM_STATE_EOF;
  lDecode=false;
  lhasLength=false;
  // this default is necessary. if you have a blocking
  // device and we start automatic the thread
  // may block on it and all commands (including play)
  // blocks everything
  // *you should not use autoplay*
  lAutoPlay=false;

  pluginInfo=new PluginInfo();
  runCheck_Counter=0;
  decode_loopCounter=0;
  instance=instanceCnt;
  instanceCnt++;
  abs_thread_create(&tr,playerThread,this);

  // we send a ping. because this signal is synchron
  // we block until the thread is started and
  // has read the ping singnal
  Command cmd(_COMMAND_PING);
  insertSyncCommand(&cmd);


}


DecoderPlugin::~DecoderPlugin(){
  void* ret;
  lCreatorLoop=false;
  Command cmd(_COMMAND_CLOSE);
  insertAsyncCommand(&cmd);

  abs_thread_join(tr,&ret);

  abs_thread_cond_destroy(&streamStateCond);
  abs_thread_mutex_destroy(&streamStateMut);
  abs_thread_mutex_destroy(&shutdownMut);
  
  delete commandPipe;
  delete threadCommand;
  delete pluginInfo;
}

  
void DecoderPlugin::close(){
  // from here we can only walk to init or eof
  // in both cases we sometimes catch out decoderMut :-)
  Command cmd(_COMMAND_CLOSE);
  insertAsyncCommand(&cmd);
  shutdownLock();
  if (input != NULL) {
    input->close();
  }
  shutdownUnlock();
  insertSyncCommand(&cmd);
  waitForStreamState(_STREAM_STATE_EOF);
  input=NULL;
}


void DecoderPlugin::pause() {
  Command cmd(_COMMAND_PAUSE);
  insertSyncCommand(&cmd);
}

  


int DecoderPlugin::play() {
  Command cmd(_COMMAND_PLAY);
  insertSyncCommand(&cmd);

  return true;
}

int DecoderPlugin::seek(int second) {
  Command cmd(_COMMAND_SEEK,second);
  insertSyncCommand(&cmd);

  return true;
}

void DecoderPlugin::insertAsyncCommand(Command* cmd) {
  commandPipe->sendCommandNoWait(*cmd);
}

void DecoderPlugin::insertSyncCommand(Command* cmd) {
  commandPipe->sendCommand(*cmd);
}


void DecoderPlugin::shutdownLock() {
  abs_thread_mutex_lock(&shutdownMut);
}


void DecoderPlugin::shutdownUnlock() {
  abs_thread_mutex_unlock(&shutdownMut);
}

int DecoderPlugin::getTime(int lCurrent) {
  int secLen=getTotalLength();
  
  if (lCurrent==false) {
    return secLen;
  }
  shutdownLock();
  int byteLen=1;
  int pos=1;
  if (input != NULL) {
    pos=input->getBytePosition()+1;
    byteLen=input->getByteLength()+1;
  }
  int back=(int)(((double)pos/(double)byteLen) * (double)secLen);
  shutdownUnlock();
  return back;

}

int DecoderPlugin::getTotalLength() {
  cout << "plugin does not support total playtime reporting"<<endl;
  return 0;
}

int DecoderPlugin::seek_impl(int) {
  cout << "plugin does not support seek"<<endl;
  return false;
}




void DecoderPlugin::setOutputPlugin(OutputStream* output) {
  this->output=output;
}


int DecoderPlugin::setInputPlugin(InputStream* input) {
  this->input=input;

  if (!input) {
    cout << "input is NULL"<<endl;
    exit(0);
  }
  pluginInfo->setUrl(input->getUrl());


  // the command is synchron we block until the
  // thread has read it
  Command cmd(_COMMAND_START);
  insertSyncCommand(&cmd);


  // now that we know he has read it, we send another
  // command, this is only read if the thread is in the
  // decode_loop, and we then know that the streamState 
  // is FIRST_INIT
  Command ping(_COMMAND_PING);
  insertSyncCommand(&ping);
  

  if (lAutoPlay) {
    play();
  }
  return true;
}


void DecoderPlugin::config(const char* key,const char* value,void* ){
  if (strcmp(key,"-y")==0) {
    if (strcmp(value,"on")==0) {
      lAutoPlay=true;
    } else {
      lAutoPlay=false;
    }
  }
  
}

/**
   during shutdown the streamState is undefined until
   the thread has left the decode_loop().
   Make sure we wait for this.
*/
int DecoderPlugin::getStreamState() {
  shutdownLock();
  int back=streamState;
  shutdownUnlock();
  return back;
}


int DecoderPlugin::waitForStreamState(int state) {
  int back;
  abs_thread_mutex_lock(&streamStateMut);
  while ((streamState & state) == false) {
    abs_thread_cond_wait(&streamStateCond,&streamStateMut);
  }
  back=streamState;
  abs_thread_mutex_unlock(&streamStateMut);
  return back;
}


void DecoderPlugin::setStreamState(int streamState) {
  abs_thread_mutex_lock(&streamStateMut);
  this->streamState=streamState;
  abs_thread_cond_signal(&streamStateCond);
  abs_thread_mutex_unlock(&streamStateMut);
}


void DecoderPlugin::decoder_loop() {
  cout << "direct call decoder loop->plugin not found ???"<<endl;
  TimeWrapper::usleep(100000);
}


void* DecoderPlugin::idleThread() {

  while(lCreatorLoop) {
    linDecoderLoop=true;
    commandPipe->waitForCommand();
    commandPipe->hasCommand(threadCommand);
    int id=threadCommand->getID();
    switch(id) {
    case _COMMAND_START:
      lDecoderLoop=true;
      break;
    case _COMMAND_PING:
      break;
      /*
	default:
	threadCommand->print("ignoring non START command in idleThread");
      */
    }
    

    if (lDecoderLoop) {
      setStreamState(_STREAM_STATE_FIRST_INIT);
      linDecoderLoop=false;
      decode_loopCounter++;
      runCheck_Counter=0;
      shutdownLock();
      decoder_loop();
      lDecode=false;
      lDecoderLoop=false;
      lhasLength=false;     
      setStreamState(_STREAM_STATE_EOF);
      shutdownUnlock();
    } 
  }
  return NULL;
}


PluginInfo* DecoderPlugin::getPluginInfo() {
  return pluginInfo;
}


int DecoderPlugin::runCheck() {
  if (runCheck_Counter==0) {
    shutdownUnlock();
  }
  runCheck_Counter++;
  while (lDecoderLoop && lCreatorLoop) {

    // if we have an eof this always leads to 
    // a shutdown of the decode_loop thread
    // it has more priority than the resyn request
    if (input->eof()) {
      setStreamState(_STREAM_STATE_WAIT_FOR_END);
    }  
    //
    // if we are in _STREAM_STATE_RESYNC_COMMIT
    // we only leave it if command is _COMMAND_RESYNC_END  
    // (or close)

    //
    // check user commands
    // 
    if (lDecode==false) {
      commandPipe->waitForCommand();
      commandPipe->hasCommand(threadCommand);
    } else {
      if (commandPipe->hasCommand(threadCommand)==false) {
	// no commands and lDecode=true
	return true;
      }
    }

    // here we forward the command to a special
    // method who can handle everything
    // (the default method should work fine);
    int nextCheck= processThreadCommand(threadCommand);
    switch(nextCheck) {
    case _RUN_CHECK_CONTINUE:
      break;
    case _RUN_CHECK_FALSE:
      shutdownLock();
      return false;
    case _RUN_CHECK_TRUE:
      return true;
    default:
      cout << "unknown runCheck return command"<<endl;
      exit(0);
    }
        

  }

  shutdownLock();
  return false;
}

int DecoderPlugin::processThreadCommand(Command* command) {

  
  int id=command->getID();
  int intArg;

  //
  // if we are in _STREAM_STATE_RESYNC_COMMIT
  // we only leave it if command is _COMMAND_RESYNC_END  
  //
  if (streamState==_STREAM_STATE_RESYNC_COMMIT) {
    switch(id) {
    case _COMMAND_RESYNC_END:
      setStreamState(_STREAM_STATE_INIT);
      input->clear();
      break; 
    case _COMMAND_CLOSE:
      //
      // we return false so that the plugin clears
      // all its allocated classes
      // its a _must_ !!
      // in the next call we exit immediately
      return _RUN_CHECK_FALSE;

      /*  
    default:
      command->print("ignore command in _STREAM_STATE_RESYNC_COMMIT");
      */
    }
    return _RUN_CHECK_CONTINUE;
  }


  switch(id) {
  case _COMMAND_NONE:
    break;
  case _COMMAND_PING:
    break;
  case _COMMAND_PAUSE:
    lDecode=false;
    break;
  case _COMMAND_PLAY:
    lDecode=true;
    break;
  case _COMMAND_SEEK: {
    if (streamState==_STREAM_STATE_FIRST_INIT) {
      command->print("ignore command seek in _STREAM_STATE_FIRST_INIT");
    } else {
      intArg=command->getIntArg();
      seek_impl(intArg);
    }
    break;
  }
  case _COMMAND_CLOSE:
    //
    // we return false so that the plugin clears
    // all its allocated classes
    // its a _must_ !!
    // in the next call we exit immediately
    return _RUN_CHECK_FALSE;
  case _COMMAND_RESYNC_START:
    setStreamState(_STREAM_STATE_RESYNC_COMMIT);
    input->clear();
    break;
    /*
  default:
    cout << "unknown command id in Command::print"<<endl;
    */
  }
  return _RUN_CHECK_CONTINUE;
}


