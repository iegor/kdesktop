/*
  here we have an ogg frame. Its still a raw frame.
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __OGG_FRAME_H
#define __OGG_FRAME_H


/**
   One Ogg Paket as frame. We pass the datapointer and the size
   to the rawFrame and of course, set the PaktedID to:
   _FRAME_RAW_OGG
*/

#include "../frame/rawFrame.h"


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef OGG_VORBIS

#include <vorbis/codec.h>

class OGGFrame : public RawFrame {

  ogg_packet op;

 public:
  OGGFrame();
  ~OGGFrame();


};

#endif

#endif

