/*
  class for mp3-mpeglib decoder
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __MP3PLAYOBJECT_IMPL_H
#define __MP3PLAYOBJECT_IMPL_H


#include "decoderBaseObject_impl.h"
#include "../mpeglib/lib/decoder/splayPlugin.h"


class MP3PlayObject_impl: 
 virtual public DecoderBaseObject_impl,
 virtual public MP3PlayObject_skel {



public:

  MP3PlayObject_impl();
  virtual ~MP3PlayObject_impl();

  DecoderPlugin* createPlugin();
  bool doFloat();



  void calculateBlock(unsigned long samples);

};


#endif
