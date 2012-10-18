/*
  class for mp3-mpeglib decoder
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#include "cddaPlayObject_impl.h"
#include "debug.h"
#include <iostream>



CDDAPlayObject_impl::CDDAPlayObject_impl() {
  arts_debug("CDDAPlayObject_impl");
}

CDDAPlayObject_impl::~CDDAPlayObject_impl() {
  arts_debug("~CDDAPlayObject_impl");
}


DecoderPlugin* CDDAPlayObject_impl::createPlugin() {
    std::cout << "CDDAPlayObject_impl::getPlugin"<<std::endl;
  return new CDDAPlugin();
}

InputStream* CDDAPlayObject_impl::createInputStream(const char* ) {
  cout << "CDDAPlayObject_impl::createInputStream"<<endl;
  InputStream* back;
  back=InputPlugin::createInputStream(__INPUT_CDDA,_INPUT_THREADSAFE);
  return back;
}

void CDDAPlayObject_impl::calculateBlock(unsigned long samples) {
  DecoderBaseObject_impl::calculateBlock(samples,left,right);
}



REGISTER_IMPLEMENTATION(CDDAPlayObject_impl);


