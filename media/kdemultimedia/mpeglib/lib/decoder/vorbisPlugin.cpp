/*
  vorbis player plugin
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "vorbisPlugin.h"

#include <iostream>

using namespace std;

#ifdef OGG_VORBIS

size_t fread_func(void *ptr, size_t size, size_t nmemb, void *stream) {
  InputStream* input=((VorbisPlugin*)stream)->getInputStream();
  int bytes=input->read((char*)ptr,size*nmemb);
  // workaround for RC3, report success during seek
  /*
  if (((VorbisPlugin*)stream)->vorbis_seek_bug_active == true) {
    errno=0;
    return 0;
  }
  */
  // error on read and no "seek workaround"
  if (bytes == 0) {
    //
    // If different thread close the input we signal
    // a read error to vorbis
    //
    if (input->isOpen() == false) {
      // note: errno is in this thread another variable, than in 
      // the thread which closed the file.
      // here we "fake" the errno var.
      errno=EBADF;
      return 0;
    }
  }
  // successful read
  return bytes;
}


int fseek_func(void *stream, ogg_int64_t offset, int whence) {
  int ret=-1;
  InputStream* input=((VorbisPlugin*)stream)->getInputStream();

  switch(whence) {
  case SEEK_SET:
    ret=input->seek(offset);
    break;
  case SEEK_CUR:
    ret=input->seek(input->getBytePosition()+offset);
    break;
  case SEEK_END:
    ret=input->seek(input->getByteLength());
    break;
  default:
    cout << "fseek_func VorbisPlugn strange call"<<endl;
  }

  if (ret == false) {
    // vorbis does not handle errors in seek at all
    // and if they are handled vorbis segfaults. (RC3)
    // here we always "success" but set an seek_error_mark
    // 
    ret=-1;
  }

  return ret;

}


int fclose_func (void *) {
  //InputStream* input=(InputStream*) stream;

  // its handled different in kmpg
  // we close the stream if the decoder signals eof.
  return true;

}


long ftell_func  (void *stream) {
  InputStream* input=((VorbisPlugin*)stream)->getInputStream();
  return input->getBytePosition();
}


VorbisPlugin::VorbisPlugin() {
  

  memset(&vf, 0, sizeof(vf));
  timeDummy=new TimeStamp();
  pcmout=new char[4096];
  lnoLength=false;
  lshutdown=true;

}


VorbisPlugin::~VorbisPlugin() {
  delete timeDummy;
  delete pcmout;
}


// here we can config our decoder with special flags
void VorbisPlugin::config(const char* key,const char* value,void* user_data) {

  if (strcmp(key,"-c")==0) {
    lnoLength=true;
  }
  DecoderPlugin::config(key,value,user_data);
}


int VorbisPlugin::init() {
  ov_callbacks callbacks;

  callbacks.read_func = fread_func;
  callbacks.seek_func = fseek_func;
  callbacks.close_func = fclose_func;
  callbacks.tell_func = ftell_func;

  // here is the hack to pass the pointer to
  // our streaming interface.
  
  if(ov_open_callbacks(this, &vf, NULL, 0, callbacks) < 0) {
    return false;
  }

  return true;
}


// called by decoder thread
int VorbisPlugin::processVorbis(vorbis_info* vi,vorbis_comment* comment) {

  // decode
  int ret;
  int current_section=-1; /* A vorbis physical bitstream may
			     consist of many logical sections
			     (information for each of which may be
			     fetched from the vf structure).  This
			     value is filled in by ov_read to alert
			     us what section we're currently
			     decoding in case we need to change
			     playback settings at a section
			     boundary */
  ret=ov_read(&vf,pcmout,4096,0,2,1,&current_section);
  switch(ret){
  case 0:
    /* EOF */
    lDecoderLoop=false;
    break;
  case -1:
    /* error in the stream.  Not a problem, just reporting it in
       case we (the app) cares.  In this case, we don't. */
    cout << "error found"<<endl;
    break;  
  default:
    if(current_section!=last_section){
      vi=ov_info(&vf,-1); /* The info struct is different in each
			     section.  vf holds them all for the
			     given bitstream.  This requests the
			     current one */
      
      double timeoffset=ov_time_tell(&vf);
      
      comment = ov_comment(&vf, -1);
      if(comment) {
	cout << "we have a comment:"<<timeoffset<<endl;
      }
    }  
      last_section=current_section;
      output->audioPlay(timeDummy,timeDummy,pcmout,ret);
      break;
    }
  return true;
}


void VorbisPlugin::decoder_loop() {
  vorbis_info *vi=NULL;
  vorbis_comment *comment=NULL;
  last_section=0;
  current_section=0;
       


  if (input == NULL) {
    cout << "VorbisPlugin::decoder_loop input is NULL"<<endl;
    exit(0);
  }
  if (output == NULL) {
    cout << "VorbisPlugin::decoder_loop output is NULL"<<endl;
    exit(0);
  }
  // init audio stream
  output->audioInit();

  /********** Decode setup ************/
  // start decoding
  lshutdown=false;
  /** 
      Vorbis RC3 introducted a new bug:
          seek on a closed stream segfaults vorbis.
	  The bugfix in vorbis is (RC3) around line 1190 should be:
                seek_error:
		  // the caller of this goto may not set a return code 
                  ret=OV_ENOSEEK;
      The workaround is to check if we are in a seek operation
      and always "fake" successful reads.
  */
  vorbis_seek_bug_active=false;


  while(runCheck()) {

    switch(streamState) {
    case _STREAM_STATE_FIRST_INIT :
      if (init()== false) {
	// total failure. exit decoding
	lDecoderLoop=false;
	break;
      }	
      // now init stream
      vi=ov_info(&vf,-1);
      if (lnoLength==false) {
	pluginInfo->setLength(getTotalLength());
	output->writeInfo(pluginInfo);
      }
      output->audioOpen();
      output->audioSetup(vi->rate,vi->channels-1,1,0,16);
  

      lhasLength=true;
      setStreamState(_STREAM_STATE_PLAY);
      break;
    case _STREAM_STATE_INIT :
    case _STREAM_STATE_PLAY :
      processVorbis(vi,comment);
      break;
    case _STREAM_STATE_WAIT_FOR_END:
      // exit while loop
      lDecoderLoop=false;
      usleep(2000000);
      break;
    default:
      cout << "unknown stream state vorbis decoder:"<<streamState<<endl;
    }

  }

  lshutdown=true;
  ov_clear(&vf); /* ov_clear closes the stream if its open.  Safe to
		    call on an uninitialized structure as long as
		    we've zeroed it */
  memset(&vf, 0, sizeof(vf));

  output->audioFlush();
}

// vorbis can seek in streams
int VorbisPlugin::seek_impl(int second) {
  vorbis_seek_bug_active=true;
  ov_time_seek(&vf,(double) second);
  vorbis_seek_bug_active=false;
  return true;
}



int VorbisPlugin::getTotalLength() {
  int back=0;
  int byteLen=input->getByteLength();
  if (byteLen == 0) {
    return 0;
  }
  /* Retrieve the length in second*/
  shutdownLock();
  if (lshutdown==false) {
      back = (int) ov_time_total(&vf, -1);
  }
  shutdownUnlock();
  
  return back;
}




#endif
//OGG_VORBIS


