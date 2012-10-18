/*
  This class can wait for an input by the user
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __INPUTINTERFACE_H
#define __INPUTINTERFACE_H


#include "buffer.h"
#include <fstream>
#include <kdemacros.h>

extern "C" {
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
}

// uncomment this for debugging the input stream
// (written to file instream.dbg)
//#define _DEBUG_INPUT

#define _TIMEOUT_MS   2000

class MultiReader;

class KDE_EXPORT InputInterface {

  int currentCommandNumber;

  int protocolSyntax;
  Buffer* currentLine;
  Buffer* rawLine;
  MultiReader* multiReader;
  Buffer* loopback;
  Buffer* yafScript;
  std::ifstream* yafInput;


 public:

  InputInterface();
  ~InputInterface();

  int addFileDescriptor(int fd);
  void removeFileDescriptor(int fd);
  int write(int fd,const char* txt);

  /**
     waits until a complete Line is entered 
  */
  void waitForLine();

  /**
    returns true if a complete new InputLine is avaiable
  */

  int hasLine();

  int getCurrentCommandNumber();

  void increaseCurrentCommandNumber();

  /**
     clears the input Line
  */
  void clearLine();
  

  /**
     returns the new Line
  */
  char* getLine();


  void setProtocolSyntax(int proto);
  void addInputLine(struct Buffer* buffer);

 private:
  void makeValidLine(char* line);
  void insertYafScript(std::ifstream* stream);

  
};

#endif


   
