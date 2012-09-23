/*
  generic interactive controller 
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "yaf_control.h"
#include "parser.h"
#include <iostream>

using namespace std;


void yaf_control(InputInterface* input,
		 OutputInterface* output,
		 InputDecoder* decoder) {

  Parser parser;
  CommandLine* commandLine;
  struct timeval timeout;
  const char* retCode;
  int status;
  
  timeout.tv_sec=0;
  timeout.tv_usec=0;

  output->setProtocolSyntax(true);  // we send "raw" protocol data
  input->addFileDescriptor(0);
 
  while(1) {

    status=decoder->getDecoderStatus();

    if (status == _DECODER_STATUS_EXIT) {
      break;
    }
    input->waitForLine();
    
    if (input->hasLine()) {
      parser.setParseString(input->getLine());
      parser.parse();
      
      if (parser.isOK()){

		
	commandLine=parser.getCommandLine();

	decoder->processCommandLine(commandLine);
	retCode=decoder->getReturnCode();

	// now get the part after Msg: (in the inputLine)

	output->lock();
	output->clearBuffer();
	output->appendBuffer(retCode);
	output->flushBuffer();
	output->unlock();
      } else {
	cout << "Error parsing input:"<<input->getLine()<<endl;
      }

      input->clearLine();
    } else {
      cout << "no line"<<endl;
    }
  }
  input->removeFileDescriptor(0);

}




