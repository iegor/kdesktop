/*
  converts raw mpeg audio stream data into mpeg I encoded audio frames/packets
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __MPEGAUDIOFRAME_H
#define __MPEGAUDIOFRAME_H

#include "mpegAudioHeader.h"
#include "../frame/framer.h"
#include <kdemacros.h>

/*
  Here we are framing from raw to mpeg audio.
*/




class KDE_EXPORT MpegAudioFrame : public Framer {

  // max size of buffer is:
  // header:          4
  // max bitrate:   448
  // min freq:    22050
  // padding:         1
  // ------------------
  // maxsize:    4+144000*max(bitrate)/min(freq)+1 ca: 2931 byte
  // then we add a "sentinel" at the end these are 4 byte.
  // so we should be ok, with a 4KB buffer.

  // internal, how much data we need to read
  int framesize;

  // internall, for header searching
  int find_frame_state;

  // internal use for header parsing+validating
  MpegAudioHeader* mpegAudioHeader;

 public:
  MpegAudioFrame();
  ~MpegAudioFrame();


 private:

  int find_frame(RawDataBuffer* input,RawDataBuffer* store);
  int read_frame(RawDataBuffer* input,RawDataBuffer* store);

  void unsync(RawDataBuffer* store,int lReset);
  void printPrivateStates();

};


#endif
