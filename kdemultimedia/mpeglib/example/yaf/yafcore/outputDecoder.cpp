/*
  Default output decoder
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#include "outputDecoder.h"
#include <iostream>

using namespace std;

#define DEBUG cout << "Command:1 Msg:"


OutputDecoder::OutputDecoder() {
  yafCommands=new CommandTableYAF();
  yafRuntime=new RuntimeTableYAF();

  appendCommandTable(yafCommands);
  appendRuntimeTable(yafRuntime);
  
}



OutputDecoder::~OutputDecoder() {
  delete yafCommands;
  delete yafRuntime;
}



void OutputDecoder::appendCommandTable(CommandTable* table) {
  commandTable.join(table);
}


void OutputDecoder::appendRuntimeTable(CommandTable* table) {
  runtimeTable.join(table);
}



CommandTable* OutputDecoder::getCommandTable() {
  return &commandTable;
}


CommandTable* OutputDecoder::getRuntimeTable() {
  return &runtimeTable;
}




void OutputDecoder::processCommandLine(CommandLine* commandLine){
  CommandTable* ct=&commandTable;  // ct , rt are shorter..
  CommandTable* rt=&runtimeTable;

  
  int command;
  int commandNr;
  const char* commandStr;
  const char* retString;
  const char* args;


  // The number of the command (unique for every command)
  commandNr=atoi(commandLine->getValue(0));

  // if commandNr is greater zero then we have a return command from decoder
  if (commandNr >= 40) {

    // the command (longName or shortName )
    commandStr=ct->getCommand(commandLine->getValue(2));
    // the int value of the command (faster for compare)
    command=ct->getNr(commandStr);

    // the Arguments of the command
    args=ct->getArgs(commandStr,commandLine->getValue(2));
    retString=commandLine->getValue(1);
    
    processReturnCommand(commandNr,command,retString,args);
    return;
  }

  // if commandNr < 40 then we have a runtime command from decoder

  if (commandNr < 40) {
    // the command (longName or shortName )
    commandStr=rt->getCommand(commandLine->getValue(1));
    // the int value of the command (faster for compare)
    command=rt->getNr(commandStr);

    // here I make a hack because it cannot be expected
    // that during debugging every debug-info has its clean
    // installed debug-identifer.
    // hack:
    // if the commandNr==1 (debug) and we have NO defined
    // debug identifier then we do not send -1 (as in all other case)
    // but the number 1 itsself.
    
    if ((commandNr == 1) && (command == -1)) {
     args=rt->getArgs(commandStr,commandLine->getValue(1));
     processRuntimeCommand(1,args);
     return;
    }

    // in *all* other cases the developer should
    // implement a clean protokoll with identifiers
    // (this leads to well defined interfaces :-)

    // the Arguments of the command
    args=rt->getArgs(commandStr,commandLine->getValue(1));
    
    processRuntimeCommand(command,args);
    return;
  }


}



int OutputDecoder::processRuntimeCommand(int command,const char* args){
  cout << command <<" * "<< args <<" * "<< endl;
  return false;
}

int OutputDecoder::processReturnCommand(int cmdNr,int cmdId,
					const char* ret,const char* args){
  cout << cmdNr <<" * "<< cmdId<<" * "<< ret<<" * " << args << endl;
  return false;
}




