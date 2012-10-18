/*
  valid Commands for generic cd player
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __COMMAND_TABLEXPLAYER_H
#define __COMMAND_TABLEXPLAYER_H

#include "../yafcore/commandTable.h"
#include "xplayerCommand.defs"





class CommandTableXPlayer : public CommandTable {

 public:
  CommandTableXPlayer();
  ~CommandTableXPlayer();
  void init();

};

#endif

