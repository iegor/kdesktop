#ifndef NOATUN_CONVERT_H
#define NOATUN_CONVERT_H

/**
 * Helper functions to convert between sample types
 * @short sample type conversion
 **/
namespace Conversion
{
/**
 * Convert a mono 8 bit group to float
 **/
void convertMono8ToFloat(unsigned long samples, unsigned char *from, float *to);

/**
 * convert a mono 8 bit group to float, at a different speed
 **/
void interpolateMono8ToFloat(unsigned long samples, double start, double speed,
                             unsigned char *from, float *to);

/**
 * convert a mono 16 bit little-endian stream to float
 **/
void convertMono16leToFloat(unsigned long samples, unsigned char *from, float *to);

/**
 * convert a mono 16 bit little-endian stream to float at a different speed
 **/
void interpolateMono16leToFloat(unsigned long samples, double startpos, double speed,
                                unsigned char *from, float *to);

/**
 * convert an stereo 8-bit interleaved (Alternating left/right channel) to floats
 **/
void convertStereoI8To2Float(unsigned long samples, unsigned char *from,
                             float *left, float *right);

/**
 * convert a stereo 8-bit interleaved (alternating left/right channel) to floats,
 * at a different speed
 **/
void interpolateStereoI8To2Float(unsigned long samples, double startpos, double speed,
                                 unsigned char *from, float *left, float *right);
/**
 * convert an interleaved 16 bit little endian stream to two floats
 **/
void convertStereoI16leTo2Float(unsigned long samples, unsigned char *from, float *left,
                                float *right);

/**
 * convert an interleaved 16 bit little endian stream to two floats at a different
 * speed
 **/
void interpolateStereoI16leTo2Float(unsigned long samples, double startpos, double speed,
                                    unsigned char *from, float *left, float *right);

/**
 * convert a float to a float, but at a different speed
 **/
void interpolateMonoFloatToFloat(unsigned long samples, double startpos, double speed,
                                 float *from, float *to);

/**
 * convert a stereo interleaved float to two floats
 **/
void convertStereoIFloatTo2Float(unsigned long samples,	float *from, float *left,
                                 float *right);

/**
 * convert a stereo interleaved float to two floats at a different speed
 **/
void interpolateStereoIFloatTo2Float(unsigned long samples, double startpos,
                                     double speed, float *from, float *left,
                                     float *right);

/**
 * convert a mono float to a 16 bit little endian float
 **/
void convertMonoFloatTo16le(unsigned long samples, float *from, unsigned char *to);

/**
 * convert a two floats to a 16 bit little endian interleaved float
 **/
void convertStereo2FloatToI16le(unsigned long samples, float *left, float *right,
                                unsigned char *to);

/**
 * convert a mono float to an 8 bit stream
 **/
void convertMonoFloatTo8(unsigned long samples, float *from, unsigned char *to);

/**
 * convert two floats to an 8 bit interleaved stream
 **/
void convertStereo2FloatToI8(unsigned long samples,	float *left, float *right,
                             unsigned char *to);

/**
 * to little endian (Intel) with swapEndian
 * does nothing if this is an intel
 **/
inline void toLittleEndian(unsigned long len, char *buffer);

/**
 * to big endian with swapEndian
 * does nothing if this isn't an intel
 **/
inline void toBigEndian(unsigned long len, char *buffer);

/**
 * swap the endian, does so on every platform
 * operates on 16 bits at a time. so loads 16, swaps, and copies them
 **/
void swapEndian(unsigned long length, char *buffer);
}

#endif
