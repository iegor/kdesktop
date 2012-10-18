/* 
 * tplay - buffered audio player
 *
 * (c) 1997 ilkka karvinen <ik@iki.fi>
 *
 * Copyright under the GNU GENERAL PUBLIC LICENSE
 *   (see the file COPYING in this directory)
 *
 * 
 *   SunOS audio file header functions.
 *   Reference: http://www.wotsit.org
 */

#include "tplayfunctions.h"

/* read_au returns zero if Sun audio file format is found. */
int read_au(struct info_struct* info,char * buffer) {
   DWORD magic, start, end, encoding, speed, channels;
   int bits;


   /* If '.snd'-header exits, this should be an au-file */
   magic = read_big_endian_long(buffer);
   if (magic != SUN_MAGIC)
      return (1);

   start = read_big_endian_long(buffer + 0x04);
   end = read_big_endian_long(buffer + 0x08);
   encoding = read_big_endian_long(buffer + 0x0C);
   speed = read_big_endian_long(buffer + 0x10);
   channels = read_big_endian_long(buffer + 0x14);

#ifdef DEBUG
   printf("Sun audio file.\nspeed: %ld, start: %ld, end: %ld, \
encoding: %X, channels: %ld\n",
	  speed, start, end, encoding, channels);
   fflush(stdout);
#endif

   bits = DEFAULT_BITS;
   switch (encoding) {
   case 1:
      die("8-bit ISDN u-law Sun audio file not supported");
      break;
   case 2:
      bits = 8;
      break;
   case 3:
      bits = 16;
      break;
   case 4:
      die("24-bit linear PCM Sun audio file not supported");
      break;
   case 5:
      die("32-bit linear PCM Sun audio file not supported");
      break;
   case 6:
      die("32-bit IEEE floating point Sun audio file not supported");
      break;
   case 7:
      die("64-bit IEEE floating point Sun audio file not supported");
      break;
   case 23:
      die("8-bit ISDN u-law compressed(G.721 ADPCM) Sun audio file \
not supported");
      break;
   default:
      errdie("Unknown Sun audio file");
      break;
   }

   info->filetype = SUN_FILE;

   /* Set audio parameters */
   info->speed = (int) speed;
   info->bits = bits;
   info->channels = (int) channels;

   if (info->verbose) {
      printf("Sun audio file: %ld samples/s, %d bits, %d channel(s).\n",
	     info->speed, info->bits, info->channels);
      /*
      if ((comment_size = start - SUN_HDRSIZE) > 0) {
	 printf("Header info: ");
	 for (i = 0; i < comment_size; i++)
	    nice_fputc((int) buffer[SUN_HDRSIZE + i], stdout);
	 printf("\n");
      }
      */
   }

   /* Move data to start from the beginning of the buffer.            */
   /* This is to ensure the correct behaviour of rounding when 16bits */
   /* and/or stereo sample is to be played.                           */
   memmove(buffer, buffer + (start + 1), info->blocksize - start - 1);

   info->headerskip = (int) (start + 1);

   return (0);
}
