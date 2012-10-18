/*
  parse/stores GOP (group of picture) information from a mpegVideoStream
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */





#ifndef __GOP_H
#define __GOP_H

#include "mpegVideoStream.h"
#include "mpegExtension.h"

class GOP {
  /* Group of pictures structure. */

  int drop_flag;                     /* Flag indicating dropped frame. */
  int tc_hours;                      /* Hour component of time code.   */
  unsigned int tc_minutes;           /* Minute component of time code. */
  unsigned int tc_seconds;           /* Second component of time code. */
  unsigned int tc_pictures;          /* Picture counter of time code.  */
  int closed_gop;                    /* Indicates no pred. vectors to
				        previous group of pictures.    */
  int broken_link;                   /* B frame unable to be decoded.  */
  MpegExtension* mpegExtension;
  

 public:
  GOP();
  ~GOP();

  int processGOP(MpegVideoStream* mpegVideoStream);
  void copyTo(GOP* dest);

  inline int          getDropFlag()   { return drop_flag; }
  inline unsigned int getHour()       { return tc_hours; }
  inline unsigned int getMinutes()    { return tc_minutes; }
  inline unsigned int getSeconds()    { return tc_seconds; }
  inline unsigned int getPictures()   { return tc_pictures; }
  inline int          getClosedGOP()  { return closed_gop; }
  inline int          getBrokenLink() { return broken_link; }


  inline void    setHour(int hour)              { this->tc_hours=hour; }
  inline void    setMinute(unsigned int minute) { this->tc_minutes=minute; }
  inline void    setSecond(unsigned int second) { this->tc_seconds=second; }
  
  
  // returns diff in seconds
  int substract(GOP* minus,GOP* dest);    
  void print(const char* description);
};
#endif
