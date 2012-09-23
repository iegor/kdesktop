/*
  base class for all mpeglib decoders
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#include "nullPlayObject_impl.h"
#include "debug.h"

NULLPlayObject_impl::NULLPlayObject_impl() {
  _state=Arts::posIdle;

}

NULLPlayObject_impl::~NULLPlayObject_impl() {
  arts_debug("~NULLPlayObject_impl -s");
}


bool NULLPlayObject_impl::loadMedia(const string &filename) {
  arts_debug("NULLPlayObject_impl::loadMedia");
  return true;
}



string NULLPlayObject_impl::description() {
  arts_debug("description");
  string back;
  return back;
}

void NULLPlayObject_impl::description(const string &) {
  arts_debug("description");
  // what should do this?
}

Arts::poTime NULLPlayObject_impl::currentTime() {
  poTime time;
  return time;
}



poTime NULLPlayObject_impl::overallTime() {
  poTime time;
  return time;
}

poCapabilities NULLPlayObject_impl::capabilities() {
  arts_debug("capabilities");
  return Arts::capPause;  /* no seek supported */
}

string NULLPlayObject_impl::mediaName() {
  arts_debug("mediaName");
  string back;
  // whats a mediaName?
  return back;
}

poState NULLPlayObject_impl::state() {
  return _state;
}

void NULLPlayObject_impl::play() {
  arts_debug("NULLPlayObject_impl::play");
  _state = Arts::posPlaying;
}

void NULLPlayObject_impl::seek(const class poTime& seekTime) {
  long sec=seekTime.seconds;

  arts_debug("NULLPlayObject_impl::sec is %d:", sec);

}

void NULLPlayObject_impl::pause() {
  arts_debug("NULLPlayObject_impl::pause");
  _state = Arts::posPaused;
}

void NULLPlayObject_impl::halt() {
  arts_debug("NULLPlayObject_impl::halt");
  _state=Arts::posIdle;
}
 

void NULLPlayObject_impl::streamInit() {
  arts_debug(" NULLPlayObject_impl::streamInit");
}


void NULLPlayObject_impl::streamStart() {
  arts_debug(" NULLPlayObject_impl::streamStart");
}

void NULLPlayObject_impl::calculateBlock(unsigned long samples,
					    float* left , float* right) {


  arts_debug(" NULLPlayObject_impl::calculateBlock");
  
  unsigned int i;
  for(i=0;i<samples;i++)
    left[i] = right[i] = 0.0;
}

void NULLPlayObject_impl::streamEnd() {
  arts_debug("NULLPlayObject_impl::streamEnd");
}


REGISTER_IMPLEMENTATION(NULLPlayObject_impl);
