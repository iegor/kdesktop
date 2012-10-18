

#ifndef __AUDIOIO_H
#define __AUDIOIO_H


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif



extern "C" {
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
}

/* AUSUZ should be the amount of data your audio device will accept after it
 * has said it is ready to receive data. ie when the device is ready 
 * for data it
 * will accept it without blocking. It must also be a multiple of 128
 */

#ifdef OS_AIX
  #define AUSIZ 32768
#endif

#ifdef OS_Linux
  extern int AUSIZ;
#endif

#ifdef OS_BSD
  #define AUSIZ 32768
#endif

#if defined(OS_IRIX) || defined(OS_IRIX64)
  #define AUSIZ 32768
#endif

#ifdef OS_HPUX
  #define AUSIZ 4096
#endif

#ifdef OS_SunOS
  #define AUSIZ 4096
#endif


#ifdef DEBUG
  #define DB(type,cmd) if (debugFlags.type) { cmd ; }   
#else
  #define DB(type,cmd)
#endif




//Prototypes:

int audioConstruct();
void audioDestruct();



int audioOpen();
void audioClose();
void audioInit(int sampleSize,int frequency, int stereo,int sign, int bigendian);


int mixerOpen();
void mixerClose();
void mixerSetVolume(int volumeLeft,int volumeRight);

int audioWrite(char *buffer, int count);
int getAudioFd();
int getAudioBufferSize();

#endif
