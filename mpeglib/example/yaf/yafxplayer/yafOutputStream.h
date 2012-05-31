/*
  concret OutputClass for yaf text consol player
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __YAFOUTPUTSTREAM_H
#define __YAFOUTPUTSTREAM_H

   

//
// If we build mpeglib we include the path absolute
// otherwise we must have installed the includes (eg /usr/include/mpeglib)
//

#ifdef DIRECT_INCLUDE
#include "../../../lib/output/avSyncer.h"
#include "../../../lib/output/pluginInfo.h"
#include "../../../lib/decoder/decoderPlugin.h"
#include "../../../lib/input/inputPlugin.h"
#include "../../../lib/output/outPlugin.h"
#else 
#include <mpeglib/output/avSyncer.h>
#include <mpeglib/output/pluginInfo.h>
#include <mpeglib/decoder/decoderPlugin.h>
#include <mpeglib/input/inputPlugin.h>
#include <mpeglib/output/outPlugin.h>
#endif

#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <kdemacros.h>

// If you get compile errors when using mpeglib
// simply remove the yaf dependencies
#include "../yafcore/inputInterface.h"



class KDE_EXPORT YafOutputStream : public OutputStream {


  long bytes;
  int lInternalDevice;
  long allWrite;
  int lOpen;
  int sd;
  struct sockaddr_un sockad;
  const char* filename;
  OutputStream* directOutput;
  int fd[2];
  InputInterface* input;

  int writeToBuffer;


  

 public:
  // to the filedescriptor we write status messages from
  // the thread. This makes it thread safe
  YafOutputStream(InputInterface* input);
  ~YafOutputStream();

  // Audio Output

  int audioSetup(int frequency,int stereo,int sign,int big,int sixteen);
  void audioClose(void);
  int audioPlay(TimeStamp* startStamp,
		TimeStamp* endStamp,char *buffer, int size);

  void audioFlush();
  void audioOpen();

  int getPreferredDeliverSize();


  // Video part (no yaf protocol for this)
  // Maybe in a future version we distribute this interface
  // over shared mem or something, but for now
  // a video player must include the mpeg video lib directly
  // we forward the calls to the direct output

 
  int openWindow(int width, int height,const char *title);
  void closeWindow();
  void flushWindow();

  PictureArray* lockPictureArray();
  void unlockPictureArray(PictureArray* pictureArray);
  
  int getFrameusec();

  int getDepth();
  int getOutputInit();
  void setOutputInit(int lInit);


  // Info Output
  void writeInfo(PluginInfo* pluginInfo);
 
  void config(const char* key,const char* value,void* user_data);


  // additional functionality (needed for the yaf-protocol)
  void setBytesCounter(long value);


  int isOpenStream();
  void setStreamFile(const char* filename);
  int openStream();
  int closeStream();

  void internalDevice(int lInternal);
  long getBytesCounter();
  long getAllWriteCounter();


  

  


};



#endif
