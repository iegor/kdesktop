/*
 * AIX audio - griff@acm.org 02aug2000
 * tested on 43P 260 with builtin audio
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
/* A conflict within AIX 4.3.3 <sys/> headers and probably others as well.
 * I guess nobody ever uses audio... Shame over AIX header files.  */
#include <sys/machine.h>
#undef BIG_ENDIAN
#include <sys/audio.h>

static int audio_fd;

static void debugUpdate( unsigned long& flags, long& bsize );

#ifndef AUDIO_BIG_ENDIAN 
#define AUDIO_BIG_ENDIAN BIG_ENDIAN
#endif



int audioConstruct() {
  printf("audioConstruct AIX ********\n");
  audio_fd=-1;
  return true;
}


void audioDestruct() {
  
}

int audioOpen()
{
    char devname[14];
    for ( int dev=0; dev<4; dev++ )
    {
	for ( int chan=1; chan<8; chan++ )
	{
	    sprintf(devname,"/dev/paud%d/%d",dev,chan);
	    audio_fd = open (devname, O_WRONLY, 0);
	    if ( audio_fd >= 0 )
	    {
		return 1;
	    }
	    sprintf(devname,"/dev/baud%d/%d",dev,chan);
	    audio_fd = open (devname, O_WRONLY, 0);
	    if ( audio_fd >= 0 )
	    {
		return 1;
	    }
	}
    }

    fprintf(stderr, "Could not open AIX audio device, faking\n" );
    return 1;
}

int getAudioBufferSize()
{
    audio_buffer  paud_bufinfo;

    if( audio_fd < 0 ) return 1024*65;

    if ( ioctl(audio_fd, AUDIO_BUFFER, &paud_bufinfo) < 0 )
    {
	perror("ioctl getAudioBufferSize using default");
        return 1024*65;
    }

    /*
     * Do you need the total capacity or the current capacity?
     * This is the total capacity:
     */
    return paud_bufinfo.write_buf_cap;
    /*
     * This is the current capacity:
     * return (paud_bufinfo.write_buf_cap - paud_bufinfo.write_buf_size);
     */
}

void audioInit(int sampleSize,int frequency, int stereo, int sign, int bigendian )
{
    // int           format;
    int           bytes_per_sample;
    audio_init    paud_init;
    audio_buffer  paud_bufinfo;
    // audio_status  paud_status;
    audio_control paud_control;
    audio_change  paud_change;

    if( audio_fd < 0 ) return;

    /*
     * We can't set the buffer size - just ask the device for the maximum
     * that we can have.
     */
    if ( ioctl(audio_fd, AUDIO_BUFFER, &paud_bufinfo) < 0 )
    {
        perror("Couldn't get audio buffer information");
        return;
    }

    /*
     * Fields in the audio_init structure:
     *
     * Ignored by us:
     *
     * paud.loadpath[LOAD_PATH]; * DSP code to load, MWave chip only?
     * paud.slot_number;         * slot number of the adapter
     * paud.device_id;           * adapter identification number
     *
     * Input:
     *
     * paud.srate;           * the sampling rate in Hz
     * paud.bits_per_sample; * 8, 16, 32, ...
     * paud.bsize;           * block size for this rate
     * paud.mode;            * ADPCM, PCM, MU_LAW, A_LAW, SOURCE_MIX
     * paud.channels;        * 1=mono, 2=stereo
     * paud.flags;           * FIXED - fixed length data
     *                       * LEFT_ALIGNED, RIGHT_ALIGNED (var len only)
     *                       * TWOS_COMPLEMENT - 2's complement data
     *                       * SIGNED - signed? comment seems wrong in sys/audio.h
     *                       * BIG_ENDIAN
     * paud.operation;       * PLAY, RECORD
     *
     * Output:
     *
     * paud.flags;           * PITCH            - pitch is supported
     *                       * INPUT            - input is supported
     *                       * OUTPUT           - output is supported
     *                       * MONITOR          - monitor is supported
     *                       * VOLUME           - volume is supported
     *                       * VOLUME_DELAY     - volume delay is supported
     *                       * BALANCE          - balance is supported
     *                       * BALANCE_DELAY    - balance delay is supported
     *                       * TREBLE           - treble control is supported
     *                       * BASS             - bass control is supported
     *                       * BESTFIT_PROVIDED - best fit returned
     *                       * LOAD_CODE        - DSP load needed
     * paud.rc;              * NO_PLAY         - DSP code can't do play requests
     *                       * NO_RECORD       - DSP code can't do record requests
     *                       * INVALID_REQUEST - request was invalid
     *                       * CONFLICT        - conflict with open's flags
     *                       * OVERLOADED      - out of DSP MIPS or memory
     * paud.position_resolution; * smallest increment for position
     */

    paud_init.srate = frequency;
    paud_init.mode = PCM;
    paud_init.operation = PLAY;
    paud_init.channels = (stereo?2:1);

    /*
     * options in AIX:
     * paud_init.bits_per_sample: 8 | 16
     * paud_init.flags: AUDIO_BIG_ENDIAN (not used here)
     *                  SIGNED (always used here)
     *                  TWOS_COMPLEMENT (always on for Linux dsp porting?)
     *                  FIXED           <- that's right for SDL
     *                  or LEFT_ALIGNED <- that's right for mpeglib
     *                  or RIGHT_ALIGNED
     * paud_init.bsize: sample byte size,
     *                  bits_per_sample * (stereo?2:1)     - for SDL
     *                  bits_per_sample * (stereo?2:1) * 2 - for mpeglib
     */
    if ( sampleSize == 8 )
    {
        /* AFMT_S8 in linux dsp */
        bytes_per_sample = 2; // why not 1 ?
        paud_init.bits_per_sample = 8;
        paud_init.flags = TWOS_COMPLEMENT | LEFT_ALIGNED;
    }
    else
    {
        /* AFMT_S16_LE in linux dsp */
        bytes_per_sample = 4; // why not 2 ?
        paud_init.bits_per_sample = 16;
        paud_init.flags = TWOS_COMPLEMENT | LEFT_ALIGNED;
    }
    if( sign )      paud_init.flags |= SIGNED;
    if( bigendian ) paud_init.flags |= AUDIO_BIG_ENDIAN;

    paud_init.bsize = bytes_per_sample * (stereo?2:1);

#if 0
    debugUpdate(paud_init.flags, paud_init.bsize);

    printf("CG: sampleSize              = %d\n", sampleSize);
    printf("CG: frequency               = %d\n", frequency);
    printf("CG: stereo                  = %s\n", (stereo)?"y":"n");
    printf("CG: mode                    = %s\n", "PCM");
    printf("CG: channels                = %d\n", paud_init.channels);
    printf("CG: bsize                   = %d\n", paud_init.bsize);
    printf("CG: bits_per_sample         = %d\n", paud_init.bits_per_sample);
    printf("CG: flags & BIG_ENDIAN      = %s\n", ((paud_init.flags&AUDIO_BIG_ENDIAN)?"y":"n"));
    printf("CG: flags & SIGNED          = %s\n", ((paud_init.flags&SIGNED)?"y":"n"));
    printf("CG: flags & TWOS_COMPLEMENT = %s\n", ((paud_init.flags&TWOS_COMPLEMENT)?"y":"n"));
    printf("CG: flags & FIXED           = %s\n", ((paud_init.flags&FIXED)?"y":"n"));
    printf("CG: flags & LEFT_ALIGNED    = %s\n", ((paud_init.flags&LEFT_ALIGNED)?"y":"n"));
    printf("CG: flags & RIGHT_ALIGNED   = %s\n", ((paud_init.flags&RIGHT_ALIGNED)?"y":"n"));
#endif

    /*
     * We know the buffer size and the max number of subsequent writes
     * that can be pending. If more than one can pend, allow the application
     * to do something like double buffering between our write buffer and
     * the device's own buffer that we are filling with write() anyway.
     *
     * We can calculate the number of samples that fit into the audio
     * device buffer if that is necessary:
     *
     * samples_capacity = paud_bufinfo.write_buf_cap
     *                  / bytes_per_sample
     *                  / (stereo?2:1);
     * if ( paud_bufinfo.request_buf_cap != 1 ) samples_capacity /= 2;
     */

    /*
     * The AIX paud device init can't modify the values of the audio_init
     * structure that we pass to it. So we don't need any recalculation
     * of this stuff and no reinit call as in linux SDL dsp and dma code.
     *
     * /dev/paud supports all of the encoding formats, so we don't need
     * to do anything like reopening the device, either.
     */
    if ( ioctl(audio_fd, AUDIO_INIT, &paud_init) < 0 )
    {
        switch ( paud_init.rc )
        {
        case 1 :
            perror("Couldn't set audio format: DSP can't do play requests");
            return;
            break;
        case 2 :
            perror("Couldn't set audio format: DSP can't do record requests");
            return;
            break;
        case 4 :
            perror("Couldn't set audio format: request was invalid");
            return;
            break;
        case 5 :
            perror("Couldn't set audio format: conflict with open's flags");
            return;
            break;
        case 6 :
            perror("Couldn't set audio format: out of DSP MIPS or memory");
            return;
            break;
        default :
            perror("Couldn't set audio format: not documented in sys/audio.h");
            return;
            break;
        }
    }

    /*
     * Set some parameters: full volume, first speaker that we can find.
     * Ignore the other settings for now.
     */
    paud_change.input         = AUDIO_IGNORE; /* the new input source */
    paud_change.output        = OUTPUT_1;
					      /* EXTERNAL_SPEAKER,
                                               * INTERNAL_SPEAKER,
                                               * OUTPUT_1 */
    paud_change.monitor       = AUDIO_IGNORE; /* the new monitor state */
    paud_change.volume        = 0x7fffffff;   /* volume level [0-0x7fffffff] */
    paud_change.volume_delay  = AUDIO_IGNORE; /* the new volume delay */
    paud_change.balance       = 0x3fffffff;   /* the new balance */
    paud_change.balance_delay = AUDIO_IGNORE; /* the new balance delay */
    paud_change.treble        = AUDIO_IGNORE; /* the new treble state */
    paud_change.bass          = AUDIO_IGNORE; /* the new bass state */
    paud_change.pitch         = AUDIO_IGNORE; /* the new pitch state */

    paud_control.ioctl_request = AUDIO_CHANGE;
    paud_control.request_info  = (char*)&paud_change;
    if ( ioctl(audio_fd, AUDIO_CONTROL, &paud_control) < 0 )
    {
        perror("Can't change audio display settings (ignoring)" );
    }

    /*
     * Tell the device to expect data. Actual start will wait for
     * the first write() call.
     */
    paud_control.ioctl_request = AUDIO_START;
    paud_control.position = 0;
    if ( ioctl(audio_fd, AUDIO_CONTROL, &paud_control) < 0 )
    {
        perror("Can't start audio play");
        return;
    }
}


void audioSetVolume(int volume)
{
    long vol = (long)(volume/100.0) * 0x7fffffff;
    if( audio_fd < 0 ) return;

    audio_control paud_control;
    audio_change  paud_change;

    paud_change.input          = AUDIO_IGNORE; /* the new input source */
    paud_change.output         = OUTPUT_1;     /* EXTERNAL_SPEAKER,INTERNAL_SPEAKER,
                                                  OUTPUT_1 */
    paud_change.monitor        = AUDIO_IGNORE; /* the new monitor state */
    paud_change.volume         = vol;          /* volume level [0-0x7fffffff] */
    paud_change.volume_delay   = AUDIO_IGNORE; /* the new volume delay */
    paud_change.balance        = AUDIO_IGNORE; /* the new balance */
    paud_change.balance_delay  = AUDIO_IGNORE; /* the new balance delay */
    paud_change.treble         = AUDIO_IGNORE; /* the new treble state */
    paud_change.bass           = AUDIO_IGNORE; /* the new bass state */
    paud_change.pitch          = AUDIO_IGNORE; /* the new pitch state */

    paud_control.ioctl_request = AUDIO_CHANGE;
    paud_control.request_info  = (char*)&paud_change;

    if ( ioctl(audio_fd, AUDIO_CONTROL, &paud_control) < 0 )
    {
	perror("Change audio volume failed");
    }
}

void audioFlush()
{
    if( audio_fd < 0 ) return;

    if ( ioctl(audio_fd, AUDIO_WAIT, NULL) < 0 )
    {
	perror("Flush audio buffers failed");
    }
}

void audioClose()
{
    if( audio_fd < 0 ) return;

    if ( ioctl(audio_fd, AUDIO_WAIT, NULL) < 0 )
    {
	perror("Flush audio buffers failed");
    }
    close(audio_fd);
}

int audioWrite(char *buffer, int count)
{
    int written = write(audio_fd, buffer, count);
    if( written < count )
    {
        return count;
    }

    return written;
}

int
getAudioFd()
{
    return audio_fd;
}

int mixerOpen()
{
  return true;
}

void mixerClose()
{
}

void mixerSetVolume(int leftVolume,int rightVolume)
{
    long balance;

    if( audio_fd < 0 ) return;

    balance = 2 * (leftVolume-rightVolume) / (leftVolume+rightVolume);
    balance = 0x3fffffff + balance*0x3fffffff;

    audio_control paud_control;
    audio_change  paud_change;

    paud_change.input = AUDIO_IGNORE;         /* the new input source */
    paud_change.output = OUTPUT_1;            /* EXTERNAL_SPEAKER,INTERNAL_SPEAKER,
						 OUTPUT_1 */
    paud_change.monitor = AUDIO_IGNORE;       /* the new monitor state */
    paud_change.volume = AUDIO_IGNORE;        /* volume level [0-0x7fffffff] */
    paud_change.volume_delay = AUDIO_IGNORE;  /* the new volume delay */
    paud_change.balance = balance;            /* the new balance */
    paud_change.balance_delay = AUDIO_IGNORE; /* the new balance delay */
    paud_change.treble = AUDIO_IGNORE;        /* the new treble state */
    paud_change.bass = AUDIO_IGNORE;          /* the new bass state */
    paud_change.pitch = AUDIO_IGNORE;         /* the new pitch state */

    paud_control.ioctl_request = AUDIO_CHANGE;
    paud_control.request_info = (char*)&paud_change;

    if ( ioctl(audio_fd, AUDIO_CONTROL, &paud_control) < 0 )
    {
	perror("Change audio volume failed");
    }
}

static void debugUpdate( unsigned long& flags, long& bsize )
{
    const char* g;

    g = getenv("AUDIO_BIG_ENDIAN");
    if ( g )
    {
	int i = atoi(g);
	if ( i == 1 )
	{
	    flags |= AUDIO_BIG_ENDIAN;
	}
	else if ( i == 0 )
	{
	    flags &= ~AUDIO_BIG_ENDIAN;
	}
	else
	{
	    printf("CG: bad AUDIO_BIG_ENDIAN env variable %s\n", g);
	}
    }

    g = getenv("SIGNED");
    if ( g )
    {
	int i = atoi(g);
	if ( i == 1 )
	{
	    flags |= SIGNED;
	}
	else if ( i == 0 )
	{
	    flags &= ~SIGNED;
	}
	else
	{
	    printf("CG: bad SIGNED env variable %s\n", g);
	}
    }

    g = getenv("TWOS_COMPLEMENT");
    if ( g )
    {
	int i = atoi(g);
	if ( i == 1 )
	{
	    flags |= TWOS_COMPLEMENT;
	}
	else if ( i == 0 )
	{
	    flags &= ~TWOS_COMPLEMENT;
	}
	else
	{
	    printf("CG: bad TWOS_COMPLEMENT env variable %s\n", g);
	}
    }

    g = getenv("FIXED");
    if ( g )
    {
	int i = atoi(g);
	if ( i == 1 )
	{
	    flags |= FIXED;
	}
	else if ( i == 0 )
	{
	    flags &= ~FIXED;
	}
	else
	{
	    printf("CG: bad FIXED env variable %s\n", g);
	}
    }

    g = getenv("LEFT_ALIGNED");
    if ( g )
    {
	int i = atoi(g);
	if ( i == 1 )
	{
	    flags |= LEFT_ALIGNED;
	}
	else if ( i == 0 )
	{
	    flags &= ~LEFT_ALIGNED;
	}
	else
	{
	    printf("CG: bad LEFT_ALIGNED env variable %s\n", g);
	}
    }

    g = getenv("RIGHT_ALIGNED");
    if ( g )
    {
	int i = atoi(g);
	if ( i == 1 )
	{
	    flags |= RIGHT_ALIGNED;
	}
	else if ( i == 0 )
	{
	    flags &= ~RIGHT_ALIGNED;
	}
	else
	{
	    printf("CG: bad RIGHT_ALIGNED env variable %s\n", g);
	}
    }

    g = getenv("BSIZE");
    if ( g )
    {
	bsize = atoi(g);
    }
}

