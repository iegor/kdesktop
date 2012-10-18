/*
  declares various tables to make things faster
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __DECODERTABLES_H
#define __DECODERTABLES_H

#include "videoDecoder.h"
#include "proto.h"

/* Code for unbound values in decoding tables */
#define MPGDECODE_ERROR (-1)

#define MACRO_BLOCK_STUFFING 34
#define MACRO_BLOCK_ESCAPE 35


/* Structure for an entry in the decoding table of 
 * macroblock_address_increment */
typedef struct {
  int value;       /* value for macroblock_address_increment */
  int num_bits;             /* length of the Huffman code */
} mb_addr_inc_entry;


/* Structure for an entry in the decoding table of macroblock_type */
typedef struct {
  unsigned int mb_quant;              /* macroblock_quant */
  unsigned int mb_motion_forward;     /* macroblock_motion_forward */
  unsigned int mb_motion_backward;    /* macroblock_motion_backward */
  unsigned int mb_pattern;            /* macroblock_pattern */
  unsigned int mb_intra;              /* macroblock_intra */
  unsigned int num_bits;              /* length of the Huffman code */
} mb_type_entry;


/* Structures for an entry in the decoding table of coded_block_pattern */
typedef struct {
  unsigned int cbp;            /* coded_block_pattern */
  int num_bits;                /* length of the Huffman code */
} coded_block_pattern_entry;


/* Structure for an entry in the decoding table of motion vectors */
typedef struct {
  int code;              /* value for motion_horizontal_forward_code,
			  * motion_vertical_forward_code, 
			  * motion_horizontal_backward_code, or
			  * motion_vertical_backward_code.
			  */
  int num_bits;          /* length of the Huffman code */
} motion_vectors_entry;


/* Structure for an entry in the decoding table of dct_dc_size */
typedef struct {
  unsigned int value;    /* value of dct_dc_size (luminance or chrominance) */
  int num_bits;          /* length of the Huffman code */
} dct_dc_size_entry;



/* External declaration of dct coeff tables. */

extern unsigned short int dct_coeff_tbl_0[256];
extern unsigned short int dct_coeff_tbl_1[16];
extern unsigned short int dct_coeff_tbl_2[4];
extern unsigned short int dct_coeff_tbl_3[4];
extern unsigned short int dct_coeff_next[256];
extern unsigned short int dct_coeff_first[256];

/* External declaration of dct dc size lumiance table. */

extern dct_dc_size_entry dct_dc_size_luminance[32];
extern dct_dc_size_entry dct_dc_size_luminance1[16];

/* External declaration of dct dc size chrom table. */

extern dct_dc_size_entry dct_dc_size_chrominance[32];
extern dct_dc_size_entry dct_dc_size_chrominance1[32];

/* External declaration of coded block pattern table. */

extern coded_block_pattern_entry coded_block_pattern[512];

/* Decoding table for macroblock_type in predictive-coded pictures */
extern mb_type_entry mb_type_P[64];

/* Decoding table for macroblock_type in bidirectionally-coded pictures */
extern mb_type_entry mb_type_B[64];

/* Decoding table for macroblock_address_increment */
extern mb_addr_inc_entry mb_addr_inc[2048];

/* Decoding table for motion vectors */
extern motion_vectors_entry motion_vectors[2048];

extern void init_tables();

#endif
