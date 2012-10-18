
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "fft.h"

#define TRUE  1
#define FALSE 0

/*
 band pass filter for the Eq.  This is a modification of Kai Lassfolk's work, as
 removed from the Sound Processing Kit:

    Sound Processing Kit - A C++ Class Library for Audio Signal Processing
    Copyright (C) 1995-1998 Kai Lassfolk

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#define SAMPLERATE 44100

#ifndef M_PI
#define M_PI DDC_PI
#endif

void BandPassInit(struct BandPassInfo *ip, float center, float bw)
{
	ip->center = center;
	ip->bandwidth = bw;

	ip->C = 1.0 / tan(M_PI * bw / SAMPLERATE);
	ip->D = 2 * cos(2 * M_PI * center / SAMPLERATE);

	ip->a[0] = 1.0 / (1.0 + ip->C);
	ip->a[1] = 0.0;
	ip->a[2] = -ip->a[0];

	ip->b[0] = -ip->C * ip->D * ip->a[0];
	ip->b[1] = (ip->C - 1.0) * ip->a[0];

	ip->bufferX[0] = ip->bufferX[1] = 0.0;
	ip->bufferY[0] = ip->bufferY[1] = 0.0;
}
void BandPassSSE(struct BandPassInfo *ip, float *inbuffer, float *outbuffer, unsigned long samples)
{
#ifdef HAVE_X86_SSE
	__asm__(
		"testl      %0, %0                \n"
		"jz         .l5                   \n" /* if (!samples) */

		"movl       %1, %%ecx             \n"
		"movups     0x10(%%ecx), %%xmm2   \n" /* ip->a[0] */
		"shufps     $0x00, %%xmm2, %%xmm2 \n" /* ip->a[0] all over xmm3 */
		"movups     0x14(%%ecx), %%xmm4   \n" /* xmm4 = {ip->a[1], ip->a[2], ip->b} */
		"movups     0x24(%%ecx), %%xmm5   \n" /* xmm5 = {ip->bufferX, ip->bufferY} */
		"xorl       %%ecx, %%ecx          \n" /* i = 0 */
		"movl       $1, %%edx             \n" /* j = 1 */
		"prefetcht0 (%2)                  \n"
		".l1:                             \n"

		"decl       %%edx                 \n" /* --j */
		"jnz        .l4                   \n" /* if (j) */

		/* only load single values if less than four remain in inbuffer */
		"testl      $0xfffffffc, %0       \n"
		"jnz        .l2                   \n"
		"movss      (%2, %%ecx, 4), %%xmm3\n"
		"movl       $1, %%edx             \n"
		"jmp        .l3                   \n"
		".l2:                             \n"
		/* {inbuffer[i], inbuffer[i+1], inbuffer[i+2], inbuffer[i+3]} * ip->a[0] */
		"movups     (%2, %%ecx, 4), %%xmm3\n"
		"movl       $3, %%edx             \n" /* j = 3 */
		".l3:                             \n"
		"movaps     %%xmm3, %%xmm6        \n"
		"mulps      %%xmm2, %%xmm3        \n"
		".l4:                             \n"

		/* {ip->a[1], ip->a[2], ip->b} * {ip->bufferX, ip->bufferY} */
		"movaps     %%xmm4, %%xmm0        \n"
		"mulps      %%xmm5, %%xmm0        \n"
		"movaps     %%xmm0, %%xmm1        \n"
		/* xmm0 = {xmm0[0] + xmm0[1], <unused>, xmm0[2] + xmm0[3], <unused>} */
		"shufps     $0xb1, %%xmm0, %%xmm1 \n"
		"addps      %%xmm1, %%xmm0        \n"
		/* xmm0[0] -= xmm0[2] */
		"movhlps    %%xmm0, %%xmm1        \n"
		"subss      %%xmm1, %%xmm0        \n"
		"addss      %%xmm3, %%xmm0        \n" /* xmm0[0] += inbuffer[i] * ip->a[0] */
		"movss      %%xmm0, (%3, %%ecx, 4)\n" /* outbuffer[i] = xmm0[0] */

		/* xmm5 = {inbuffer[i], xmm5[0], outbuffer[i], xmm5[2]} */
		"shufps     $0x24, %%xmm5, %%xmm0 \n"
		"shufps     $0x81, %%xmm0, %%xmm5 \n"
		"movss      %%xmm6, %%xmm5        \n"

		/* right-shift xmm3 (inbuffer * ip->a[0]) and xmm6 (inbuffer) */
		"shufps     $0x39, %%xmm3, %%xmm3 \n"
		"shufps     $0x39, %%xmm6, %%xmm6 \n"

		"incl       %%ecx                 \n" /* ++i */
		"decl       %0                    \n"
		"jnz        .l1                   \n"

		"movl       %1,%%ecx              \n"
		"movups     %%xmm5, 0x24(%%ecx)   \n" /* {ip->bufferX, ip->bufferY} = xmm5 */
		"emms                             \n"
		".l5:                             \n"
	:
	: "r" (samples),  /* %0 */
	  "m" (ip),       /* %1 */
	  "r" (inbuffer), /* %2 */
	  "r" (outbuffer) /* %3 */
	: "ecx", "edx");
#endif
}

void BandPass(struct BandPassInfo *ip, float *inbuffer, float *outbuffer, unsigned long samples)
{
	unsigned long i;
	for (i=0; i<samples; ++i)
	{
	    outbuffer[i] = ip->a[0] * inbuffer[i] + ip->a[1] * ip->bufferX[0] + ip->a[2]
		               * ip->bufferX[1] - ip->b[0] * ip->bufferY[0] - ip->b[1]
					   * ip->bufferY[1];

		ip->bufferX[1] = ip->bufferX[0];
		ip->bufferX[0] = inbuffer[i];
		ip->bufferY[1] = ip->bufferY[0];
		ip->bufferY[0] = outbuffer[i];
	}
}


/*============================================================================

    fftmisc.c  -  Don Cross <dcross@intersrv.com>

    http://www.intersrv.com/~dcross/fft.html

    Helper routines for Fast Fourier Transform implementation.
    Contains common code for fft_float() and fft_double().

    See also:
        fourierf.c
        fourierd.c
        ..\include\fourier.h

    Revision history:

1998 September 19 [Don Cross]
    Improved the efficiency of IsPowerOfTwo().
    Updated coding standards.

============================================================================*/


#define BITS_PER_WORD   (sizeof(unsigned) * 8)


static int IsPowerOfTwo ( unsigned x )
{
    if ( x < 2 )
        return FALSE;

    if ( x & (x-1) )        /* Thanks to 'byang' for this cute trick! */
        return FALSE;

    return TRUE;
}


static unsigned NumberOfBitsNeeded ( unsigned PowerOfTwo )
{
    unsigned i;

    if ( PowerOfTwo < 2 )
    {
        fprintf (
            stderr,
            ">>> Error in fftmisc.c: argument %d to NumberOfBitsNeeded is too small.\n",
            PowerOfTwo );

        exit(1);
    }

    for ( i=0; ; i++ )
    {
        if ( PowerOfTwo & (1 << i) )
            return i;
    }
}



static unsigned ReverseBits ( unsigned ind, unsigned NumBits )
{
    unsigned i, rev;

    for ( i=rev=0; i < NumBits; i++ )
    {
        rev = (rev << 1) | (ind & 1);
        ind >>= 1;
    }

    return rev;
}

/*
static double Index_to_frequency ( unsigned NumSamples, unsigned Index )
{
    if ( Index >= NumSamples )
        return 0.0;
    else if ( Index <= NumSamples/2 )
        return (double)Index / (double)NumSamples;

    return -(double)(NumSamples-Index) / (double)NumSamples;
}
*/
#undef TRUE
#undef FALSE
#undef BITS_PER_WORD
/*============================================================================

    fourierf.c  -  Don Cross <dcross@intersrv.com>

    http://www.intersrv.com/~dcross/fft.html

    Contains definitions for doing Fourier transforms
    and inverse Fourier transforms.

    This module performs operations on arrays of 'float'.

    Revision history:

1998 September 19 [Don Cross]
    Updated coding standards.
    Improved efficiency of trig calculations.

============================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "fft.h"

#define CHECKPOINTER(p)  CheckPointer(p,#p)

static void CheckPointer ( const void *p, const char *name )
{
    if ( p == NULL )
    {
        fprintf ( stderr, "Error in fft_float():  %s == NULL\n", name );
        exit(1);
    }
}


void fft_float (
    unsigned  NumSamples,
    int       InverseTransform,
    float    *RealIn,
    float    *ImagIn,
    float    *RealOut,
    float    *ImagOut )
{
    unsigned NumBits;    /* Number of bits needed to store indices */
    unsigned i, j, k, n;
    unsigned BlockSize, BlockEnd;

    double angle_numerator = 2.0 * DDC_PI;
    double tr, ti;     /* temp real, temp imaginary */

    if ( !IsPowerOfTwo(NumSamples) )
    {
        fprintf (
            stderr,
            "Error in fft():  NumSamples=%u is not power of two\n",
            NumSamples );

        exit(1);
    }

    if ( InverseTransform )
        angle_numerator = -angle_numerator;

    CHECKPOINTER ( RealIn );
    CHECKPOINTER ( RealOut );
    CHECKPOINTER ( ImagOut );

    NumBits = NumberOfBitsNeeded ( NumSamples );

    /*
    **   Do simultaneous data copy and bit-reversal ordering into outputs...
    */

    for ( i=0; i < NumSamples; i++ )
    {
        j = ReverseBits ( i, NumBits );
        RealOut[j] = RealIn[i];
        ImagOut[j] = (ImagIn == NULL) ? 0.0 : ImagIn[i];
    }

    /*
    **   Do the FFT itself...
    */

    BlockEnd = 1;
    for ( BlockSize = 2; BlockSize <= NumSamples; BlockSize <<= 1 )
    {
        double delta_angle = angle_numerator / (double)BlockSize;
        double sm2 = sin ( -2 * delta_angle );
        double sm1 = sin ( -delta_angle );
        double cm2 = cos ( -2 * delta_angle );
        double cm1 = cos ( -delta_angle );
        double w = 2 * cm1;
        double ar[3], ai[3];

        for ( i=0; i < NumSamples; i += BlockSize )
        {
            ar[2] = cm2;
            ar[1] = cm1;

            ai[2] = sm2;
            ai[1] = sm1;

            for ( j=i, n=0; n < BlockEnd; j++, n++ )
            {
                ar[0] = w*ar[1] - ar[2];
                ar[2] = ar[1];
                ar[1] = ar[0];

                ai[0] = w*ai[1] - ai[2];
                ai[2] = ai[1];
                ai[1] = ai[0];

                k = j + BlockEnd;
                tr = ar[0]*RealOut[k] - ai[0]*ImagOut[k];
                ti = ar[0]*ImagOut[k] + ai[0]*RealOut[k];

                RealOut[k] = RealOut[j] - tr;
                ImagOut[k] = ImagOut[j] - ti;

                RealOut[j] += tr;
                ImagOut[j] += ti;
            }
        }

        BlockEnd = BlockSize;
    }

    /*
    **   Need to normalize if inverse transform...
    */

    if ( InverseTransform )
    {
        double denom = (double)NumSamples;

        for ( i=0; i < NumSamples; i++ )
        {
            RealOut[i] /= denom;
            ImagOut[i] /= denom;
        }
    }
}


