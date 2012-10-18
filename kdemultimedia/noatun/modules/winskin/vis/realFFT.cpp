/*
  a FFT class
  Copyright (C) 1998  Martin Vogt;Philip VanBaren

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "realFFT.h"

/*
 *  Initialize the Sine table and Twiddle pointers (bit-reversed pointers)
 *  for the FFT routine.
 */
RealFFT::RealFFT(int fftlen) {
   int i;
   int temp;
   int mask;

   /*
    *  FFT size is only half the number of data points
    *  The full FFT output can be reconstructed from this FFT's output.
    *  (This optimization can be made since the data is real.)
    */
   Points = fftlen;

   if((SinTable=(short *)malloc(Points*sizeof(short)))==NULL)
   {
      puts("Error allocating memory for Sine table.");
      exit(1);
	}
   if((BitReversed=(int *)malloc(Points/2*sizeof(int)))==NULL)
   {
      puts("Error allocating memory for BitReversed.");
      exit(1);
   }

   for(i=0;i<Points/2;i++)
   {
      temp=0;
      for(mask=Points/4;mask>0;mask >>= 1)
         temp=(temp >> 1) + (i&mask ? Points/2 : 0);

      BitReversed[i]=temp;
   }

   for(i=0;i<Points/2;i++)
	{
      register double s,c;
      s=floor(-32768.0*sin(2*M_PI*i/(Points))+0.5);
      c=floor(-32768.0*cos(2*M_PI*i/(Points))+0.5);
      if(s>32767.5) s=32767;
      if(c>32767.5) c=32767;
      SinTable[BitReversed[i]  ]=(short)s;
      SinTable[BitReversed[i]+1]=(short)c;
   }
  
}

/*
 *  Free up the memory allotted for Sin table and Twiddle Pointers
 */
RealFFT::~RealFFT() {
  free(BitReversed);
  free(SinTable);
  Points=0;
}


/*
 *  Actual FFT routine.  Must call InitializeFFT(fftlen) first!
 *  This routine has another parameter list than the other fft's
 *  But because we want a fast fft on pcm data this routine
 *  is better than the other two.
 *  The other two can be useful for inverse FFT.
 *  The format is an array of floats. (only real parts the img
 *  part does not exists)
 */
void RealFFT::fft(short* buffer) {
   int ButterfliesPerGroup=Points/4;

   endptr1=buffer+Points;

   /*
    *  Butterfly:
    *     Ain-----Aout
    *         \ /
    *         / \
    *     Bin-----Bout
    */

   while(ButterfliesPerGroup>0)
   {
      A=buffer;
      B=buffer+ButterfliesPerGroup*2;
      sptr=SinTable;

      while(A<endptr1)
      {
         register short sin=*sptr;
         register short cos=*(sptr+1);
         endptr2=B;
         while(A<endptr2)
         {
            long v1=((long)*B*cos + (long)*(B+1)*sin) >> 15;
            long v2=((long)*B*sin - (long)*(B+1)*cos) >> 15;
	    *B=(*A+v1)>>1;
            *(A++)=*(B++)-v1;
	    *B=(*A-v2)>>1;
            *(A++)=*(B++)+v2;
         }
         A=B;
         B+=ButterfliesPerGroup*2;
         sptr+=2;
      }
      ButterfliesPerGroup >>= 1;
   }
   /*
    *  Massage output to get the output for a real input sequence.
    */
   br1=BitReversed+1;
   br2=BitReversed+Points/2-1;

   while(br1<=br2)
   {
      register long temp1,temp2;
      short sin=SinTable[*br1];
      short cos=SinTable[*br1+1];
      A=buffer+*br1;
      B=buffer+*br2;
      HRplus = (HRminus = *A     - *B    ) + (*B << 1);
      HIplus = (HIminus = *(A+1) - *(B+1)) + (*(B+1) << 1);
      temp1  = ((long)sin*HRminus - (long)cos*HIplus) >> 15;
      temp2  = ((long)cos*HRminus + (long)sin*HIplus) >> 15;
      *B     = (*A     = (HRplus  + temp1) >> 1) - temp1;
      *(B+1) = (*(A+1) = (HIminus + temp2) >> 1) - HIminus;

      br1++;
      br2--;
   }
   /*
    *  Handle DC bin separately
    */
   buffer[0]+=buffer[1];
   buffer[1]=0;
}


int* RealFFT::getBitReversed() {
  return BitReversed;
}
