/*
  reads input data
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __HTTPINPUTSTREAM_H
#define __HTTPINPUTSTREAM_H


#include "inputStream.h"

#ifndef __STRICT_ANSI__
#define __STRICT_ANSI__
#endif

#include <stdlib.h>
#include <string.h>



#if defined WIN32
#include <io.h>
#define SOCKETWRITEFUNC _write
#else

extern "C" {
#include <netdb.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
	   }
#define SOCKETWRITEFUNC write
#endif

#include <sys/types.h>
#include <errno.h>

class HttpInputStream : public InputStream{

  long byteCnt;
  
 public:
  HttpInputStream();
  ~HttpInputStream();

  int open(const char* dest);
  void close();
  int isOpen();

  int eof();
  int read(char* ptr,int size);
  int seek(long bytePos);

  long getByteLength();
  long getBytePosition();

  void print();

 private:

  FILE* fp;
  int  size;
  
  int writestring(int fd,char *string);
  int readstring(char *string,int maxlen,FILE *f);
  FILE* http_open(char *url); 

  char* proxyurl;
  unsigned long proxyip;
  unsigned int proxyport;

  int lopen;

};
#endif
