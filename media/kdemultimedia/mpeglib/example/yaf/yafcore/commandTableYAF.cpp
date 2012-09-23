/*
  valid Commands for yaf (basic yaf commands)
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#define _USE_YAF_STRUC

#include "commandTableYAF.h"


CommandTableYAF::CommandTableYAF(){
  init();
}

CommandTableYAF::~CommandTableYAF(){
}

void CommandTableYAF::init() {


  int i;
  for (i=0;i<YAFCOMMANDS_SIZE;i++) {
    insert(&yafCommands[i]);
  }
	   

}

