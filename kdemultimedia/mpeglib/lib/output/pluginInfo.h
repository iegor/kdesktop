/*
  add on information about plugin.
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __PLUGININFO_H
#define __PLUGININFO_H

#include "../util/dynBuffer.h"
#include <kdemacros.h>

/**
   Here we have the base class for all additional information
   about a plugin.
   The len in time of the current song belongs here, as
   well as the author, version number etc..
*/


class KDE_EXPORT PluginInfo {

  int sec;
  DynBuffer* musicName;

  
 public:
  PluginInfo();
  ~PluginInfo();

  void setLength(int sec);
  int getLength();

  void setUrl(char* name);
  char* getUrl();
  
  void reset();
  void print();
};
#endif
