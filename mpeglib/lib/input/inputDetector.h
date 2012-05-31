/*
  returns inputtype for a given string
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __INPUTDETECTOR_H
#define __INPUTDETECTOR_H



#define __INPUT_UNKNOWN   0
#define __INPUT_FILE      1
#define __INPUT_HTTP      2
#define __INPUT_CDI       3
#define __INPUT_BUFFER    4
#define __INPUT_CDDA      5

extern "C" {
#include <string.h>
}


class InputDetector {


 public:
  InputDetector();
  ~InputDetector();

  static int getInputType(const char* url);

  static int getProtocolType(const char* url);
  // returns new allocated string without protocol specifier
  static char* removeProtocol(const char* url);
  static char* getWithoutExtension(const char* url);
  static char* getExtension(const char* url);
  static char* removeExtension(const char* url,char* extension);
  static char* removeSlash(const char* url);
  static char* getFilename(const char* url);
  
 private:
  static int getProtocolPos(int type,const char* url);

};

#endif

