/*
  mpeg I video decoder (derived from mpeg_play)
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __VIDEO_H
#define __VIDEO_H


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "decoderClass.h"
#include "recon.h"
#include "motionVector.h"
#include "slice.h"
#include "proto.h"
#include "../input/inputStream.h"
#include "../output/outputStream.h"
#include "../util/timeStamp.h"
#include "mpegVideoHeader.h"
#include "gop.h"
#include "picture.h"
#include "macroBlock.h"
#include "startCodes.h"



/* Define Parsing error codes. */



#define SYNC_TO_CLOSED_GOP            1
#define SYNC_HAS_CLOSED_GOP           2
#define SYNC_HAS_I_FRAME_SYNC         3
#define SYNC_HAS_P_FRAME_SYNC         4
#define SYNC_HAS_FRAME_SYNC           5

#define _SYNC_TO_GOP                  1
#define _SYNC_TO_PIC                  2
#define _SYNC_TO_NONE                 3



/* Number of macroblocks to process in one call to mpegVidRsrc. */

#define MB_QUANTUM 100


/* Video stream structure. */
class VideoDecoder {


 public:
  VideoDecoder(MpegVideoStream* mpegVideoStream,
	       MpegVideoHeader* initSequence);

  ~VideoDecoder();
  int mpegVidRsrc(PictureArray* pictureArray);


  // ugly callbacks FIX ME!
  class MpegVideoStream* mpegVideoStream;
  class DecoderClass* decoderClass;
  class Recon* recon;
  class MotionVector* motionVector;
  class Slice* slice;                         /* Current slice.            */
  class MpegVideoHeader* mpegVideoHeader;     /* Sequence info in stream   */
  class GOP* group;
  class Picture* picture;                     /* Current picture.          */
  class MacroBlock* macroBlock;               /* Current macroblock.       */
  
  void resyncToI_Frame();



 private:

  int syncState;
 
  int ParseSeqHead();
  int ParseGOP();
  int ParsePicture();
  int ParseSlice();
  void doPictureDisplay(PictureArray* pictureArray);
  MpegExtension* extension;
  int frameCounter;

};   



/* Declaration of global display pointer. */



extern int qualityFlag;




extern int gXErrorFlag;




#endif /* videoDecoder.h already included */


