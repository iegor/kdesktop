/*
  C interface creator for input_plugins
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __INPUTPLUGIN_H
#define __INPUTPLUGIN_H

#include "httpInputStream.h"
#include "fileInputStream.h"
#include "cdromInputStream.h"
#include "bufferInputStream.h"
#include "threadSafeInputStream.h"
#include "cddaInputStream.h"
#include <string.h>

#include "inputDetector.h"
#include <kdemacros.h>

#define _INPUT_THREADSAFE     1

class KDE_EXPORT InputPlugin {

 public:
  InputPlugin();
  ~InputPlugin();

  static InputStream* createInputStream(int inputType);
  static InputStream* createInputStream(int inputType,int lThreadSafe);
  static InputStream* createInputStream(const char* dest);
  static InputStream* createInputStream(const char* dest,int lThreadSafe);
  static int getInputType(const char* dest);

};
#endif
