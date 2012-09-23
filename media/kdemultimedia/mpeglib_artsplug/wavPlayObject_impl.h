/*
  class for wav-mpeglib decoder
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __WAVPLAYOBJECT_IMPL_H
#define __WAVPLAYOBJECT_IMPL_H


#include "decoderBaseObject_impl.h"
#include "../mpeglib/lib/decoder/tplayPlugin.h"



class WAVPlayObject_impl: 
 virtual public DecoderBaseObject_impl,
 virtual public WAVPlayObject_skel {



public:

  WAVPlayObject_impl();
  virtual ~WAVPlayObject_impl();


  DecoderPlugin* createPlugin();



  void calculateBlock(unsigned long samples);

};


#endif
