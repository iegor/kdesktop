/*
  generic output plugin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __OUTPLUGIN_H
#define __OUTPLUGIN_H


#include "dspX11OutputStream.h"
#include "artsOutputStream.h"
#include "threadSafeOutputStream.h"
#include <kdemacros.h>

#define _OUTPUT_LOCAL 1
#define _OUTPUT_EMPTY 2
#define _OUTPUT_YAF   3
#define _OUTPUT_ARTS  4

#define _OUTPUT_THREADSAFE 1

class KDE_EXPORT OutPlugin {

 public:
  OutPlugin();
  ~OutPlugin();

  static OutputStream* createOutputStream(int outputType);
  static OutputStream* createOutputStream(int outputType,int lThreadSafe);

};
#endif




