/*
  class for mp3-mpeglib decoder
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#include "mp3PlayObject_impl.h"
#include "debug.h"



MP3PlayObject_impl::MP3PlayObject_impl() {
  arts_debug("MP3PlayObject_impl");
}

MP3PlayObject_impl::~MP3PlayObject_impl() {
  arts_debug("~MP3PlayObject_impl");
}


DecoderPlugin* MP3PlayObject_impl::createPlugin() {
  arts_debug("MP3PlayObject_impl::getPlugin");
  return new SplayPlugin();
}

bool MP3PlayObject_impl::doFloat()
{
  return true;
}

void MP3PlayObject_impl::calculateBlock(unsigned long samples) {
  DecoderBaseObject_impl::calculateBlock(samples,left,right);
}



REGISTER_IMPLEMENTATION(MP3PlayObject_impl);


