#include "noatun/conversion.h"

// inherit the aRts routines
#include <convert.h>

#include <config.h>
#include <limits.h>

namespace Conversion
{

void convertMono8ToFloat(unsigned long samples, unsigned char *from, float *to)
{
	Arts::convert_mono_8_float(samples, from, to);
}

void interpolateMono8ToFloat(unsigned long samples, double start, double speed,
                             unsigned char *from, float *to)
{
	Arts::interpolate_mono_8_float(samples, start, speed, from, to);
}

void convertMono16leToFloat(unsigned long samples, unsigned char *from, float *to)
{
	Arts::convert_mono_16le_float(samples, from, to);
}

void interpolateMono16leToFloat(unsigned long samples, double startpos, double speed,
                                unsigned char *from, float *to)
{
	Arts::interpolate_mono_16le_float(samples, startpos, speed, from, to);
}

void convertStereoI8To2Float(unsigned long samples, unsigned char *from,
                             float *left, float *right)
{
	Arts::convert_stereo_i8_2float(samples, from, left, right);
}


void interpolateStereoI8To2Float(unsigned long samples, double startpos, double speed,
                                 unsigned char *from, float *left, float *right)
{
	Arts::interpolate_stereo_i8_2float(samples, startpos, speed, from, left, right);
}

void convertStereoI16leTo2Float(unsigned long samples, unsigned char *from, float *left,
                                float *right)
{
	Arts::convert_stereo_i16le_2float(samples, from, left, right);
}

void interpolateStereoI16leTo2Float(unsigned long samples, double startpos, double speed,
                                    unsigned char *from, float *left, float *right)
{
	Arts::interpolate_stereo_i16le_2float(samples, startpos, speed, from, left, right);
}

void interpolateMonoFloatToFloat(unsigned long samples, double startpos, double speed,
                                 float *from, float *to)
{
	Arts::interpolate_mono_float_float( samples, startpos, speed,	from, to);
}

void convertStereoIFloatTo2Float(unsigned long samples,	float *from, float *left,
                                 float *right)
{
	Arts::convert_stereo_ifloat_2float(samples, from, left, right);
}

void interpolateStereoIFloatTo2Float(unsigned long samples, double startpos,
                                     double speed, float *from, float *left,
                                     float *right)
{
	Arts::interpolate_stereo_ifloat_2float(samples, startpos, speed, from, left, right);
}

void convertMonoFloatTo16le(unsigned long samples, float *from, unsigned char *to)
{
	Arts::convert_mono_float_16le(samples, from, to);
}

void convertStereo2FloatToI16le(unsigned long samples, float *left, float *right,
                                unsigned char *to)
{
	Arts::convert_stereo_2float_i16le(samples, left, right, to);
}

void convertMonoFloatTo8(unsigned long samples, float *from, unsigned char *to)
{
	Arts::convert_mono_float_8(samples, from, to);
}

void convertStereo2FloatToI8(unsigned long samples,	float *left, float *right,
                             unsigned char *to)
{
	Arts::convert_stereo_2float_i8(samples, left, right, to);
}

inline void toLittleEndian(unsigned long length, char *buffer)
{
#ifdef WORDS_BIGENDIAN
	swapEndian(length, buffer);
#else
	(void)length;
	(void)buffer;
#endif
}

inline void toBigEndian(unsigned long length, char *buffer)
{
#ifndef WORDS_BIGENDIAN
	swapEndian(length, buffer);
#else
	(void)length;
	(void)buffer;
#endif
}

void swapEndian(unsigned long length, char *buffer)
{
	// if you use a little-endian non intel box, and  the ASM
	// version doesn't work, it's safe to use the C version
#ifdef __i386__
	__asm__(
		"shrl $1,%0\n"
		"jz .l2\n"
		".l1:\n"
		"rolw $8,(%1)\n"
		"incl %1\n"
		"incl %1\n"
		"decl %0\n"
		"jnz .l1\n"
		".l2:\n"
		: : "r" (length), "r" (buffer));
#else
	while (length--)
	{
		register char c=*(buffer+1);
		*(buffer+1)=*buffer;
		*(buffer)=c;
		buffer++; buffer++;
		--length;
	}
#endif
}

}

