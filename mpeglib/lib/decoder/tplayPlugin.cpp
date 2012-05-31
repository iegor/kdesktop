/*
  tplay player plugin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "tplayPlugin.h"

#include "../tplay/tplayfunctions.h"

#include <iostream>

using namespace std;

TplayPlugin::TplayPlugin() {
  info=new info_struct();

  info->progname = NULL;
  info->loop = FALSE;
  info->in_seconds = FALSE;
  info->speed = DEFAULT_SPEED;
  info->bits = DEFAULT_BITS;
  info->channels = DEFAULT_CHANNELS;
  info->buffer_size = BUFFER_SIZE;
  info->show_usage = FALSE;
  info->swap = FALSE;
  info->forceraw = FALSE;
  info->force = FALSE;
  info->device = NULL;
  info->verbose = FALSE;
  info->optind = 0;
  info->buffer = NULL;
  info->firstblock = NULL;
  info->number_of_blocks = 0;
  info->readblock = 0;
  info->writeblock = 0;
  info->readcount = 0;
  info->writecount = 0;
  info->alldone = FALSE;
  info->overflow = FALSE;
  info->underflow = FALSE;
  info->audioset = FALSE;
  info->headerskip = 0;
  info->blocksize = 4096;
  info->bytes_on_last_block = 0;

  startStamp=new TimeStamp();
  endStamp=new TimeStamp();
}


TplayPlugin::~TplayPlugin() {
  delete startStamp;
  delete endStamp;
  delete info;
}



void TplayPlugin::decoder_loop() {
  int bytesread, count, stereo;
  char *p, *bufferp;

  if (input == NULL) {
    cout << "TplayPlugin::decoder_loop input is NULL"<<endl;
    exit(0);
  }
  if (output == NULL) {
    cout << "TplayPlugin::decoder_loop output is NULL"<<endl;
    exit(0);
  }

  TimeStamp* stamp;
  long pos;

  info->buffer = (char*) malloc(info->buffer_size * sizeof(char));
 
  while(runCheck()) {
    switch(streamState) {
    case _STREAM_STATE_FIRST_INIT : {
       read_header();
       
       if (info->channels == 1)
	 stereo = FALSE;
       else
	 stereo = TRUE;

       info->number_of_blocks = 0;
       bufferp = info->buffer;
       pluginInfo->setLength(getTotalLength());
       output->writeInfo(pluginInfo);
       lhasLength=true;

       setStreamState(_STREAM_STATE_INIT);
       break;
    }
    case _STREAM_STATE_INIT :
      setStreamState(_STREAM_STATE_PLAY);
      cout << "audioSetup call"<<endl;
      output->audioOpen();
      output->audioSetup(info->speed, stereo, 1, 0, info->bits);
      break;   
    case _STREAM_STATE_PLAY :
      bytesread = 0;
      count = 0;
      p = bufferp;
      while ((bytesread < info->blocksize) && (count != -1) &&
	     ((count = input->read(p,info->blocksize-bytesread))!=0)){
	p += count;
	bytesread += count;
      }
      if (info->swap) {
	swap_block(bufferp, bytesread);
      }
      
      if (bytesread > 0) {
	pos=input->getBytePosition();
	stamp=input->getTimeStamp(pos-bytesread);
      	output->audioPlay(stamp,endStamp,(char *) bufferp, bytesread);
      }

      if (bytesread < info->blocksize) {
	info->alldone = TRUE;

      }
      break;
    case _STREAM_STATE_WAIT_FOR_END:
      // exit while loop
      lDecoderLoop=false;
      break;
    default:
      cout << "unknown stream state:"<<streamState<<endl;
    }
  }
  cout << "tplay exit"<<endl;
  free(info->buffer);
  info->buffer = NULL;
  output->audioFlush();
}


int TplayPlugin::seek_impl(int second) {
  int bytes_per_second;
  int seconds;
  int back;
  

  bytes_per_second = info->speed * info->channels * (info->bits / 8);


  
  seconds = bytes_per_second * second;
  back=input->seek(seconds);

  return back;
}


void TplayPlugin::config(const char* key,const char* value,void* user_data) {
  DecoderPlugin::config(key,value,user_data);
}


void TplayPlugin::swap_block(char * buffer, int blocksize) {
  register int i;
  char c, *p;

  p = buffer;
  for (i = 0; i < (blocksize / 2); i++) {
    c = *p;
    *p = *(p + 1);
    *++p = c;
    p++;
  }
}


void TplayPlugin::read_header() {
  int bytesread, count;
  char *p, *bufferp;

  info->firstblock = (char*) malloc(info->blocksize * sizeof(char));
  bufferp = info->firstblock;
  if (!info->forceraw) {
    bytesread = 0;
    count = 0;
    p = bufferp;
    while ((bytesread < info->blocksize) && (count != -1) &&
	   ((count=input->read(p,info->blocksize-bytesread)) != 0)){
      p += count;
      bytesread += count;
    }

    if (bytesread < SUN_HDRSIZE)
      cout << "Sample size is too small"<<endl;
    
    if (read_au(info,info->firstblock) && read_wav(info,info->firstblock)) {
      if (info->verbose)
	printf("Playing raw data: %ld samples/s, %d bits, %d channels.\n",
	       info->speed, info->bits, info->channels);
    }
    
    if (info->swap)
      swap_block(bufferp, bytesread);
    
    if (bytesread < info->blocksize) {
      info->alldone = TRUE;
      info->bytes_on_last_block = bytesread;
      return;
    }
    
    if (info->headerskip) {
      count = 0;
      bytesread = info->blocksize - info->headerskip;
      p = info->firstblock + (info->blocksize - info->headerskip);
      while ((bytesread < info->blocksize) && (count != -1) &&
	     ((count = input->read(p,info->blocksize - bytesread)) != 0)) {
	p += count;
	bytesread += count;
      }
    }
    
    info->writeblock++;
    info->writecount++;
  }
  else {
    if (info->verbose)
      printf("Playing raw data: %ld samples/s, %d bits, %d channels\n",
	     info->speed, info->bits, info->channels);
  }
}

/**
   Nobody is perfect I'm too tired to fix this race
*/

int TplayPlugin::getTotalLength() {
  float wavfilesize = input->getByteLength();
  int back=0;


  float frequence=info->speed;

  int bits=info->bits;


  if (bits == 16) {
    wavfilesize=wavfilesize/2;
  }
  if (info->channels==2) {
    wavfilesize=wavfilesize/2;
  }
  if (frequence != 0) {
    back=(int)(wavfilesize/frequence);
  }
  return back;
}
