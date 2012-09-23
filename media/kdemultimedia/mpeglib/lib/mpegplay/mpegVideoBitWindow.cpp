/*
  bitwindow mpeg video
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "mpegVideoBitWindow.h"

#include <iostream>

using namespace std;


MpegVideoBitWindow::MpegVideoBitWindow() {
  // Make buffer length multiple of 4. 
  
  this->size=80000;



  if ((size % 4) != 0) {
    cout << "MpegVideoStream not multiple of 4"<<endl;
    exit(-1);
  }

  /* Create MpegVideoStream. */
  buf_start = (unsigned int*) malloc (sizeof(unsigned int)*(size*4));
  
  /*
   * Set max_buf_length to one less than actual length to deal with messy
   * data without proper seq. end codes.
   */
  max_buf_length=size-1;

  
  /* Initialize bitstream i/o fields. */

  bit_offset = 0;
  buffer = buf_start;
  buf_length = 0;
  num_left=0;
  leftover_bytes=0;
  curBits = 0;




  nBitMask[0] = 0x00000000;
  nBitMask[1] = 0x80000000;
  nBitMask[2] = 0xc0000000;
  nBitMask[3] = 0xe0000000;
  nBitMask[4] = 0xf0000000;
  nBitMask[5] = 0xf8000000;
  nBitMask[6] = 0xfc000000;
  nBitMask[7] = 0xfe000000;
  nBitMask[8] = 0xff000000;
  nBitMask[9] = 0xff800000;
  nBitMask[10] = 0xffc00000;
  nBitMask[11] = 0xffe00000;
  nBitMask[12] = 0xfff00000;
  nBitMask[13] = 0xfff80000;
  nBitMask[14] = 0xfffc0000;
  nBitMask[15] = 0xfffe0000;
  nBitMask[16] = 0xffff0000;
  nBitMask[17] = 0xffff8000;
  nBitMask[18] = 0xffffc000;
  nBitMask[19] = 0xffffe000;
  nBitMask[20] = 0xfffff000;
  nBitMask[21] = 0xfffff800;
  nBitMask[22] = 0xfffffc00;
  nBitMask[23] = 0xfffffe00;
  nBitMask[24] = 0xffffff00;
  nBitMask[25] = 0xffffff80;
  nBitMask[26] = 0xffffffc0;
  nBitMask[27] = 0xffffffe0;
  nBitMask[28] = 0xfffffff0;
  nBitMask[29] = 0xfffffff8;
  nBitMask[30] = 0xfffffffc;
  nBitMask[31] = 0xfffffffe;
  nBitMask[32] = 0xffffffff;


}


MpegVideoBitWindow::~MpegVideoBitWindow() {
  delete buf_start;
}



int MpegVideoBitWindow::appendToBuffer(unsigned char* ptr,int len) {
  int byte_length = getLength()*4;


  resizeBuffer(len);
  
  if (num_left != 0) {
    byte_length += num_left;
    *(buffer+buf_length)=leftover_bytes;
  }
  memcpy(((unsigned char *)buffer)+byte_length,ptr,len);
  
  if ((unsigned long int)1 != ntohl(1)) {
    unsigned int *mark = buffer+buf_length;
    int i;
    int n=(len+num_left)&0xfffffffc;
    for (i=0; i < n; i+=4) {
      *mark=ntohl(*mark);
      mark++;
    }
  }
  byte_length = byte_length + len;
  num_left = byte_length % 4;
  buf_length = byte_length / 4;
  updateCurBits();
  
  leftover_bytes = *(buffer +buf_length);
  return true;
}

int MpegVideoBitWindow::getLinearFree() {
  unsigned int* endPos=buf_start+size;
  unsigned int* currPos=buffer+buf_length;
  
  int back=endPos-currPos;
  return back;
}





void MpegVideoBitWindow::flushByteOffset() {
  int byteoff;

  byteoff = bit_offset % 8;

  if (byteoff != 0) {
    flushBitsDirect((8-byteoff));
  }
}


void MpegVideoBitWindow::appendToBuffer(unsigned int startCode) {
  unsigned int startCodeRaw=htonl(startCode);
  resizeBuffer(4);
  appendToBuffer((unsigned char*)&startCodeRaw,4);
}


void MpegVideoBitWindow::clear() {
  buffer = buf_start;
  buf_length = 0;
  bit_offset = 0;
  curBits = 0;
}


int MpegVideoBitWindow::getLength() {
  return buf_length;
}







void MpegVideoBitWindow::printChar(int bytes) {
  int i;
  unsigned char* mark;

  mark=(unsigned char *)buffer;

  for(i=0;i<bytes;i++) {
    printf("i:%d read=%x\n",i,mark[i]);
  }
  printf("*********\n");

}


void MpegVideoBitWindow::printInt(int bytes) {
  int i;
  int n;
  unsigned int* mark;
  
  mark=(unsigned int*)buf_start;

  n=bytes/sizeof(int);
  for(i=0;i<n;i++) {
    printf("i:%d read=%x\n",i,mark[i]);
  }
  printf("*********\n");

}


void MpegVideoBitWindow::print() {
  int byte_length = getLength()*4;

  printf("bit_offset:%x\n",bit_offset);
  printf("num_left:%x\n",num_left);
  printf("leftover_bytes:%x\n",leftover_bytes);
  printf("buf_length:%x\n",buf_length);
  printf("curBits:%x\n",curBits);
  printf("pos:%8x\n",byte_length);
  printChar(8);
}


void MpegVideoBitWindow::resizeBuffer(int insertBytes) {
  /* Read all the headers, now make room for packet */
  if (buf_start+max_buf_length < buffer+insertBytes/4+buf_length) {

    if (max_buf_length - buf_length < (int) insertBytes/4) {

      /* Buffer too small for a packet (plus whats there), 
       * time to enlarge it! 
       */

      unsigned int *old = buf_start;
      max_buf_length=buf_length+insertBytes/4+1;

      buf_start=(unsigned int*) malloc(sizeof(unsigned int)*max_buf_length);
      if (buf_start == NULL) {
	cout << "allocation of:"<<max_buf_length<<" bytes failed"<<endl;
	exit(0);
      }
      memcpy((unsigned char *)buf_start,buffer,(unsigned int)buf_length*4);
      delete old;
      buffer = buf_start;
      cout << "enlarge buffer-1 end***********"<<endl;

    } else {
      memcpy((unsigned char *)buf_start,
	     buffer, (unsigned int) buf_length*4);
      buffer = buf_start;
    }
  }
}


void MpegVideoBitWindow::fillWithIsoEndCode(int bytes) {
  int i;
  int n=bytes/4;
  for (i=0;i<n;i++) {
    appendToBuffer(ISO_11172_END_CODE);
  }
}
