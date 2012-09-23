/* 
 * tplay - buffered audio player
 *
 * (c) 1997 ilkka karvinen <ik@iki.fi>
 *
 * Copyright under the GNU GENERAL PUBLIC LICENSE
 *   (see the file COPYING in this directory)
 *
 * 
 *   RIFF/WAVE file header checking.
 *   check_wav returns zero if RIFF/WAVE file format is found.
 *   Reference: http://www.wotsit.demon.co.uk/formats/wav/wav.txt
 */

#include "tplayfunctions.h"

#include <iostream>

using namespace std;

int read_wav(struct info_struct* info, char * buffer) {
   WORD format, channels, bits;
   DWORD magic, samples_per_second, length, data_length;

   magic = read_little_endian_long(buffer);
   if (magic != RIFF_MAGIC)	/* RIFF file? */
      return (1);
   magic = read_little_endian_long(buffer + 0x08);
   if (magic != WAVE_MAGIC)	/* WAVE file? */
      return (1);
   magic = read_little_endian_long(buffer + 0x24);
   if ((magic != DATA_MAGIC) && (magic != INFO_MAGIC)) {	/* data-portion there? */
     cout << "Unknown WAV-header magic. Continuing anyway."<<endl;
   }

   length = read_little_endian_long(buffer + 0x10);

   /* Subchunk length should be 16 here */
   if (length != 16)
      errdie("Unknown RIFF/WAVE header");

   format = read_little_endian_word(buffer + 0x14);

   switch (format) {
   case 0x0001:		/* PCM format */
      break;
   case 0x0101:		/* mu-law */
      die("Mu-law RIFF/WAVE audio file not supported");
      break;
   case 0x0102:		/* a-law */
      die("A-law RIFF/WAVE audio file not supported");
      break;
   case 0x0103:		/* ADPCM */
      die("ADPCM RIFF/WAVE audio file not supported");
      break;
   default:
      errdie("Unknown RIFF/WAVE audio file format");
      break;
   }

   info->filetype = RIFF_FILE;

   channels = read_little_endian_word(buffer + 0x16);
   samples_per_second = read_little_endian_long(buffer + 0x18);
   cout << "samples_per_second:"<<samples_per_second<<endl;
   bits = read_little_endian_word(buffer + 0x22);

   if (bits == 12)
      die("12 bits per sample not supported");

   data_length = read_little_endian_long(buffer + 0x28);

   /* Set audio parameters */
   info->speed = (int) samples_per_second;
   info->bits = (int) bits;
   info->channels = (int) channels;
   
   if (info->verbose)
      printf("RIFF/WAVE audio file: %ld samples/s, %d bits, %d channel(s).\n",
	     info->speed, info->bits, info->channels);

   /* Move data to start from the beginning of the buffer.            */
   /* This is to ensure the correct behaviour of rounding when 16bits */
   /* and/or stereo sample is to be played.                           */
   memmove(buffer, buffer + 0x2c, info->blocksize - 0x2c);

   /* Save audio sample starting point */
   info->headerskip = 0x2c;

   return (0);
}
