/*
  class for mp3-mpeglib decoder
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#include "wavPlayObject_impl.h"
#include "debug.h"
#include <iostream>


WAVPlayObject_impl::WAVPlayObject_impl() {
  arts_debug("WAVPlayObject_impl");
}

WAVPlayObject_impl::~WAVPlayObject_impl() {
  arts_debug("~WAVPlayObject_impl");
}


DecoderPlugin* WAVPlayObject_impl::createPlugin() {
    std::cout << "WAVPlayObject_impl::getPlugin"<<std::endl;
  return new TplayPlugin();
}

void WAVPlayObject_impl::calculateBlock(unsigned long samples) {
  DecoderBaseObject_impl::calculateBlock(samples,left,right);
}



REGISTER_IMPLEMENTATION(WAVPlayObject_impl);

