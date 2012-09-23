/*
  class for mp3-mpeglib decoder
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __CDDAPLAYOBJECT_IMPL_H
#define __CDDAPLAYOBJECT_IMPL_H


#include "decoderBaseObject_impl.h"
#include "../mpeglib/lib/decoder/cddaPlugin.h"


class CDDAPlayObject_impl: 
 virtual public DecoderBaseObject_impl,
 virtual public CDDAPlayObject_skel {



public:

  CDDAPlayObject_impl();
  virtual ~CDDAPlayObject_impl();


  DecoderPlugin* createPlugin();

  InputStream* createInputStream(const char* url);

  void calculateBlock(unsigned long samples);

};


#endif
