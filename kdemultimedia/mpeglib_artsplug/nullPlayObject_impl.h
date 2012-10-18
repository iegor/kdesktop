/*
  this playobkect does nothing
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __NULLPLAYOBJECT_IMPL_H
#define __NULLPLAYOBJECT_IMPL_H



#include <math.h>
#include "decoderBaseObject.h"
#include "stdsynthmodule.h"
#include "convert.h"
#include <X11/Xlib.h>
#include <audiosubsys.h>
#include <iostream>
#include <stdio.h>
#include <string.h>


using namespace std;
using Arts::poState;
using Arts::poTime;
using Arts::poCapabilities;


class NULLPlayObject_impl :
  virtual public Arts::StdSynthModule,
  virtual public NULLPlayObject_skel {

  poState _state;

public:

  NULLPlayObject_impl();
  virtual ~NULLPlayObject_impl();


  bool loadMedia(const string &filename);
  string description();
  void description(const string &);

  poTime currentTime();
  poTime overallTime();

  poCapabilities capabilities();
  string mediaName();
  poState state();
  void play();
  void halt();

  void blocking(bool newvalue);
  bool blocking();

  void seek(const class poTime &);
  void pause();
  void streamInit();
  void streamStart();
  void calculateBlock(unsigned long samples,float* left,float* right);
  void streamEnd();

};



#endif
