/*
  concret OutputClass
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __DSPX11OUTPUTSTREAM_H
#define __DSPX11OUTPUTSTREAM_H

#include "outputStream.h"



class DSPWrapper;
class WindowOut;
class AVSyncer;
class AudioTime;
class Performance;
class YUVDumper;

class DspX11OutputStream : public OutputStream {

  DSPWrapper* dspWrapper;
  WindowOut* x11Window;
  AVSyncer* avSyncer;
  

  int lBufferSet;
  int lVideoInit;
  int lavSync;
  int lneedInit;
  int lPerformance;
  int lYUVDump;
  
  AudioTime* audioTime;
  YUVDumper* yuvDumper;

 public:
  DspX11OutputStream(int bufferSize);
  ~DspX11OutputStream();

  // Audio Output

  int audioSetup(int frequency,int stereo,int sign,int big,int sixteen);
  void audioClose();
  void audioOpen();
  int audioPlay(TimeStamp* startStamp,
		TimeStamp* endStamp,char *buffer, int size);


  int getPreferredDeliverSize();


  // Video Output

  int openWindow(int width, int height,const char *title);
  int x11WindowId();
  void closeWindow();
  void flushWindow();

  PictureArray* lockPictureArray();
  void unlockPictureArray(PictureArray* pictureArray);

  int getFrameusec();

  int getDepth();
  int getOutputInit();
  void setOutputInit(int lInit);

  void config(const char* key,const char* value,void* user_data);

  // methods which do not belong to the outputStream intferface;
  AVSyncer* getAVSyncer();

  
};



#endif
