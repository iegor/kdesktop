/*
  reads/parse slice infos
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __SLICE_H
#define __SLICE_H

#include "jrevdct.h"
#include "mpegExtension.h"

/* Size increment of extension data buffers. */

#define EXT_BUF_SIZE 1024

class Slice {


 public:
  Slice();
  ~Slice();
  int parseSlice(MpegVideoStream* mpegVideoStream);
  unsigned int getVertPos();
  inline unsigned int getQuantScale() {return quant_scale;}
  void setQuantScale(unsigned int quant_scale);
  

 private:

  /* Slice structure. */

  unsigned int vert_pos;                 /* Vertical position of slice. */
  unsigned int quant_scale;              /* Quantization scale.         */
  MpegExtension* mpegExtension;          /* Extra bit slice info.       */

};


#endif
