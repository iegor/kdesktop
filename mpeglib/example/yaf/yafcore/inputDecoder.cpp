/*
  process Messages on the decoder
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "inputDecoder.h"

#include "commandTable.h"
#include "commandTableYAF.h"
#include "yafRuntime.defs"
#include "commandLine.h"
#include <iostream>

using namespace std;

InputDecoder::InputDecoder() {
  commandTable=new CommandTable();
  yafCommands=new CommandTableYAF();
  appendCommandTable(yafCommands);
  status=_DECODER_STATUS_IDLE;
  lRuntimeInfo=true;
  returnBuffer= new Buffer(200);
  returnLine= new Buffer(200);

  commandCounter=-1;
  commandCounter=-1;
  commandId=-1;
  commandCounterString=NULL;
  
}


InputDecoder::~InputDecoder(){
  delete yafCommands;
  delete returnBuffer;
  delete returnLine;
  delete commandTable;
}



void InputDecoder::setRuntimeInfo(int lRuntimeInfo) {
  this->lRuntimeInfo=lRuntimeInfo;
}


int InputDecoder::getRuntimeInfo() {
  return lRuntimeInfo;
}


void InputDecoder::appendCommandTable(CommandTable* table){
  commandTable->join(table);
}




void InputDecoder::processCommandLine(CommandLine* commandLine){
  CommandTable* ct=commandTable;  // "ct" is shorter
  commandId=-1;

  // The number of the command (unique for every command)
  commandCounterString=commandLine->getValue(0);
  commandCounter=atoi(commandCounterString);

  // the command (longName or shortName )
  if (commandLine->getCommandCount() == 0) {
    clearReturnBuffer();
    appendReturnBuffer("no Msg");
  }    
  commandMsg=ct->getCommand(commandLine->getValue(1));
  if ((commandMsg == NULL) || (strlen(commandMsg) == 0)) {
    clearReturnBuffer();
    appendReturnBuffer("unknown Command");
    commandMsg=commandLine->getValue(1);

    return ;
  }

  // the int value of the command (faster for compare)
  commandId=ct->getNr(commandMsg);

  // the Arguments of the command
  commandArgs=ct->getArgs(commandMsg,(const char*) commandLine->getValue(1));

  retString=processCommand(commandId,commandArgs);

  if ((retString == NULL) || (strlen(retString) == 0)) {
    retString=(char*)"ok";
  }
  clearReturnBuffer();
  appendReturnBuffer(retString);
}


const char* InputDecoder::processCommand(int command,const char* args){

  if (command == _YAF_I_HELP) {
    if (strlen(args)==0) {
      commandTable->print();
    } else {
      commandTable->print(commandTable->getNr(args),true);
    }
    return "";
  }
  if (command == _YAF_I_RUNTIME) {
    if (strcmp("off",args)==0) {
      setRuntimeInfo(false);
      return "";
    }
    setRuntimeInfo(true);
    return "";
  }
  if (command == _YAF_I_QUIT) {
    ::exit(0);
    return "";
  }
  if (command == _YAF_I_WHATIS) {
    cout << "Yaf <y>et <a>nother <f>rontend" << endl;
    cout << endl;
    cout << "Yaf is an interactive command line oriented shell for decoders." \
	 << endl;
    cout << endl;
    cout << "Copyright (C) 1998,1999  Martin Vogt <mvogt@rhrk.uni-kl.de>"\
	 <<endl;
    cout << "This program is free software; you can redistribute "\
         << "it and/or modify"<<endl;
    cout << "it under the terms of the GNU Library General Public License "\
         << "as published by"<<endl;
    cout << "the Free Software Foundation." <<endl;

    cout << "For more information look at the file COPYRIGHT in "\
	 << "this package" <<endl;
    cout << endl;
    cout << "THIS SOFTWARE COMES WITH ABSOLUTELY NO WARRANTY! " \
         << "USE AT YOUR OWN RISK!"<<endl;
    return "";
  }
      
  
  if (command == _YAF_I_PING) {
    return "";
  }
  if (command == _YAF_I_PROTOCOL) {
    return "";
  }
  if (command == _YAF_I_NOPROTOCOL) {
    return "";
  }

  return "unknown Command";
}


/**
   These two functions are entered by the decoder
   thread  [START]
*/


void InputDecoder::doSomething(){
  cout << "did something"<<endl;
}



int InputDecoder::getDecoderStatus() {
  return status;
}



void InputDecoder::setDecoderStatus(int newState) {

  // First start a new thread with start()!
  // Their is no recovery if the thread status is set to exit!
  if (status == _DECODER_STATUS_EXIT) {
    return;
  }
  status=newState;
}



void InputDecoder::clearReturnBuffer() {
  returnBuffer->clear();
}


void InputDecoder::appendReturnBuffer(const char* str) {
  returnBuffer->append(str);
}



const char* InputDecoder::getReturnCode() {
  // now we send back the return code
  
  if ((commandId != -1) && (commandTable->getReturnFlag(commandId)==false)) {
    return "";
  }

  returnLine->clear();
  returnLine->append("Command:");


  // The number of the command (unique for every command)
  // 0: the command nr
  returnLine->append(commandCounterString);
  returnLine->append(" Ret:(");
  returnLine->append(returnBuffer->getData());
  returnLine->append(") Msg:");

  
  // now get the part after Msg: (in the inputLine)

  returnLine->append(commandMsg);
  returnLine->append(" ");
  returnLine->append(commandArgs);



  return (const char*)returnLine->getData();

}

  

