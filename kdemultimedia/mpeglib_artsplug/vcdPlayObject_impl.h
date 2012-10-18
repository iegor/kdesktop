/*
  class for mp3-mpeglib decoder
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __VCDPLAYOBJECT_IMPL_H
#define __VCDPLAYOBJECT_IMPL_H


#include "decoderBaseObject_impl.h"
#include "../mpeglib/lib/decoder/mpgPlugin.h"



class VCDPlayObject_impl: 
 virtual public DecoderBaseObject_impl,
 virtual public VCDPlayObject_skel {



public:

  VCDPlayObject_impl();
  virtual ~VCDPlayObject_impl();


  DecoderPlugin* createPlugin();

  InputStream* createInputStream(const char* url);

  void calculateBlock(unsigned long samples);

};


#endif
