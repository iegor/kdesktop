/*
  converts ogg frames into audioFrames
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

 

#ifndef __VORBISDECODER_H
#define __VORBISDECODER_H

#include "../frame/audioFrame.h"
#include <string.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef OGG_VORBIS

#include <vorbis/codec.h>
#include "oggFrame.h"

class VorbisDecoder {

  vorbis_info      vi; /* struct that stores all the static vorbis bitstream
                          settings */
  vorbis_comment   vc; /* struct that stores all the bitstream user comments */
  vorbis_dsp_state vd; /* central working state for the packet->PCM decoder */
  vorbis_block     vb; /* local working space for packet->PCM decode */

  int initState;

 public:
  VorbisDecoder();
  ~VorbisDecoder();

  void reset();
  int hasHeader();

  int decode(RawFrame* rawFrame,AudioFrame* dest);
  void config(const char* key,const char* val,void* ret);
};

#endif

#endif
