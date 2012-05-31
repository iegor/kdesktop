/*
  reads/parse slice infos
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "slice.h"




Slice::Slice() {
  mpegExtension=new MpegExtension();
}


Slice::~Slice() {
  delete mpegExtension;

}

unsigned int Slice::getVertPos() {
  return vert_pos;
}



void Slice::setQuantScale(unsigned int quant_scale) {
  this->quant_scale=quant_scale;
}

/*
 *--------------------------------------------------------------
 *
 * ParseSlice --
 *
 *      Parses off slice header.
 *
 * Results:
 *      Values found in slice header put into video stream structure.
 *
 * Side effects:
 *      Bit stream irreversibly parsed.
 *
 *--------------------------------------------------------------
 */
int Slice::parseSlice(MpegVideoStream* mpegVideoStream) {
  
  /* Flush slice start code. */
  mpegVideoStream->flushBits(24);

  /* Parse off slice vertical position. */
  /* its the "slice number" */
  vert_pos=mpegVideoStream->getBits(8);

  /* Parse off quantization scale. */
  quant_scale=mpegVideoStream->getBits(5);

  /* Parse off extra bit slice info. */
  mpegExtension->processExtra_bit_info(mpegVideoStream);

  return true;
}

  

