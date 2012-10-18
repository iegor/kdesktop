/*
  reads input data
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "httpInputStream.h"

#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif

#include <iostream>

using namespace std;

static const char *httpstr="http://";



static char *strndup(char *src,int num) {
  char *dst;

  if(!(dst=(char *)malloc(num+1)))return NULL;
  dst[num]='\0';

  return strncpy(dst, src, num);
}

static char *url2hostport(char *url,char **hname,
			  unsigned long *hip,unsigned int *port) {
  char *cptr;
  struct hostent *myhostent;
  struct in_addr myaddr;
  int isip=1;

  if(!(strncmp(url,httpstr,7)))url+=7;
  cptr=url;
  while(*cptr && *cptr!=':' && *cptr!='/') {
    if((*cptr<'0' || *cptr>'9') && *cptr!='.')isip=0;
    cptr++;
  }
  if(!(*hname=strndup(url,cptr-url))) {
    *hname=NULL;
    return NULL;
  }
  if(!isip)
  {
    if (!(myhostent=gethostbyname(*hname)))return NULL;
    memcpy(&myaddr,myhostent->h_addr,sizeof(myaddr));
    *hip=myaddr.s_addr;
  }
  else if((*hip=inet_addr(*hname))==INADDR_NONE)return NULL;
  if(!*cptr || *cptr=='/') {
    *port=80;
    return cptr;
  }
  *port=atoi(++cptr);
  while(*cptr && *cptr!='/')cptr++;
  return cptr;
}













HttpInputStream::HttpInputStream() {

  proxyurl=NULL;
  proxyip=0;
  lopen=false;
  byteCnt=0;
}


HttpInputStream::~HttpInputStream() {
  close();
}


int HttpInputStream::open(const char* filename) {
  close();
  if (filename == NULL) {
    return false;
  }
  /*
  int matchPos=InputStream::getPath(filename,"http");
  if (matchPos=0) {
    return false;
  }
  */

  char* filename2=strdup(filename);
  if((fp=http_open(filename2))==NULL) {
    cout << "seterrorcode(SOUND_ERROR_FILEOPENFAIL)"<<endl;
    delete filename2;
    return false;
  }
  delete filename2;
  lopen=true;
  setUrl(filename);
  return lopen;
}


void HttpInputStream::close() {
  if (isOpen()) {
    ::fclose(fp);
  }
  lopen=false;
  setUrl(NULL);
}


int HttpInputStream::isOpen() {
  return lopen;
}


int HttpInputStream::eof() {
  if (isOpen()==false){
    return true;
  }
  return feof(fp);
}


int HttpInputStream::read(char* ptr,int size) {
  int bytesRead=0;
  if (isOpen()) {
    bytesRead=fread(ptr, 1,size, fp);
    if (ferror(fp) != 0){
      cout <<"http fread error"<<endl;
    } else {
      byteCnt+=bytesRead;
    }
  }
  return bytesRead;
}

int HttpInputStream::seek(long posInBytes) {
  cout << "HttpInputStream::setBytePos not implemented:"<<posInBytes<<endl;
  return false;
}



long HttpInputStream::getByteLength() {
  cout << "HttpInputStream::getByteLength not implemented"<<endl;
  return 0;
}



void HttpInputStream::print() {
  printf("pos in file:%8x\n",(int)ftell(fp));
}


int HttpInputStream::writestring(int fd, char *string) {
  int result,bytes=strlen(string);

  while (bytes) {
    if((result=SOCKETWRITEFUNC(fd,string,bytes))<0 && errno!=EINTR) {
      cout << "writestring fail -1"<<endl;
      return false;
    }
    else if(result==0) {
      cout << "writestring fail -2"<<endl;
      return false;
    }
    string += result;
    bytes -= result;
  }
  return true;
}



FILE* HttpInputStream::http_open(char *url) {
  char *purl=NULL,*host,*request,*sptr;
  char agent[50];
  int linelength;
  unsigned long myip;
  unsigned int myport;
  int sock;
  int relocate=0,numrelocs=0;
  struct sockaddr_in server;
  FILE *myfile;
  if (url == NULL) {
    cout << "cannot open NULL http_open"<<endl;
    return NULL;
  }
  if (strlen(url)==0) {
    cout << "zero length http_open"<<endl;
    return NULL;
  }

  if(!proxyip)
  {
    if(!proxyurl)
      if(!(proxyurl=getenv("MP3_HTTP_PROXY")))
	if(!(proxyurl=getenv("http_proxy")))
	  proxyurl = getenv("HTTP_PROXY");
    if (proxyurl && proxyurl[0] && strcmp(proxyurl, "none"))
    {
      if (!(url2hostport(proxyurl, &host, &proxyip, &proxyport)))
      {
	cout << "seterrorcode(SOUND_ERROR_UNKNOWNPROXY)"<<endl;;
	return NULL;
      }
      if(host)free(host);
    }
    else
      proxyip = INADDR_NONE;
  }
  
  if((linelength=strlen(url)+100)<1024)
    linelength=1024;
  if(!(request=(char *)malloc(linelength)) || !(purl=(char *)malloc(1024))) 
  {
    cout << "seterrorcode(SOUND_ERROR_MEMORYNOTENOUGH)"<<endl;
    return NULL;
  }
  strncpy(purl,url,1023);
  purl[1023]='\0';
  do{
    strcpy(request,"GET ");
    if(proxyip!=INADDR_NONE) 
    {
      if(strncmp(url,httpstr,7))
	strcat(request,httpstr);
      strcat(request,purl);
      myport=proxyport;
      myip=proxyip;
    }
    else
    {
      if(!(sptr=url2hostport(purl,&host,&myip,&myport)))
      {
	cout << "seterrorcode(SOUND_ERROR_UNKNOWNHOST)"<<endl;;
	return NULL;
      }
      if (host)
	free (host);
      strcat (request, sptr);
    }
    sprintf (agent, " HTTP/1.0\r\nUser-Agent: %s/%s\r\n\r\n",
	     "Splay","0.6");
    strcat (request, agent);
    server.sin_family = AF_INET;
    server.sin_port = htons(myport);
    server.sin_addr.s_addr = myip;
    if((sock=socket(PF_INET,SOCK_STREAM,6))<0) {
      cout <<"seterrorcode(SOUND_ERROR_SOCKET)"<<endl;
      return NULL;
    }
    if(connect(sock,(struct sockaddr *)&server,sizeof(server))) {
      cout <<"seterrorcode(SOUND_ERROR_CONNECT)"<<endl;
      return NULL;
    }
    if(!writestring(sock,request))return NULL;
    if(!(myfile=::fdopen(sock, "rb"))) {
      cout << "seterrorcode(SOUND_ERROR_FDOPEN)"<<endl;
      return NULL;
    };
    relocate=false;
    purl[0]='\0';
    if(!readstring(request,linelength-1,myfile))return NULL;
    if((sptr=strchr(request,' '))) {
      switch(sptr[1]) {
        case '3':relocate=true;
        case '2':break;
        default: 
	  cout <<"seterrorcode(SOUND_ERROR_HTTPFAIL)"<<endl;
	  return NULL;
      }
    }
    do {
      if(!readstring(request,linelength-1,myfile))return NULL;
      if(!strncmp(request,"Location:",9))
	strncpy (purl,request+10,1023);
    } while (request[0]!='\r' && request[0]!='n');
  } while(relocate && purl[0] && numrelocs++<5);
  if(relocate) { 
    cout << "seterrorcode(SOUND_ERROR_TOOMANYRELOC)"<<endl;
    return NULL;
  }
  free(purl);
  free(request);
  return myfile;
}

long HttpInputStream::getBytePosition() {
  return 0;
}

int HttpInputStream::readstring(char *string,int maxlen,FILE *f) {
  char *result;

  do{
    result=fgets(string,maxlen,f);
  }while(!result  && errno==EINTR);
  if(!result)
  {
    cout << "seterrorcode(SOUND_ERROR_FILEREADFAIL)"<<endl;
    return false;
  }

  return true;
}
