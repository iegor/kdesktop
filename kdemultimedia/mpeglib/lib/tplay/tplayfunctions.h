/* 
 * tplay - buffered audio player header file
 *
 * (c) 1997 ilkka karvinen <ik@iki.fi>
 *
 * Copyright under the GNU GENERAL PUBLIC LICENSE
 *   (see the file COPYING in this directory)
 *
 */

#ifndef __TPLAYCONTROL_H
#define __TPLAYCONTROL_H

extern "C" {
#include <stdio.h>
#include <string.h>
}

/* tplay version */
#define MAJOR_VERSION 0
#define MINOR_VERSION 5
#define PATCHLEVEL    5

/* Default audio parameters */
#define DEFAULT_BITS     16
#define DEFAULT_SPEED    44100
#define DEFAULT_CHANNELS 2

/* Audio memory pool. 512k is the default. */
#define BUFFER_SIZE    0x80000

/* The minimum and maximum buffer block sizes. */
#if 0
#define MIN_BLOCK_SIZE 0x4000	/* 16k */
#else
#define MIN_BLOCK_SIZE 4096
#endif
#define MAX_BLOCK_SIZE 0x10000	/* 64k */

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif

/* The maximum retry count for buffer fill tries. */
#define RETRY_COUNT 5

/* Magics. Little-endian. */
#define RIFF_MAGIC    0x46464952	/* ASCII: 'RIFF' */
#define WAVE_MAGIC    0x45564157	/* ASCII: 'WAVE' */
#define DATA_MAGIC    0x61746164	/* ASCII: 'data' */
#define INFO_MAGIC    0x4f464e49	/* ASCII: 'INFO' */
#define SUN_MAGIC     0x2e736e64	/* ASCII: '.snd' */

/* Magics. Big-endian. */
#define SUN_INV_MAGIC 0x646e732e	/* ASCII: '.snd' */

/* Sun headersize */
#define SUN_HDRSIZE 24

/* File types */
#define UNKNOWN_FILE  0
#define RIFF_FILE     1
#define SUN_FILE      2

typedef unsigned long DWORD;
typedef unsigned short WORD;

/* Circular buffer info structure of audio data blocks. */
/* declared in tplay.c                                  */
struct info_struct {
   char *buffer;		/* the audio data */
   char *firstblock;		/* pointer to the first block */
   int readblock, writeblock;	/* reading and writing block number */
   long readcount, writecount;
   int alldone;
   int in_seconds;
   double seconds;
   int blocksize;		/* size of one block */
   int buffer_size;		/* size of the buffer */
   int number_of_blocks;
   int last_block;		/* -1 if not the last block */
   int bytes_on_last_block;
   int overflow;
   int underflow;
   int swap;
   int forceraw;
   int force;
   int filetype;
   int headerskip;
   int audioset;
   int show_usage;
   DWORD speed;
   int channels;
   int bits;
   char *progname;
   char *device;		/* Audio device name */
   int loop;
   int verbose;
   int optind;
};

/* au.c */
 int read_au(struct info_struct* info, char * buffer);

/* wav.c */
 int read_wav(struct info_struct* info, char * buffer);

/* common.c */
 DWORD read_big_endian_long(char * buffer);
 void write_big_endian_long(char * buffer, DWORD value);
 DWORD read_little_endian_long(char * buffer);
 WORD read_little_endian_word(char * buffer);
 void errprintf(char *fmt,...);
 void warning(char *str);
 void warning2(char *str1, char *str2);
 void die(const char *str);
 void errdie(const char *str);
 void open_audio();
 void set_audio_parameters();
 void sync_audio(void);
 void reset_audio(void);
 void post_audio(void);
 void destroy_buffer(void);
 void nice_fputc(int c, FILE * fp);


#endif
