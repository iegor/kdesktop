/*
  class for ogg-mpeglib decoder
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#include "oggPlayObject_impl.h"
#include "debug.h"




OGGPlayObject_impl::OGGPlayObject_impl() {
  arts_debug("OGGPlayObject_impl");
}

OGGPlayObject_impl::~OGGPlayObject_impl() {
  arts_debug("~OGGPlayObject_impl");
}


DecoderPlugin* OGGPlayObject_impl::createPlugin() {
  arts_debug("OGGPlayObject_impl::getPlugin");
  return new VorbisPlugin();
}

void OGGPlayObject_impl::calculateBlock(unsigned long samples) {
  DecoderBaseObject_impl::calculateBlock(samples,left,right);
}



REGISTER_IMPLEMENTATION(OGGPlayObject_impl);

