/*
  this plugin nukes the input data. 
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __NUKEPLUGIN_H
#define __NUKEPLUGIN_H


#include "../decoder/decoderPlugin.h"
#include <kdemacros.h>

class KDE_EXPORT NukePlugin : public DecoderPlugin {

 public:

  NukePlugin();
  ~NukePlugin();

  void decoder_loop();
  void config(const char* key,const char* value,void* user_data);

 private:

};
#endif
