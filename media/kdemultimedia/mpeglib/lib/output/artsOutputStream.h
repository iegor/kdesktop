/*
  output to arts
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __ARTSOUTPUTSTREAM_H
#define __ARTSOUTPUTSTREAM_H

#include "outputStream.h"
#include "../input/bufferInputStream.h"
#include "audioTime.h"


class WindowOut;
class AVSyncer;




/**
   a word about synchronisation: Arts currently has not asynchronous
   starting of stream, which means, poll if stream is initialized
   or set a callbackfunction.

   mpeglib is threaded and would not have much problems
   with a "callback" but currently we only support blocking wait
   for initilisation.

   I think, if people realize that smooth mixing of streams
   is impossible(because of the blocking start behaviour),
   this will become a desired feature :-)

*/


/**
   This class offers a blocking device, its behaviour is similar
   to /dev/dsp we use a buffer to store the pcm data, if
   we open/close the buffer the stored data is removed
   and the blocking thread is "kicked"

   Arts must first wait for an init signal. it has its own
   method to check for eof. during seek we close this
   device, after seek we reopen it.

   close and open have the same functionality, they simply
   clear the buffer.
*/

   

class ArtsOutputStream : public OutputStream {

  BufferInputStream* stream;
  AudioTime* audioTime;

  void (*streamStateChangeCallback)(void*);

  WindowOut* x11Window;
  AVSyncer* avSyncer;
  int privateBufferSize;
  class ThreadQueue* threadQueue;

 public:
  ArtsOutputStream(void (*streamStateChangeCallback)(void*));
  ~ArtsOutputStream();

  // Audio part

  int audioSetup(int freq,int stereo,int sign,int big,int sampleSize);
  void audioClose();
  void audioOpen();
  void audioFlush();
  
  int audioPlay(TimeStamp* startStamp,
		TimeStamp* endStamp,char *buffer, int size);


  AudioTime* getAudioTime();

  int getPreferredDeliverSize();

  // Video part

  int openWindow(int width, int height,const char *title);
  int x11WindowId();
  void closeWindow();
  void flushWindow();

  PictureArray* lockPictureArray();
  void unlockPictureArray(PictureArray* pictureArray);

  int getFrameusec();

  void config(const char* key,const char* value,void* user_data);

  // Remote read extension

  int read(char** buffer,int bytes);
  void forwardReadPtr(int bytes);

  // buffer control
  int getBufferFillgrade();

  // sync control
  void setAudioBufferSize(int size);
  
 private:
  void sendSignal(int state,int value);
  void initStream();
  
};
#endif
