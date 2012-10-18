/*
  bitwindow mpeg video
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __MPEGVIDEOBITWINDOW_H
#define __MPEGVIDEOBITWINDOW_H

#include "../input/inputPlugin.h"


#define ISO_11172_END_CODE          ((unsigned int)0x000001b9)

class MpegVideoBitWindow {
  int size;                             /* size of buffer */
  unsigned int bit_offset;              /* Bit offset in stream.      */
  unsigned int *buffer;                 /* Pointer to next byte in buffer */
  int buf_length;                       /* Length of remaining buffer.*/
  unsigned int *buf_start;              /* Pointer to buffer start.   */
  int max_buf_length;                   /* Max length of buffer.      */
  unsigned int num_left;                /* from ReadPacket - leftover */
  unsigned int leftover_bytes;          /* from ReadPacket - leftover */
  unsigned int curBits;                 /* current bits               */
  

  unsigned int nBitMask[33];


 public:
  MpegVideoBitWindow();
  ~MpegVideoBitWindow();
  
  int appendToBuffer(unsigned char* ptr,int len);
  int getLinearFree();
  // true if feof() is true and the buffer is emtpy
  int isEof();
  void flushByteOffset();
  void appendToBuffer(unsigned int startCode);

  
  inline void updateCurBits() {
    curBits = *buffer << bit_offset;
  }

  inline unsigned int showBits(int bits) {
    
    unsigned int mask=nBitMask[bits];
    int shift=32-(bits);
    int bO;                                                               
    shift=(curBits & mask)>>shift;
    bO = bit_offset + bits;                                    
    if (bO > 32) {
      return (shift | (*(buffer+1)>>(64-bO)));
    }                                                                     
    return shift;    
  }

  inline unsigned int showBits32() {
    if (bit_offset) {                                         
      return (curBits | (*(buffer+1) >> (32 - bit_offset)));   
    }                                                                     
    return curBits;  
  }

  inline void flushBitsDirect(unsigned int bits) {
    bit_offset += bits;       
    if (bit_offset & 0x20) {                                
      bit_offset -= 32;
      bit_offset &= 0x1f;
      buffer++;                                             
      updateCurBits();
      buf_length--;                                         
    }                                                                   
    else {                                                              
      curBits <<= bits;                                      
    }    
  }
 
  inline unsigned int getBits(int bits) {
    unsigned int result=showBits(bits);
    flushBitsDirect(bits);
    return result;
  }
  
  void clear();
  int getLength();

  int hasBytes(int bytes);

  void printChar(int bytes);
  void printInt(int bytes);
  void print();
  void resizeBuffer(int insertBytes);
  void fillWithIsoEndCode(int bytes);



};
#endif
