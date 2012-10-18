/*
  Checks if a valid Command Line is avaiable
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "parser.h"


Parser::Parser() {
  commandLine=new CommandLine();
  parseString=new Buffer(200);
}


Parser::~Parser() {
  delete commandLine;
  delete parseString;
}

void Parser::setParseString(char* parseString){
  (this->parseString)->setData(parseString);
  // clear everything in commandline
  commandLine->clear();
}

void Parser::parse() {
  int commandCounter=0;
  
  parse(parseString->getData(),&commandCounter);
  commandLine->setCommandCount(commandCounter);
}


void Parser::parse(char* strStart,int* nCommand){
  char* doppelPkt;
  char* current;
  char* space;

  if (strlen(strStart) == 0) return;

  doppelPkt=strchr(strStart,':');
  if (doppelPkt == NULL) return;
  (*doppelPkt)='\0';
  commandLine->setIdentifier(*nCommand,strStart);
  strStart=++doppelPkt;
  current=strStart;

  // Now two possibilities:
  // a command with Msg : means the rest ist the value
  // a command different from Msg means: further processing

  if (strcmp("Msg",commandLine->getIdentifier(*nCommand)) == 0) {
    commandLine->setValue((*nCommand),current);
    (*nCommand)++;
    return;
  }

  // Now two possibilities:
  // a command with "Ret:" means string until the the ")" is the value
  // a command different from "Ret:" means: the string until the first space
  //                                        ist the value
  if (strcmp("Ret",commandLine->getIdentifier(*nCommand)) == 0) {
    char* seperator;
    seperator=strchr(current,')');
    if (seperator == NULL) {
      commandLine->setValue((*nCommand),current);
      (*nCommand)++;
      return;
    } else {
      (*seperator)='\0';
      current++;
      commandLine->setValue((*nCommand),current);
      (*nCommand)++;    
      seperator++;  
      if (strlen(seperator) == 0) return;
      parse(++seperator,&(*nCommand));
      return;
    }
  }
 
  space=strchr(current,' ');
  if (space == NULL) {
    commandLine->setValue((*nCommand),current);
    (*nCommand)++;
    return;
  }    
  (*space)='\0';
  commandLine->setValue((*nCommand),current);
  parse(++space,&(++(*nCommand)));
}


  

int Parser::isOK(){
  // a Commandline ist valid when it contains :

  // * 2 * (identifer/value)
  // * first identifier ist "Command"
  // * second identifier is "Msg"
  

  // or:
  // 3 identifier
  // first: "Command"
  // second "Ret"
  // third: "Msg"

  if (commandLine->getCommandCount() == 2){
    if (strcmp("Command",commandLine->getIdentifier(0)) != 0) return 0;
    if (strcmp("Msg",commandLine->getIdentifier(1)) != 0) return 0;
    return 1;
  }

  if (commandLine->getCommandCount() == 3){
    if (strcmp("Command",commandLine->getIdentifier(0)) != 0) return 0;
    if (strcmp("Ret",commandLine->getIdentifier(1)) != 0) return 0;
    if (strcmp("Msg",commandLine->getIdentifier(2)) != 0) return 0;
    return 1;
  }
 
  return 0;
}
  
CommandLine* Parser::getCommandLine(){
  return commandLine;
}

















