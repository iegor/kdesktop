/* 
 * tplay - buffered audio player
 *
 * (c) 1997 ilkka karvinen <ik@iki.fi>
 *
 * Copyright under the GNU GENERAL PUBLIC LICENSE
 *   (see the file COPYING in this directory)
 *
 * 
 *   common functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "tplayfunctions.h"

DWORD read_big_endian_long(char * buf)
{
   DWORD byte0, byte1, byte2, byte3;
   unsigned char* buffer=(unsigned char*) buf;

   byte0 = (DWORD) buffer[0];
   byte1 = (DWORD) buffer[1];
   byte2 = (DWORD) buffer[2];
   byte3 = (DWORD) buffer[3];
   return (byte0 << 24 | byte1 << 16 | byte2 << 8 | byte3);
}

void write_big_endian_long(char * buf, DWORD value)
{
   unsigned char* buffer=(unsigned char*) buf;
   buffer[0] = (unsigned char) (value >> 24 & 0xFF);
   buffer[1] = (unsigned char) (value >> 16 & 0xFF);
   buffer[2] = (unsigned char) (value >> 8 & 0xFF);
   buffer[3] = (unsigned char) (value & 0xFF);
}

DWORD read_little_endian_long(char* buf) {
   DWORD byte0, byte1, byte2, byte3;
   unsigned char* buffer=(unsigned char*) buf;

   byte0 = (DWORD) buffer[0];
   byte1 = (DWORD) buffer[1];
   byte2 = (DWORD) buffer[2];
   byte3 = (DWORD) buffer[3];
   return (byte3 << 24 | byte2 << 16 | byte1 << 8 | byte0);
}

WORD read_little_endian_word(char * buf)
{
   WORD byte0, byte1;
   unsigned char* buffer=(unsigned char*) buf;
   
   byte0 = (WORD) buffer[0];
   byte1 = (WORD) buffer[1];
   return (byte1 << 8 | byte0);
}

void errprintf(char *fmt,...)
{
   va_list ap;

   va_start(ap, fmt);
   vfprintf(stderr, fmt, ap);
}



void die(const char *str)
{
   fprintf(stderr, "%s: \n", str);
   exit(-1);
}

void errdie(const char *str)
{
   fprintf(stderr, "Error: %s\n", str);
   exit(-1);
}



