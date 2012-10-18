/*
  valid runtime commands for generic player
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __RUNTIME_TABLEXPLAYER_H
#define __RUNTIME_TABLEXPLAYER_H

#include "../yafcore/commandTable.h"
#include "xplayerRuntime.defs"





class RuntimeTableXPlayer : public CommandTable {

 public:
  RuntimeTableXPlayer();
  ~RuntimeTableXPlayer();
  void init();

};

#endif





