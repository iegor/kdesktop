/*
  info about vorbis files.
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __VORBISINFO_H
#define __VORBISINFO_H


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef OGG_VORBIS

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include "../util/file/fileAccess.h"

/**
   callbacks from vorbisfile
*/
extern "C" {

extern size_t  fread_func2  (void *ptr,size_t size,size_t nmemb, void *stream);
extern int     fseek_func2  (void *stream, ogg_int64_t offset, int whence);
extern int     fclose_func2 (void *stream);
extern long    ftell_func2 (void *stream);
 
}



class VorbisInfo {

  FileAccess* input;
  OggVorbis_File* vf;
  vorbis_info *vi;

  long lastSeekPos;

 public:
  VorbisInfo(FileAccess* input);
  ~VorbisInfo();
  
  // returns byte positions
  long getSeekPosition(int second);
  // returns length in seconds
  long getLength();
  
  void print(const char* msg);
  
  void setSeekPos(long pos);
  long getSeekPos();

  FileAccess* getInput();


};

#endif

#endif
