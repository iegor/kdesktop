/*
  definittion for reverse dct
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __JREVDCT_H
#define __JREVDCT_H

extern "C" {
#include <string.h>
}

#include "proto.h"


#ifndef XMD_H
typedef int INT32;
typedef short INT16;
typedef char INT8;
#endif
typedef unsigned int UINT32;
typedef unsigned short UINT16;
typedef unsigned char UINT8;
 


/* Definition of Contant integer scale factor. */

#define CONST_BITS 13

/* Misc DCT definitions */
#define DCTSIZE		8	/* The basic DCT block is 8x8 samples */
#define DCTSIZE2	64	/* DCTSIZE squared; # of elements in a block */

#define GLOBAL			/* a function referenced thru EXTERNs */
  
typedef short DCTELEM;
typedef DCTELEM DCTBLOCK[DCTSIZE2];
 

/* jrevdct.c */
void init_pre_idct (void);
void j_rev_dct_sparse (DCTBLOCK data , int pos);
void j_rev_dct (DCTBLOCK data);
void j_rev_dct_sparse (DCTBLOCK data , int pos);
void j_rev_dct (DCTBLOCK data);


#endif
