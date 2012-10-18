/*
  generic output class
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __OUTPUTSTREAM_H
#define __OUTPUTSTREAM_H

#include "../util/timeStamp.h"
#include "pluginInfo.h"
#include "../util/render/pictureArray.h"
#include "../util/abstract/abs_thread.h"
#include <kdemacros.h>

#define _OUTPUT_WAIT_METHOD_BLOCK       1
#define _OUTPUT_WAIT_METHOD_POLL        2
#define _OUTPUT_WAIT_METHOD_CALLBACK    3


#define _STREAM_MASK_IS_INIT   1
#define _STREAM_MASK_IS_EOF    2
#define _STREAM_MASK_IS_DATA   4

#define _STREAM_MASK_ALL       1+2+4

#define _STREAMTYPE_AUDIO      1
#define _STREAMTYPE_VIDEO      2

 
/**
   Outputstream. A nice base class. Mostly obvious methods,
   except PictureArray.
   write your picture into this  structure
   and then put it on the surface with the unlock call.

*/


/**
   life of stream states:
   --------------
   method             flag       additional calls
   construct                     audioInit()
                  
                  

   destruct                      audioInit()

   audioInit      isInit=false
                  isEof=false
                  isData=false

   audioSetup     isInit=true
   audioFlush     isEof=true
   audioPlay      isData=true


   Note: after a call of streamInit we have an "invalid" state
         obviously there is no stream where eof==false but
         and has not been initialised. 

	 another invalid state is audioInit->audioPlay

*/




class KDE_EXPORT OutputStream {

  int audioState;
  int videoState;
  abs_thread_mutex_t stateChangeMut; 
  abs_thread_cond_t  stateChangeCond;
 
 public:
  OutputStream();

  virtual ~OutputStream();

  // Audio Stream handling
  virtual int audioInit();
  virtual int audioSetup(int freq,int stereo,int sign,int big,int sampleSize);
  virtual int audioPlay(TimeStamp* startStamp,
			TimeStamp* endStamp,char *buffer, int size);  
  virtual void audioFlush();
  
  // Audio device "/dev/dsp" handling
  virtual void audioClose();
  virtual void audioOpen();

  // hack: FIX it
  virtual int getPreferredDeliverSize();


  // stream State handling 

  // we return the mask which triggerd (by "AND")
  // or the current polled mask when method is POLL
  // Note: you can only wait for "true" signals
  virtual int waitStreamState(int method,int mask,int streamType);
  
  
 

  // Video Output
  virtual int videoInit();
  virtual int openWindow(int width, int height,const char *title);
  virtual int x11WindowId();
  virtual void closeWindow();
  virtual void flushWindow();
  

  // get the current surfaces to draw into
  virtual PictureArray* lockPictureArray();
  virtual void unlockPictureArray(PictureArray* pictureArray);



  // maybe not needed:
  virtual int getFrameusec();
  virtual int getOutputInit();
  virtual void setOutputInit(int lInit);

  // Info Output
  virtual void writeInfo(class PluginInfo* pluginInfo);


  // config Output
  virtual void config(const char* key, 
		      const char* value,void* user_data);

 private:
  void initStream();

 protected:
  // sometimes useful, but use with care
  void sendSignal(int signal,int value,int streamType);

};
#endif
