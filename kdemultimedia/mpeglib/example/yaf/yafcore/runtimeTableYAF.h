/*
  basic runtime command (fits for every decoder)
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __RUNTIME_TABLEYAF_H
#define __RUNTIME_TABLEYAF_H

#include "commandTable.h"
#include "yafRuntime.defs"




class RuntimeTableYAF : public CommandTable {

 public:
  RuntimeTableYAF();
  ~RuntimeTableYAF();
  void init();

};

#endif


