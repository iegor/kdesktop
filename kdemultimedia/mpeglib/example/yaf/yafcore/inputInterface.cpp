/*
  This class can wait for an input by the user
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#include <config.h>

#include "inputInterface.h"
#include "multiReader.h"
#include <iostream>

using namespace std;

InputInterface::InputInterface(){

  currentLine= new Buffer(300);
  rawLine=new Buffer(300);
  loopback=new Buffer(300);
  protocolSyntax=false;
  currentCommandNumber=42;
  multiReader=new MultiReader();
  yafInput=new ifstream("yaf.script");
  if (yafInput->fail() == false) {
    cout << "Command:0 Msg:comment found yaf.script. Parsing first"<<endl;
    insertYafScript(yafInput);
  }
  yafInput->close();

}

InputInterface::~InputInterface(){
  delete yafInput;
  delete multiReader;
  delete currentLine;
  delete rawLine;
  delete loopback;
}


int InputInterface::addFileDescriptor(int fd) {
  int back;
  back=multiReader->add(fd);
  return back;
}


void InputInterface::removeFileDescriptor(int fd) {
  multiReader->remove(fd);
}


int InputInterface::getCurrentCommandNumber() {
  int back=0;
  back=currentCommandNumber;
  return back;
}

void InputInterface::increaseCurrentCommandNumber(){
  currentCommandNumber++;
}

int InputInterface::write(int fd,const char* txt) {
  int back=0;

  int len;
  loopback->clear();
  if (protocolSyntax == true) {
    snprintf(loopback->getData(),300,
	     "Command:41 Msg:%s",txt);
  } else {
    strlcpy(loopback->getData(),txt, loopback->getSize());
  }
  len =loopback->len();
  back=::write(fd,loopback->getData(),len);

  return back;
}



void InputInterface::waitForLine() {

  while(multiReader->hasLine() == false) {
    multiReader->waitForLine();
  }
  multiReader->getLine(rawLine);
  makeValidLine(rawLine->getData());

}

void InputInterface::setProtocolSyntax (int proto) {
  protocolSyntax=proto;
}



void InputInterface::makeValidLine(char* line) {

  int len;
  len=strlen(line);
  if (len >= 1) {
    if (line[len-1] == '\n') {
      line[len-1]='\0';
    }
  }
  if (strncmp("noprotocol",line,10) == 0){
    setProtocolSyntax(false);
    clearLine();
    increaseCurrentCommandNumber();
    snprintf(currentLine->getData(),300,
	     "Command:%d Msg:%s",currentCommandNumber,line);
    return;
  }
  if (strncmp("protocol",line,8) ==  0 ){
    setProtocolSyntax(true);
    clearLine();
    increaseCurrentCommandNumber();
    snprintf(currentLine->getData(),300,
	     "Command:%d Msg:%s",currentCommandNumber,line);
    return;
  }

  // Now the part if we do _not_ bypass the protocol-state

  if (protocolSyntax == false) {
    clearLine();
    increaseCurrentCommandNumber();
    snprintf(currentLine->getData(),300,
	     "Command:%d Msg:%s",currentCommandNumber,line);
  } else {
    increaseCurrentCommandNumber();
    strlcpy(currentLine->getData(),line, currentLine->getSize());
  }

  return;
}


void InputInterface::addInputLine(struct Buffer* buffer) {
  multiReader->add(buffer);
}


void InputInterface::insertYafScript(ifstream* stream) {
  char bst;
  int nBytes=0;
  Buffer yafScriptBuffer(300);

  if (stream->fail()) return;
  while (stream->eof()==false) {
    stream->get(bst);
    if (stream->eof()) break;

    yafScriptBuffer.append(&bst,1);
  }
  nBytes=(yafScriptBuffer.len()-1);  // EOF is a character we dont want

  addInputLine(&yafScriptBuffer);
}



int  InputInterface::hasLine() {
  if (currentLine->len() == 0) return 0;
  return 1;
}

void InputInterface::clearLine() {
  currentLine->clear();
}


char* InputInterface::getLine() {
  char* back=NULL;

  back=currentLine->getData();
#ifdef _DEBUG_INPUT
  ofstream infile("instream.dbg",ios::app);
  infile << back <<endl;
  infile.close();
#endif


  return back;
}



