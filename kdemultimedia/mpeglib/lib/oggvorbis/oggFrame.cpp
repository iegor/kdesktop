/*
  here we have an ogg frame. Its still a raw frame.
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "oggFrame.h"

#ifdef OGG_VORBIS


OGGFrame::OGGFrame() : RawFrame(_FRAME_RAW_OGG,0) {
  setRemoteData((unsigned char*) &op,sizeof(op));

}

OGGFrame::~OGGFrame() {
}


 
#endif
