/* this file is a part of amp software, (C) tomislav uzelac 1996,1997

	Origional code by: tomislav uzelac
	Modified by:
	* Dan Nelson - BSD mods.
	* Andrew Richards - moved code from audio.c and added mixer support etc
        * Martin Vogt
 */

/* Support for Linux and BSD sound devices */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "audioIO.h"
#include <stdlib.h>
#include <stdio.h>

//
// Why all these different system cannot make a standard where this
// soundcard.h file is ?
//
#if defined(HAVE_SYS_SOUNDCARD_H)
	#undef AUSIZ
	#undef HAVE_SYS_SOUNDCARD_H
	#include <sys/soundcard.h>
#elif defined(HAVE_MACHINE_SOUNDCARD_H) 
	#undef AUSIZ
	#include <machine/soundcard.h>
#elif defined(__NetBSD__)
	#undef AUSIZ
	#include <soundcard.h>
#else
	// fallback:
	#include <linux/soundcard.h>
#endif


/* optimal fragment size */

int AUSIZ = 0;

// declare these static to effectively isolate the audio device 

static int audio_fd;
static int mixer_fd;
static int volumeIoctl;



int audioConstruct() {
  audio_fd=-1;
  mixer_fd=-1;
  return true;
}


void audioDestruct() {
  
}



/* 
   should open the audio device, perform any special initialization	
*/
int  audioOpen() {
  audio_fd = open ("/dev/dsp", O_WRONLY, 0);
  if (audio_fd < 0)  {
    perror("Unable to open the audio");
  }

  // Ok here something important if your programm forks:
  if (audio_fd > 0) {
    if (fcntl(audio_fd,F_SETFD,true) < 0) {
      perror("fcntl socket");exit(1);
    }
  }
  
  return (audio_fd > 0);
}

inline void audioFlush() {
  if (ioctl(audio_fd, SNDCTL_DSP_RESET, 0) == -1)
    perror("Unable to reset audio device\n");
}

/* 
   should close the audio device and perform any special shutdown 
*/
void audioClose() {
  audioFlush();
  if (close(audio_fd) < 0) {
    perror("error close audiodevice:");
  }
}


void audioInit(int sampleSize,int frequency, int stereo, int sign, int big) {
  if( sign == 0 )
  {
      fprintf(stderr,
	      "%s, %d: expecting signed audio data, "
	      "initialized unsigned (ignored)\n",
	      __FILE__, __LINE__ );
  }
  if( big != 0 )
  {
      fprintf(stderr,
	      "%s, %d: expecting little endian audio data, "
	      "initialized big endian (ignored)\n",
	      __FILE__, __LINE__ );
  }

  int play_format=AFMT_S16_LE;

  if (sampleSize == 8) {
    play_format=AFMT_S8;
  }
  ioctl(audio_fd,SNDCTL_DSP_RESET,NULL);
  
  if (ioctl(audio_fd, SNDCTL_DSP_SETFMT,&play_format) < 0) {
    perror("Unable to set required audio format\n");
  }
  
  /* Set 1 or 2 channels */
  stereo=(stereo ? 1 : 0);

  if (ioctl(audio_fd, SNDCTL_DSP_STEREO, &stereo) < 0) {
    perror("Unable to set stereo/mono\n");
    exit(0);
  }
  
  /* Set the output frequency */
  if (ioctl(audio_fd, SNDCTL_DSP_SPEED, &frequency) < 0) {
    perror("Unable to set frequency");
    exit(0);
  }
  
  if (ioctl(audio_fd, SNDCTL_DSP_GETBLKSIZE, &AUSIZ) == -1) {
    perror("Unable to get fragment size\n");
    exit(0);
  }
}


int getAudioBufferSize() {
  struct audio_buf_info buf_info;
  int buf=1024*65;
  if (ioctl(audio_fd,SNDCTL_DSP_GETOSPACE,&buf_info) == -1) {
    perror("ioctl getAudioBufferSize using default");
  } else {
    buf=buf_info.bytes;
  }
  return buf;
}


int mixerOpen() {
  int supportedMixers;

  if ((mixer_fd=open("/dev/mixer",O_RDWR)) == -1) {
    perror("Unable to open mixer device");
  }
  
  // Ok here something important if your programm forks:
  if (mixer_fd > 0) {
    if (fcntl(mixer_fd,F_SETFD,true) < 0) {
      perror("fcntl socket");exit(1);
    }
  }
  
  if (ioctl(mixer_fd, SOUND_MIXER_READ_DEVMASK, &supportedMixers) == -1){
    perror("Unable to get mixer info assuming master volume");
    volumeIoctl=SOUND_MIXER_WRITE_VOLUME;
  } else {
    if ((supportedMixers & SOUND_MASK_PCM) != 0)
      volumeIoctl=SOUND_MIXER_WRITE_PCM;
    else
      volumeIoctl=0;
  }

  return (mixer_fd > 0);
}


void mixerClose() {
  if (mixer_fd != -1) {
    close(mixer_fd);
  }
}

/* 
   only code this if your system can change the volume while 
   playing
*/
void mixerSetVolume(int leftVolume,int rightVolume) {
  int volume;

  volume=leftVolume+(rightVolume<<8);
  if ((mixer_fd != -1) && (volumeIoctl!=0)) {
    if (ioctl(mixer_fd, volumeIoctl, &volume) < 0) {
      perror("Unable to set sound volume");
    }
  }
}



int audioWrite(char *buffer, int count) {
  return(write(audio_fd,buffer,count));
}


int getAudioFd() {
  return(audio_fd);
}
