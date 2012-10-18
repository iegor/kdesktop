/*
  splay player plugin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __CDDAPLUGIN_H
#define __CDDAPLUGIN_H

#include "nukePlugin.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef CDDA_PARANOIA
class CDDAPlugin : public NukePlugin {
};
#else



class CDDAPlugin : public DecoderPlugin {
  
 public:
  CDDAPlugin();
  ~CDDAPlugin();

  void decoder_loop();
  int seek_impl(int second);
  void config(const char* key,const char* value,void* user_data);
 
 protected: 
  int getTotalLength();


};

#endif
//CDDA_PARANOIA

#endif
