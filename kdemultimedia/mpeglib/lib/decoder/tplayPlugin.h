/*
  tplay player plugin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __TPLAYPLUGIN_H
#define __TPLAYPLUGIN_H

#include "../decoder/decoderPlugin.h"
#include <kdemacros.h>
/**
   The tplayPlugin is ugly and needs a rewrite. 
   Im not sure if you can make mutiple instances of it
*/

class KDE_EXPORT TplayPlugin : public DecoderPlugin {

  struct info_struct* info;
  class TimeStamp* startStamp;
  class TimeStamp* endStamp;

 public:
  TplayPlugin();
  ~TplayPlugin();

  void decoder_loop();
  int seek_impl(int second);
  void config(const char* key,const char* value,void* user_data);

 protected:
  int getTotalLength();

 private:
  void swap_block(char* buffer, int blocksize);
  void read_header();


};
#endif
