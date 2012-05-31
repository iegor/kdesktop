/*
  valid Commands for generic cd player
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#define _USE_XPLAYER_STRUC

#include "commandTableXPlayer.h"


CommandTableXPlayer::CommandTableXPlayer(){
  init();
}

CommandTableXPlayer::~CommandTableXPlayer(){
}

void CommandTableXPlayer::init() {


  int i;
  for (i=0;i<XPLAYERCOMMANDS_SIZE;i++) {
    insert(&xplayerCommands[i]);
  }
	   

}

