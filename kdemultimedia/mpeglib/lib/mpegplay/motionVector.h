/*
  class for motionvectors
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __MOTIONVECTOR_H
#define __MOTIONVECTOR_H


#include "picture.h"


class MotionVector {


 public:
  MotionVector();
  ~MotionVector();

  void computeVector(int* recon_right_ptr,
		     int* recon_down_ptr, 
		     int& recon_right_prev, 
		     int& recon_down_prev,
		     unsigned int& f, 
		     unsigned int& full_pel_vector, 
		     int& motion_h_code, 
		     int& motion_v_code, 
		     unsigned int& motion_h_r,
		     unsigned int& motion_v_r);

};
#endif
