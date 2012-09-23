/*
  class for motionvectors
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "motionVector.h"





MotionVector::MotionVector() {
}


MotionVector::~MotionVector() {
}



/*
 *--------------------------------------------------------------
 *
 * ComputeVector --
 *
 *	Computes motion vector given parameters previously parsed
 *      and reconstructed.
 *
 * Results:
 *      Reconstructed motion vector info is put into recon_* parameters
 *      passed to this function. Also updated previous motion vector
 *      information.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */
void MotionVector::computeVector(int* recon_right_ptr,
				 int* recon_down_ptr, 
				 int& recon_right_prev, 
				 int& recon_down_prev,
				 unsigned int& f, 
				 unsigned int& full_pel_vector, 
				 int& motion_h_code, 
				 int& motion_v_code, 
				 unsigned int& motion_h_r, 
				 unsigned int& motion_v_r) {


  int comp_h_r, comp_v_r;						
  int right_little, right_big, down_little, down_big;			
  int max, min, new_vector;						
									
  /* The following procedure for the reconstruction of motion vectors 	
     is a direct and simple implementation of the instructions given	
     in the mpeg December 1991 standard draft. 				
  */									
									
  if (f == 1 || motion_h_code == 0)					
    comp_h_r = 0;							
  else 									
    comp_h_r = f - 1 - motion_h_r;					
									
  if (f == 1 || motion_v_code == 0)					
    comp_v_r = 0;							
  else 									
    comp_v_r = f - 1 - motion_v_r;					
									
  right_little = motion_h_code * f;					
  if (right_little == 0)						
    right_big = 0;							
  else {								
    if (right_little > 0) {						
      right_little = right_little - comp_h_r;				
      right_big = right_little - 32 * f;				
    }									
    else {								
      right_little = right_little + comp_h_r;				
      right_big = right_little + 32 * f;				
    }									
  }									
							 		
  down_little = motion_v_code * f;					
  if (down_little == 0)							
    down_big = 0;							
  else {								
    if (down_little > 0) {						
      down_little = down_little - comp_v_r;				
      down_big = down_little - 32 * f;					
    }									
    else {								
      down_little = down_little + comp_v_r;				
      down_big = down_little + 32 * f;					
    }									
  }									
  									
  max = 16 * f - 1;							
  min = -16 * f;							
									
  new_vector = recon_right_prev + right_little;				
									
  if (new_vector <= max && new_vector >= min)				
    *recon_right_ptr = recon_right_prev + right_little;			
                      /* just new_vector */				
  else									
    *recon_right_ptr = recon_right_prev + right_big;			
  recon_right_prev = *recon_right_ptr;					
  if (full_pel_vector)							
    *recon_right_ptr = *recon_right_ptr << 1;				
									
  new_vector = recon_down_prev + down_little;				
  if (new_vector <= max && new_vector >= min)				
    *recon_down_ptr = recon_down_prev + down_little;			
                      /* just new_vector */				
  else									
    *recon_down_ptr = recon_down_prev + down_big;			
  recon_down_prev = *recon_down_ptr;					
  if (full_pel_vector)							
    *recon_down_ptr = *recon_down_ptr << 1;				

}
