/*
  frames raw data into Ogg/Vorbis frames.
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __OVFRAMER_H
#define __OVFRAMER_H


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef OGG_VORBIS

#include <vorbis/codec.h>
#include "../frame/framer.h"
#include "oggFrame.h"

/**

   This framer works directly on the raw ogg_packet as output
   Note: the internal setup makes sure, that we initialize
   the ogg stream to the first found logical bitstream.
   (For now this should mean: we found vorbis)
   When the frame goes into the "HAS" state, you have the ogg
   packet in the in the dest struct from the constructor.
*/

class OVFramer : public Framer {

  int vorbis_state;

  ogg_sync_state   oy; /* sync and verify incoming physical bitstream */
  ogg_stream_state os; /* take physical pages, weld into a logical
                          stream of packets */
  ogg_page         og; /* one Ogg bitstream page.  Vorbis packets are inside */

  char *buffer;        /* sync buffer from ogg */

  OGGFrame*      dest; /* one raw packet of data for decode */

 public:
  // IMPORTANT: because we use this ptr internally the 
  //            data to op cannot be on the stack!
  OVFramer(OGGFrame* dest);
  ~OVFramer();


 private:

  int find_frame(RawDataBuffer* input,RawDataBuffer* store);
  int read_frame(RawDataBuffer* input,RawDataBuffer* store);

  void unsync(RawDataBuffer* store,int lReset);
  void printPrivateStates();

};
#endif

#endif
