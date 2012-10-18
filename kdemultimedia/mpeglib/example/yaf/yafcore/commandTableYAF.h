/*
  valid Commands for yaf (basic yaf commands)
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __COMMAND_YAF_H
#define __COMMAND_YAF_H

#include "commandTable.h"
#include "yafCommand.defs"





class CommandTableYAF : public CommandTable {

 public:
  CommandTableYAF();
  ~CommandTableYAF();
  void init();

};

#endif

