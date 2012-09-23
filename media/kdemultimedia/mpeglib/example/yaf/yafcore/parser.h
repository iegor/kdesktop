/*
  Checks if a valid Command Line is avaiable
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */





#ifndef __PARSER_H
#define __PARSER_H

#include "buffer.h"
#include "commandLine.h"

extern "C" {
#include <string.h>
	   }

class Parser {
  

  Buffer* parseString;
  CommandLine* commandLine;

 public:

  Parser();
  ~Parser();

  void setParseString(char* parseString);
  void parse();
  int isOK();
  CommandLine* getCommandLine();
  

 private:
  void parse(char* currentPos,int* nCommand);

};


#endif





