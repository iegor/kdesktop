/* this file is a part of amp software, (C) tomislav uzelac 1996,1997

	Origional code by: tomislav uzelac
	Modified by:
	* Andrew Richards - moved code from audio.c
	* Jim Crumley - ported some code from other audioIO_'s

 */

#include <sys/types.h>
#include <sys/stropts.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/audioio.h>
#include "audioIO.h"
#include <iostream.h>

/* declare these static to effectively isolate the audio device */

static int audio_fd;
static audio_info_t auinfo;


int audioConstruct(){
  audio_fd=-1;
  return true;
}

void audioDestruct() {

}


/* audioOpen() */
/* should open the audio device and perform any special initialization */
/* returns the file descriptior of the audio device										 */

int audioOpen() {
  AUDIO_INITINFO(&auinfo);

  if ((audio_fd = open("/dev/audio",O_RDWR))==-1) {
    perror("unable to open the audio device");
  }
  // Ok here something important if your programm forks:
  if (audio_fd > 0) {
    if (fcntl(audio_fd,F_SETFD,true) < 0) {
      perror("fcntl socket");exit(1);
    }
  }
  
  DB(audio, msg("Audio device opened on %d\n",audio_fd) );
  return (audio_fd > 0);
}

/* audioFlush() */
/* should flush the audio device */

inline void audioFlush() {
	DB(audio, msg("audio: flush %d\n",audio_fd) );
}


/* audioClose() */
/* should close the audio device and perform any special shutdown			 */

void audioClose() {
  close(audio_fd);
}


/**
   Audio init assumes that the audiodevice is open. It initializes
   it to the given values
*/

void audioInit(int sampleSize ,int frequency, int stereo,int sign, int big){

  if (ioctl(audio_fd,AUDIO_GETINFO,&auinfo)<0)
    perror("Unable to get audio info");
  
  auinfo.play.precision=sampleSize;
  auinfo.play.encoding=AUDIO_ENCODING_LINEAR;
  auinfo.play.channels=(stereo ? 2 : 1);
  DB(audio, msg("setting sample rate to %d Hz",frequency) );
  auinfo.play.sample_rate=frequency;
  if (ioctl(audio_fd,AUDIO_SETINFO,&auinfo)<0)
    perror("Unable to set audio info");

}

/* 
   only code this if your system can change the volume while 
   playing
*/


int getAudioBufferSize() {
 int buf;
 if (ioctl(audio_fd,AUDIO_GETINFO,&auinfo) == -1) {
   perror("ioctl getAudioBufferSize using default");
   buf=1024*65;
 } else {
   buf=auinfo.play.buffer_size;
 }
 return buf;
}


void mixerSetVolume(int leftVolume,int rightVolume) {
  int volume;

  volume=(leftVolume+rightVolume)/2;
  auinfo.play.gain=(volume*255)/100;
  
  // now normalize to values 0...32
  leftVolume=(32*leftVolume)/100;
  rightVolume=(32*rightVolume)/100;

  // eg : leftVolume=32, rightVolume=32 => balance=32
  // eg : leftVolume=0,  rightVolume=32 => balance=64
  // eg : leftVolume=32, rightVolume=0  => balance=0
  
  //cout << "leftVolume:"<<leftVolume<<endl;
  //cout << "rightVolume:"<<rightVolume<<endl;
  int balance=leftVolume-rightVolume+32;
  //cout << "balance:"<<balance<<endl;
  //someone should fix the volume on solaris
  balance=0;
  
  auinfo.play.balance=(uchar_t)balance;
  if (ioctl(audio_fd,AUDIO_SETINFO,&auinfo)<0)
    perror("Unable to set sound volume");
}




int mixerOpen() {
  return true;
}

void mixerClose() {
}




/* audioWrite */
/* writes count bytes from buffer to the audio device */
/* returns the number of bytes actually written */

int audioWrite(char *buffer, int count)
{
	return(write(audio_fd,buffer,count));
}

/* Let buffer.c have the audio descriptor so it can select on it. This means	*/
/* that the program is dependent on an file descriptor to work. Should really */
/* move the select's etc (with inlines of course) in here so that this is the */
/* ONLY file which has hardware dependent audio stuff in it										*/

int
getAudioFd()
{
	return(audio_fd);
}
