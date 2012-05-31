/*
  class for mp3-mpeglib decoder
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#include "vcdPlayObject_impl.h"
#include "debug.h"
#include <iostream>


VCDPlayObject_impl::VCDPlayObject_impl() {
  arts_debug("VCDPlayObject_impl");
}

VCDPlayObject_impl::~VCDPlayObject_impl() {
  arts_debug("~VCDPlayObject_impl");
}


DecoderPlugin* VCDPlayObject_impl::createPlugin() {
    std::cout << "VCDPlayObject_impl::getPlugin"<<std::endl;
  return new MpgPlugin();
}

InputStream* VCDPlayObject_impl::createInputStream(const char* ) {
  cout << "VCDPlayObject_impl::createInputStream"<<endl;
  InputStream* back;
  back=InputPlugin::createInputStream(__INPUT_CDI,_INPUT_THREADSAFE);
  return back;
}

void VCDPlayObject_impl::calculateBlock(unsigned long samples) {
  DecoderBaseObject_impl::calculateBlock(samples,left,right);
}



REGISTER_IMPLEMENTATION(VCDPlayObject_impl);


