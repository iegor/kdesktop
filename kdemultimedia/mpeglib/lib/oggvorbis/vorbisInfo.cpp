/*
  info about vorbis files.
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "vorbisInfo.h"

#include <iostream>

using namespace std;

#ifdef OGG_VORBIS
#define CHUNKSIZE 4096

#define GETINPUT(stream,input)                                            \
                               VorbisInfo* info=(VorbisInfo*) stream;     \
                               FileAccess* input=info->getInput();   


size_t fread_func2(void *ptr, size_t size, size_t nmemb, void *stream) {
  GETINPUT(stream,input);

  size_t want=size*nmemb;
  size_t back=input->read((char*)ptr,want);
  return back;
}


int fseek_func2(void *stream, ogg_int64_t offset, int whence) {
  int ret;
  GETINPUT(stream,input);

  if (whence==SEEK_SET) {
    ret=input->seek(offset);
    info->setSeekPos(offset);
    return ret;
  }
  if (whence==SEEK_CUR) {
    ret=input->seek(input->getBytePosition()+offset);
    return ret;
  }  
  if (whence==SEEK_END) {
    ret=input->seek(input->getByteLength());
    return ret;
  }   
  cout << "hm, strange call"<<endl;
  return -1;
}


int fclose_func2 (void * stream) {
  cout << "fclose_func"<<endl;
  GETINPUT(stream,input);
  // its handled different in kmpg
  // we close the stream if the decoder signals eof.
  return true;

}


long ftell_func2  (void *stream) {
  GETINPUT(stream,input);
  return input->getBytePosition();
}




VorbisInfo::VorbisInfo(FileAccess* input) {
  this->input=input;
  vf=new OggVorbis_File();

  ov_callbacks callbacks;

  callbacks.read_func  = fread_func2;
  callbacks.seek_func  = fseek_func2;
  callbacks.close_func = fclose_func2;
  callbacks.tell_func  = ftell_func2;
 
  if(ov_open_callbacks(this, vf, NULL, 0, callbacks) < 0) {
    cout << "error ov_open_callbacks"<<endl;
  }
  
  // now init stream
  vi=ov_info(vf,-1);
  lastSeekPos=0;
}


VorbisInfo::~VorbisInfo() {
  delete vf;
  if (vi != NULL) {
    //?
  }
}


long VorbisInfo::getSeekPosition(int seconds) {
  int back=0;
  if (vi != NULL) {
    lastSeekPos=0;
    ov_time_seek(vf,seconds);
    back=lastSeekPos;
  }
  return back;
}


long VorbisInfo::getLength() {
  int back=0;
  if (vi != NULL) {
    back = (int) ov_time_total(vf, -1);
  }
  return back;
}




void VorbisInfo::print(const char* msg) {
  cout << "VorbisInfo:"<<msg<<endl;
  cout << "Length (sec):"<<getLength()<<endl;

}


void VorbisInfo::setSeekPos(long pos) {
  this->lastSeekPos=pos;
}


long VorbisInfo::getSeekPos() {
  return lastSeekPos;
}

FileAccess* VorbisInfo::getInput() {
  return input;
}



#endif
