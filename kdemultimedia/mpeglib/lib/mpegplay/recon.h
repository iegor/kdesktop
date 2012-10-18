/*
  class for reconstruction
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __RECON_H
#define __RECON_H


#include <iostream>
#include "../util/render/pictureArray.h"
#include "copyFunctions.h"
#include "videoDecoder.h"

class Recon {


  CopyFunctions* copyFunctions;

 public:
  Recon();
  ~Recon();

  int ReconIMBlock(int bnum,int mb_row,int mb_col,int row_size,
		   short int* dct_start,PictureArray* pictureArray);

  int ReconPMBlock(int bnum,int recon_right_for,
		   int recon_down_for,int zflag,
		   int mb_row,int mb_col,int row_size,
		   short int* dct_start,PictureArray* pictureArray,
		   int codeType);

  int ReconBMBlock(int bnum,int recon_right_back,
		   int recon_down_back,int zflag,
		   int mb_row,int mb_col,int row_size,
		   short int* dct_start,PictureArray* pictureArray);
  
  int ReconBiMBlock(int bnum,int recon_right_for,
		    int recon_down_for,int recon_right_back,
		    int recon_down_back,int zflag,
		    int mb_row,int mb_col,int row_size,
		    short int* dct_start,PictureArray* pictureArray);


 
};
#endif
