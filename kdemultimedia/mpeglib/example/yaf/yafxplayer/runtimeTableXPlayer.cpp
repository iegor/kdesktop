/*
  valid runtime commands for generic player
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#define _USE_RUNTIME_XPLAYER_STRUC

#include "runtimeTableXPlayer.h"


RuntimeTableXPlayer::RuntimeTableXPlayer(){
  init();
}

RuntimeTableXPlayer::~RuntimeTableXPlayer(){
}

void RuntimeTableXPlayer::init() {


  int i;
  for (i=0;i<XPLAYERRUNTIME_SIZE;i++) {
    insert(&xplayerRuntime[i]);
  }
	   

}


