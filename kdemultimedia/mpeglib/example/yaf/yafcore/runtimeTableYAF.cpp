/*
  basic runtime command (fits for every decoder)
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#define _USE_RUNTIME_YAF_STRUC

#include "runtimeTableYAF.h"


RuntimeTableYAF::RuntimeTableYAF(){
  init();
}

RuntimeTableYAF::~RuntimeTableYAF(){
}

void RuntimeTableYAF::init() {

  int i;
  for (i=0;i<YAFRUNTIME_SIZE;i++) {
    insert(&yafRuntime[i]);
  }

}

