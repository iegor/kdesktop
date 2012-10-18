/*
  parses extension data (picture and sequence)
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "mpegExtension.h"

#include <iostream>

using namespace std;

MpegExtension::MpegExtension() {
  userData=NULL;
  extData=NULL;
  extraBit=NULL;
}


MpegExtension::~MpegExtension() {
  if (userData != NULL) {
    delete userData;
  }
  if (extData != NULL) {
    delete extData;
  }
  if (extraBit != NULL) {
    delete extraBit;
  }
}


int MpegExtension::processExtensionData(MpegVideoStream* mpegVideoStream) {

  /* Goto next start code. */
  mpegVideoStream->next_start_code();
 
  /*
   * If next start code is extension/user start code, 
   * parse off extension data.
   */
  
  /*
   * If next start code is extension start code, parse off extension data.
   */
  if (next_bits(32, EXT_START_CODE,mpegVideoStream)) {
    mpegVideoStream->flushBits(32);
    if (extData != NULL) {
      delete extData;
      extData = NULL;
    }
    cout << "ext"<<endl;
    extData = get_ext_data(mpegVideoStream);
  }

  /*
   * If next start code is user start code, parse off user data.
   */
  if (next_bits(32, USER_START_CODE,mpegVideoStream)) {
    mpegVideoStream->flushBits(32);
    if (userData != NULL) {
      delete userData;
      userData=NULL;
    }
    userData =get_ext_data(mpegVideoStream);
  }

  return true;
}



int MpegExtension::processExtra_bit_info(MpegVideoStream* mpegVideoStream) {
  if (extraBit != NULL) {
    delete extraBit;
    extraBit = NULL;
  }
  get_extra_bit_info(mpegVideoStream);
  return true;
}




int MpegExtension::next_bits(int num,unsigned int mask,
			     MpegVideoStream* input) {
  unsigned int data;

  /* Get next num bits, no buffer pointer advance. */

  data=input->showBits(num);

  /* Compare bit stream and mask. Set return value toTRUE if equal, FALSE if
     differs. 
  */

  if (mask == data) {
    return true;
  } 
  return false;
}

/*
 *--------------------------------------------------------------
 *
 * get_ext_data --
 *
 *	Assumes that bit stream is at begining of extension
 *      data. Parses off extension data into dynamically 
 *      allocated space until start code is hit. 
 *
 * Results:
 *	Pointer to dynamically allocated memory containing
 *      extension data.
 *
 * Side effects:
 *	Bit stream irreversibly parsed.
 *
 *--------------------------------------------------------------
 */
char* MpegExtension::get_ext_data(MpegVideoStream* mpegVideoStream) {
  unsigned int size, marker;
  char *dataPtr;
  unsigned int data;

  /* Set initial ext data buffer size. */

  size = EXT_BUF_SIZE;

  /* Allocate ext data buffer. */

  dataPtr = (char *) malloc(size);

  /* Initialize marker to keep place in ext data buffer. */

  marker = 0;

  /* While next data is not start code... */
  while (!next_bits(24, 0x000001,mpegVideoStream )) {

    /* Get next byte of ext data. */

    data=mpegVideoStream->getBits(8);



    /* Put ext data into ext data buffer. Advance marker. */

    dataPtr[marker] = (char) data;
    marker++;

    /* If end of ext data buffer reached, resize data buffer. */

    if (marker == size) {
      size += EXT_BUF_SIZE;
      dataPtr = (char *) realloc(dataPtr, size);
    }
  }

  /* Realloc data buffer to free any extra space. */

  dataPtr = (char *) realloc(dataPtr, marker);
  delete dataPtr;
  dataPtr=NULL;
  /* Return pointer to ext data buffer. */
  return dataPtr;
}

void MpegExtension::processExtBuffer(MpegVideoStream* mpegVideoStream){
  unsigned int size, marker;
  char *dataPtr;
  unsigned int data=1;

  /* Initialize size of extra bit info buffer and allocate. */


  size = EXT_BUF_SIZE;
  dataPtr = (char *) malloc(size);

  /* Reset marker to hold place in buffer. */

  marker = 0;

  /* While flag bit is true. */

  while (data) {

    /* Get next 8 bits of data. */
    data=mpegVideoStream->getBits(8);

    /* Place in extra bit info buffer. */

    dataPtr[marker] = (char) data;
    marker++;

    /* If buffer is full, reallocate. */

    if (marker == size) {
      size += EXT_BUF_SIZE;
      dataPtr = (char *) realloc(dataPtr, size);
    }

    /* Get next flag bit. */
    data=mpegVideoStream->getBits(1);
  }

  /* Reallocate buffer to free extra space. */

  dataPtr = (char *) realloc(dataPtr, marker);
  delete dataPtr;
  dataPtr=NULL;
  /* Return pointer to extra bit info buffer. */
}


/*
 *--------------------------------------------------------------
 *
 * get_extra_bit_info --
 *
 *	Parses off extra bit info stream into dynamically 
 *      allocated memory. Extra bit info is indicated by
 *      a flag bit set to 1, followed by 8 bits of data.
 *      This continues until the flag bit is zero. Assumes
 *      that bit stream set to first flag bit in extra
 *      bit info stream.
 *
 * Results:
 *	Pointer to dynamically allocated memory with extra
 *      bit info in it. Flag bits are NOT included.
 *
 * Side effects:
 *	Bit stream irreversibly parsed.
 *
 *--------------------------------------------------------------
 */

char* MpegExtension::get_extra_bit_info(MpegVideoStream* mpegVideoStream) {
  unsigned int data;

  /* Get first flag bit. */
  data=mpegVideoStream->getBits(1);

  /* If flag is false, return NULL pointer (i.e. no extra bit info). */

  if (!data) {
    return NULL;
  }
  processExtBuffer(mpegVideoStream);
  return NULL;
}
