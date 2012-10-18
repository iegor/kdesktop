/*
  parse/stores GOP (group of picture) information from a mpegVideoStream
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "gop.h"

#include <iostream>

using namespace std;

GOP::GOP() {
  drop_flag=false;
  tc_hours=0;
  tc_minutes=0;
  tc_seconds=0;
  tc_pictures=0;
  closed_gop=false;
  broken_link=false;
 
  mpegExtension=new MpegExtension();

}


GOP::~GOP() {
  delete mpegExtension;
}


/*
 *--------------------------------------------------------------
 *
 * ParseGOP --
 *
 *      Parses of group of pictures header from bit stream
 *      associated with vid_stream.
 *
 * Results:
 *      Values in gop header placed into video stream structure.
 *
 * Side effects:
 *      Bit stream irreversibly parsed.
 *
 *--------------------------------------------------------------
 */
int GOP::processGOP(MpegVideoStream* mpegVideoStream) {
  unsigned int data;

  /* Flush group of pictures start code. */

  mpegVideoStream->flushBits(32);

  /* Parse off drop frame flag. */

  data=mpegVideoStream->getBits(1);

  if (data) {
    drop_flag = true;
  } else
    drop_flag = false;

  /* Parse off hour component of time code. */

  tc_hours=mpegVideoStream->getBits(5);

  /* Parse off minute component of time code. */

  tc_minutes=mpegVideoStream->getBits(6);


  /* Flush marker bit. */

  mpegVideoStream->flushBits(1);

  /* Parse off second component of time code. */

  tc_seconds=mpegVideoStream->getBits(6);
  
  /* Parse off picture count component of time code. */

  tc_pictures=mpegVideoStream->getBits(6);

  /* Parse off closed gop and broken link flags. */
  data=mpegVideoStream->getBits(2);
  if (data > 1) {
    closed_gop = true;
    if (data > 2) {
      broken_link = true;
    } else
      broken_link = false;
  } else {
    closed_gop = false;
    if (data) {
      broken_link = true;
    } else
      broken_link = false;
  }


  /*
   * If next start code is extension/user start code, 
   * parse off extension data.
   */

  mpegExtension->processExtensionData(mpegVideoStream);

  return true; 
}

  
  
int GOP::substract(GOP* minus,GOP* dest) {
  int hours;
  int minutes;
  int seconds;
  hours=getHour()-minus->getHour();
  minutes=getMinutes()-minus->getMinutes();
  seconds=getSeconds()-minus->getSeconds();

  if (seconds < 0) {
    seconds+=60;
    minutes--;
  }
  if (minutes < 0) {
    minutes+=60;
    hours--;
  }
  dest->tc_hours=hours;
  dest->tc_minutes=minutes;
  dest->tc_seconds=seconds;
  if (hours < 0) {
    return false;
  }
  return true;
}


void GOP::copyTo(GOP* dest) {
  dest->tc_hours=getHour();
  dest->tc_minutes=getMinutes();
  dest->tc_seconds=getSeconds();
  dest->drop_flag=getDropFlag();
  dest->tc_pictures=getPictures();
  dest->closed_gop=getClosedGOP();
  dest->broken_link=getBrokenLink();

  // currently do not copy ext/user data FIX ME
}


void GOP::print(const char* description) {
  cout << "GOP [START]:"<<description<<endl;
  cout << "tc_hours:"<<getHour()<<endl;
  cout << "tc_minutes:"<<getMinutes()<<endl;
  cout << "tc_seconds:"<<getSeconds()<<endl;
  cout << "drop_flag:"<<getDropFlag()<<endl;
  cout << "tc_pictures:"<<getPictures()<<endl;
  cout << "closed_gop:"<<getClosedGOP()<<endl;
  cout << "broken_link:"<<getBrokenLink()<<endl;
  cout << "GOP [END]"<<endl;
}
