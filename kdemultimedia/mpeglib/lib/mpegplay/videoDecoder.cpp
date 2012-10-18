/*
  mpeg I video decoder (derived from mpeg_play)
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "videoDecoder.h"

using namespace std;



VideoDecoder::VideoDecoder(MpegVideoStream* inputStream,
			   MpegVideoHeader* initSequence) {

  /* Check for legal buffer length. */

  init_tables();


  /* Initialize fields that used to be global */

  mpegVideoStream=inputStream;
  decoderClass=new DecoderClass(this,mpegVideoStream);
  recon=new Recon();
  motionVector=new MotionVector();
  slice=new Slice();
  group=new GOP();
  mpegVideoHeader=new MpegVideoHeader();
  picture=new Picture();
  macroBlock=new MacroBlock(this);

  // init this stream with the init sequence
  initSequence->copyTo(mpegVideoHeader);

  syncState=SYNC_TO_CLOSED_GOP;
  extension=new MpegExtension();
  frameCounter=0;
}


VideoDecoder::~VideoDecoder() {

  delete mpegVideoHeader;
  delete picture;
  delete decoderClass;
  delete recon;
  delete motionVector;
  delete slice;
  delete group;
  delete extension;
  delete macroBlock;
}





/*
 *--------------------------------------------------------------
 *
 * mpegVidRsrc --
 *
 *      Parses bit stream until MB_QUANTUM number of
 *      macroblocks have been decoded or current slice or
 *      picture ends, whichever comes first. If the start
 *      of a frame is encountered, the frame is time stamped
 *      with the value passed in time_stamp. If the value
 *      passed in buffer is not null, the video stream buffer
 *      is set to buffer and the length of the buffer is
 *      expected in value passed in through length. The current
 *      video stream is set to vid_stream. If vid_stream
 *      is passed as NULL, a new VideoDecoder structure is created
 *      and initialized and used as the current video stream.
 *
 * Results:
 *      A pointer to the video stream structure used.
 *
 * Side effects:
 *      Bit stream is irreversibly parsed. If a picture is completed,
 *      a function is called to display the frame at the correct time.
 *
 *--------------------------------------------------------------
 */

int VideoDecoder::mpegVidRsrc(PictureArray* pictureArray) {
  int back=_SYNC_TO_NONE;

  unsigned int data;
  int i;


  /*
   * If called for the first time, find start code, make sure it is a
   * sequence start code.
   */


  /* Get next 32 bits (size of start codes). */

  data=mpegVideoStream->showBits(32);


  /*
   * Process according to start code (or parse macroblock if not a start code
   * at all).
   */
  switch (data) {
    /*
  case PACK_START_CODE:
  case SYSTEM_HEADER_START_CODE:  
    cout << "Packet in Loop **************"<<endl;
    mpegVideoStream->flushBits(32);
    packet->read_sys(data,vid_stream->bufferReader);
    */
  case SEQ_END_CODE:
  case ISO_11172_END_CODE:   /*  handle ISO_11172_END_CODE too */

    /* Display last frame. */

    // removed!

    /* Sequence done. Do the right thing. For right now, exit. */


    cout << "******** flushin end code"<<endl;
    mpegVideoStream->flushBits(32);
    goto done;
    break;
  case EXT_START_CODE:
    cout << "found EXT_START_CODE skipping"<<endl;
    mpegVideoStream->flushBits(32);
    /* Goto next start code. */
    mpegVideoStream->next_start_code();
 
    break;
  case USER_START_CODE:
    mpegVideoStream->flushBits(32);
    /* Goto next start code. */
    mpegVideoStream->next_start_code();

    break;
  case SEQ_START_CODE:
    
    /* Sequence start code. Parse sequence header. */
    if (ParseSeqHead() == false) {
      printf("SEQ_START_CODE 1-error\n");
      goto error;
    }
    goto done;

  case GOP_START_CODE:
    /* Group of Pictures start code. Parse gop header. */
    if (ParseGOP() == false) {
      printf("GOP_START_CODE 1-error\n");
      goto error;
    }
    goto done;

  case PICTURE_START_CODE:

    /* Picture start code. Parse picture header and first slice header. */

    
    back=ParsePicture();
    if (back != _SYNC_TO_NONE) {
      //cout << "skip B Frame we are late"<<endl;
      return back;
    }

    // parse ok
    if (ParseSlice() == false) {
      printf("PICTURE_START_CODE 2-error\n");
      goto error;
    }
    break;

  case SEQUENCE_ERROR_CODE:
    mpegVideoStream->flushBits(32);
    mpegVideoStream->next_start_code();

    goto done;

    
  default:

    /* Check for slice start code. */
    if ((data >= SLICE_MIN_START_CODE) && (data <= SLICE_MAX_START_CODE)) {

      /* Slice start code. Parse slice header. */
      if (ParseSlice() == false) {
	printf("default 1-error\n");
        goto error;
      }
    }
    break;
  }

  /* Parse next MB_QUANTUM macroblocks. */
  for (i = 0; i < MB_QUANTUM; i++) {

    /* Check to see if actually a startcode and not a macroblock. */
    data=mpegVideoStream->showBits(23);
    if (data != 0x0) {
      /* Not start code. Parse Macroblock. fill yuv pictures*/
      if (macroBlock->processMacroBlock(pictureArray) == false) {
        goto error;
      }
    } else {
      /* Not macroblock, actually start code. Get start code. */
      mpegVideoStream->next_start_code();

      /*
       * If start code is outside range of slice start codes, frame is
       * complete, display frame.
       */
      data=mpegVideoStream->showBits(32);

      if (((data < SLICE_MIN_START_CODE) || (data > SLICE_MAX_START_CODE)) &&
	  (data != SEQUENCE_ERROR_CODE)) {
	doPictureDisplay(pictureArray);
      }
      goto done;
    }
  }
  data=mpegVideoStream->showBits(23);
  /* Check if we just finished a picture on the MB_QUANTUM macroblock */
  if (data == 0x0) {
    mpegVideoStream->next_start_code();

    data=mpegVideoStream->showBits(32);
    if ((data < SLICE_MIN_START_CODE) || (data > SLICE_MAX_START_CODE)) {
      doPictureDisplay(pictureArray);
    }
  }

  /* Return pointer to video stream structure. */

  goto done;

error:
  init_tables();
  back=_SYNC_TO_GOP;
  mpegVideoHeader->init_quanttables();

  goto done;

done:
  return back;

}






int VideoDecoder::ParseSeqHead() {
  int back;

  /* Flush off sequence start code. */

  mpegVideoStream->flushBits(32);

  back=mpegVideoHeader->parseSeq(mpegVideoStream);

  return back;

}




int VideoDecoder::ParseGOP() {
  if (syncState==SYNC_TO_CLOSED_GOP) {
    syncState=SYNC_HAS_CLOSED_GOP;
  }

  return group->processGOP(mpegVideoStream);
}




int VideoDecoder::ParsePicture() {
  int back;
  back=picture->processPicture(mpegVideoStream);

  macroBlock->resetPastMacroBlock();
  if (back == false) {
    return _SYNC_TO_GOP;
  }


  return _SYNC_TO_NONE;
}



int VideoDecoder::ParseSlice() {

  
  slice->parseSlice(mpegVideoStream);
  macroBlock->resetMacroBlock();
  decoderClass->resetDCT();
  return true;
}




/**
   After a seek we can only start with an I frame
*/

void VideoDecoder::resyncToI_Frame() {

  syncState=SYNC_TO_CLOSED_GOP;
}


void VideoDecoder::doPictureDisplay(PictureArray* pictureArray) {


  // insert end timestamp to current picture
  YUVPicture* pic=pictureArray->getCurrent();
  unsigned int code_type=picture->getCodeType();

  TimeStamp* startTimeStamp=picture->getStartOfPicStamp();

  pic->setStartTimeStamp(startTimeStamp);
  float rate=mpegVideoHeader->getPictureRate();

  pictureArray->setPicturePerSecond(rate);



  pic->setMpegPictureType(code_type);

  if (syncState < SYNC_HAS_CLOSED_GOP) {
    return;
  }
  if (syncState < SYNC_HAS_I_FRAME_SYNC) {
    if (code_type != I_TYPE) {
      return;
    }
  }
  if (code_type == I_TYPE) {
    YUVPicture* past=pictureArray->getPast();
    YUVPicture* future=pictureArray->getFuture();
    YUVPicture* current=pictureArray->getCurrent();

    YUVPicture* tmp=past;
    past = future;
    future = current;
    current = tmp;
    
    pic=past;

  
    pictureArray->setPast(past);
    pictureArray->setCurrent(current);
    pictureArray->setFuture(future);

    if (syncState < SYNC_HAS_I_FRAME_SYNC) {
      syncState=SYNC_HAS_I_FRAME_SYNC;
      return;
    }
    if (syncState == SYNC_HAS_P_FRAME_SYNC) {
      syncState=SYNC_HAS_FRAME_SYNC;
      return;
    }     
    if (syncState == SYNC_HAS_I_FRAME_SYNC) {
      syncState=SYNC_HAS_P_FRAME_SYNC;
      return;
    }    


  
  }
  
  if (code_type == P_TYPE) {
    YUVPicture* past=pictureArray->getPast();
    YUVPicture* future=pictureArray->getFuture();
    YUVPicture* current=pictureArray->getCurrent();

    YUVPicture* tmp=past;
    past = future;
    future = current;
    current = tmp;
       
    pic = past;


   
    pictureArray->setPast(past);
    pictureArray->setCurrent(current);
    pictureArray->setFuture(future);

    if (syncState < SYNC_HAS_P_FRAME_SYNC) {
      syncState=SYNC_HAS_P_FRAME_SYNC;
      return;
    }

  }  
  if (code_type == B_TYPE) {
    if (syncState == SYNC_HAS_P_FRAME_SYNC) {
      syncState=SYNC_HAS_FRAME_SYNC;
      YUVPicture* past=pictureArray->getPast();
      pic=past;
    }   
    /**
       Now check for PTS timeStamp error. It seems some encoders
       handles this different.
       If the P frame has a timeStamp earlier than our B stamp
       we swap them.
    */
    YUVPicture* pframe=pictureArray->getFuture();
    YUVPicture* bframe=pictureArray->getCurrent();

    TimeStamp* pTime=pframe->getStartTimeStamp();
    TimeStamp* bTime=bframe->getStartTimeStamp();

    double pPTS=pTime->getPTSTimeStamp();
    double bPTS=bTime->getPTSTimeStamp();
    if (pPTS < bPTS) {
      //cout << "********P/B Frame PTS error -> enable swap and pray"<<endl;
      bTime->copyTo(pTime);
    }
   


    // we display the current picture
    // (already set)
  }

  
  if (pic == NULL) {
    cout << "pic NULL"<<endl;
    exit(0);
    return;
  }
  if (syncState < SYNC_HAS_FRAME_SYNC) {
    return;
  }

  double val=pictureArray->getPicturePerSecond();
  pic->setPicturePerSecond(val);

  TimeStamp* currentStamp=pic->getStartTimeStamp();


  frameCounter++;

  if (currentStamp->getPTSFlag()==true) {
    frameCounter=0;
  }
  currentStamp->setVideoFrameCounter(frameCounter);


  // let plugin "rip" the picture
  pictureArray->setYUVPictureCallback(pic);
}





