/*
  mpeg video picture
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "picture.h"


Picture::Picture() {

  /* Initialize pointers to extension and user data. */
  extraBit=0;
  startOfPicStamp=new TimeStamp();
  extension=new MpegExtension();
}



Picture::~Picture() {
  delete startOfPicStamp;
  delete extension;
}


/*
 *--------------------------------------------------------------
 *
 * ParsePicture --
 *
 *      Parses picture header. Marks picture to be presented
 *      at particular time given a time stamp.
 *
 * Results:
 *      Values from picture header put into video stream structure.
 *
 * Side effects:
 *      Bit stream irreversibly parsed.
 *
 *--------------------------------------------------------------
 */
int Picture::processPicture(MpegVideoStream* mpegVideoStream) {
  TimeStamp* stamp;

  /* Flush header start code. */
  mpegVideoStream->flushBits(32);

  /* Parse off temporal reference. */
  temp_ref=mpegVideoStream->getBits(10);

  /* Parse of picture type. */
  code_type=mpegVideoStream->getBits(3);


  // get timestamp from stamp queue
  stamp=mpegVideoStream->getCurrentTimeStamp();

  stamp->copyTo(startOfPicStamp);
  // now invalidate the PTSFlag
  stamp->setPTSFlag(false);


  /* Parse off vbv buffer delay value. */
  vbv_delay=mpegVideoStream->getBits(16);


  /* If P or B type frame... */

  if ((code_type == P_TYPE) || (code_type == B_TYPE)) {

    /* Parse off forward vector full pixel flag. sets it to true/false*/
    full_pel_forw_vector=mpegVideoStream->getBits(1);


    /* Parse of forw_r_code. */
    /* Decode forw_r_code into forw_r_size and forw_f. */
    forw_r_size=mpegVideoStream->getBits(3);
    forw_r_size--;


    forw_f = (1 << forw_r_size);
  }
  /* If B type frame... */
  
  if (code_type == B_TYPE) {
    
    /* Parse off back vector full pixel flag. */
    full_pel_back_vector=mpegVideoStream->getBits(1);
    

    /* Parse off back_r_code. */
    /* Decode back_r_code into back_r_size and back_f. */
    back_r_size=mpegVideoStream->getBits(3);
    back_r_size--;


    back_f = (1 << back_r_size);
  }
  /* Get extra bit picture info. */

  
  /*
  extraBit=mpegVideoStream->getBits(1);
  if (extraBit) {
    cout << "extraBit"<<endl;
    exit(0);
    extension->processExtBuffer(mpegVideoStream);
  }
  */
  extension->processExtra_bit_info(mpegVideoStream);
  extension->processExtensionData(mpegVideoStream);


 
 

  return true;
}

int Picture::processPictureCodingExtension(MpegVideoStream* ) {
  return true;
}

unsigned int Picture::geth_back_r(MpegVideoStream* mpegVideoStream) {
  return mpegVideoStream->getBits(back_r_size);
}


unsigned int Picture::getv_back_r(MpegVideoStream* mpegVideoStream) {
  return mpegVideoStream->getBits(back_r_size);
}
 

unsigned int Picture::geth_forw_r(MpegVideoStream* mpegVideoStream) {
  return mpegVideoStream->getBits(forw_r_size);
}


unsigned int Picture::getv_forw_r(MpegVideoStream* mpegVideoStream) {
  return mpegVideoStream->getBits(forw_r_size);
}
 
