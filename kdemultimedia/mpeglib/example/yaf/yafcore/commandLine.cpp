/*
  An abstraction of a Command-Line
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "commandLine.h"
#include <iostream>
using namespace std;  

CommandLine::CommandLine(){
  int i;
  for (i=0; i < 10; i++) {
    command[i].identifier=new Buffer(20);
    command[i].value=new Buffer(100);
  }
  commandCount=0;

}

CommandLine::~CommandLine(){

  int i;
  for (i=0; i < 10; i++) {
    delete command[i].value;
    delete command[i].identifier;

  }

}


int CommandLine::getCommandCount() {
  return commandCount;
}

void CommandLine::setCommandCount(int commandCount){
  this->commandCount=commandCount;
}


char* CommandLine::getIdentifier(int i){
  return (command[i].identifier)->getData();
}

void CommandLine::clear() {
  int i;
  for (i=0; i < 10; i++) {
    command[i].identifier->clear();
    command[i].value->clear();
  }
  commandCount=0;
}

char* CommandLine::getValue(int i) {
   return (command[i].value)->getData();
}

void CommandLine::setIdentifier(int i,char* identifier){
  (command[i].identifier)->setData(identifier);
}


void CommandLine::setValue(int i,char* value) {
  (command[i].value)->setData(value);
}

void CommandLine::printCommand() {
  int i;
  for(i=0;i<commandCount;i++) {
    cout << "Command:" << i << "\n";
    cout << "identifier:" << (command[i].identifier)->getData() << "\n";
    cout << "value:" << (command[i].value)->getData() << "\n";
  }
}



