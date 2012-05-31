

#ifndef __PROTO_H
#define __PROTO_H




#include "videoDecoder.h"
#include "mpegVideoHeader.h"



/* decoders.c */
void init_tables (void);
extern "C" void Fast16Dither(unsigned char *lum, 
			     unsigned char *cr,
			     unsigned char *cb,
			     unsigned char *out,
			     int rows, 
			     int cols, 
			     int mod);


/* floatdct.c */
void init_float_idct (void);
void float_idct (short* block);
extern "C" void IDCT_mmx(short *);






#endif

