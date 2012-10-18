// audiofilePlayObject
//
// Copyright (C) 2001 Neil Stevens <neil@qualityassistant.com>
// Copyright (C) 2002 Rik Hemsley (rikkus) <rik@kde.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// THE AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
// AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// 
// Except as contained in this notice, the name(s) of the author(s) shall not be
// used in advertising or otherwise to promote the sale, use or other dealings
// in this Software without prior written authorization from the author(s).

#include "audiofilePlayObjectI.h"

#include <resample.h>
#include <soundserver.h>
#include <convert.h>
#include <debug.h>
#include <math.h>
#include <stdlib.h>

namespace Arts
{
  class AudioFilePlayObjectRefiller : public Refiller
  {
    public:

      AudioFilePlayObjectRefiller()
        : fileHandle_(AF_NULL_FILEHANDLE),
      frameSize_(0)
      {
      }

      void setFileHandle(AFfilehandle fileHandle)
      {
        fileHandle_ = fileHandle;
      }

      void setFrameSize(unsigned int frameSize)
      {
        frameSize_ = frameSize;
      }


      unsigned long read(unsigned char * buffer, unsigned long len)
      {
        if (AF_NULL_FILEHANDLE == fileHandle_)
          return 0;

        int framesRead =
          afReadFrames
          (fileHandle_, AF_DEFAULT_TRACK, (void *)buffer, len / frameSize_);

        if (-1 == framesRead)
          return 0;

        return framesRead * frameSize_;
      }

    private:

      AFfilehandle fileHandle_;
      unsigned int frameSize_;
  };
}

static inline AFframecount timeToFrame(poTime time, float samplingRate)
{
  float seconds = time.seconds;
  // ignoring ms
  return (int)(floor(seconds * samplingRate));
}

static inline poTime frameToTime(AFframecount frame, float samplingRate)
{
  float seconds = (float)frame / samplingRate;
  poTime time;
  time.seconds = (long int)(floor(seconds));
  time.ms = (long int)(floor(seconds * 1000.0) - (time.seconds * 1000.0));
  return time;
}

audiofilePlayObjectI::audiofilePlayObjectI()
  : audiofilePlayObject_skel()
  , StdSynthModule()
  , fh(AF_NULL_FILEHANDLE)
  , channels(0)
  , frameSize(0)
  , sampleWidth(0)
  , samplingRate(0)
  , myState(posIdle)
  , _speed(1.0)
  , resampler(0)
{
  refiller  = new AudioFilePlayObjectRefiller;
  resampler = new Arts::Resampler(refiller);
}

audiofilePlayObjectI::~audiofilePlayObjectI()
{
  delete refiller;
  refiller = 0;

  delete resampler;
  resampler = 0;

  if (sanityCheck())
  {
    afCloseFile(fh);
    fh = AF_NULL_FILEHANDLE;
  }
}

bool audiofilePlayObjectI::loadMedia(const string &filename)
{
  if (sanityCheck())
  {
    afCloseFile(fh);
    fh = AF_NULL_FILEHANDLE;
    refiller->setFileHandle(fh);
  }

  fh = afOpenFile(filename.c_str(), "r", 0);

  if (!sanityCheck())
  {
    this->filename = "";
    return false;
  }

  this->filename = filename;
  afSetVirtualByteOrder(fh, AF_DEFAULT_TRACK, AF_BYTEORDER_LITTLEENDIAN);

  int sampleFormat;

  channels      = afGetChannels(fh, AF_DEFAULT_TRACK);

  afGetSampleFormat(fh, AF_DEFAULT_TRACK, &sampleFormat, &sampleWidth);

  samplingRate  = afGetRate(fh, AF_DEFAULT_TRACK);
  frameSize     = sampleWidth / 8 * channels;

  arts_debug("loading wav: %s", filename.c_str());
  arts_debug("  frame size: %d", frameSize);

  resampler->setChannels(channels);
  resampler->setBits(sampleWidth);
  resampler->setEndianness(Arts::Resampler::littleEndian);

  refiller->setFileHandle(fh);
  refiller->setFrameSize(frameSize);

  arts_debug("  channels: %d", channels);
  arts_debug("  bits: %d",sampleWidth);

  myState = posIdle;
  return true;
}

string audiofilePlayObjectI::description()
{
  return "audiofilePlayObject";
}

string audiofilePlayObjectI::mediaName()
{
  return filename;
}

poCapabilities audiofilePlayObjectI::capabilities()
{
  return static_cast<poCapabilities>(capSeek | capPause);
}

poState audiofilePlayObjectI::state()
{
  if(!sanityCheck())
    return posIdle;
  else  
    return myState;
}

void audiofilePlayObjectI::play()
{
  myState = posPlaying;
}

void audiofilePlayObjectI::pause()
{
  myState = posPaused;
}

void audiofilePlayObjectI::halt()
{
  afSeekFrame(fh, AF_DEFAULT_TRACK, 0);
  myState = posIdle;
}

poTime audiofilePlayObjectI::currentTime()
{
  if (!sanityCheck())
    return poTime(0,0,0,"samples");

  AFfileoffset offset = afTellFrame(fh, AF_DEFAULT_TRACK);

  float timesec = offset / samplingRate;
  float timems = (timesec - floor(timesec)) * 1000.0;

  return poTime
    (
      int(timesec),
      int(timems),
      offset,
      "samples"
    );
}

poTime audiofilePlayObjectI::overallTime()
{
  if (!sanityCheck())
    return poTime(0, 0, 0, "samples");

  AFfileoffset offset = afGetTrackBytes(fh, AF_DEFAULT_TRACK) / frameSize;

  float timesec = offset / (float)samplingRate;
  float timems = (timesec - floor(timesec)) * 1000.0;

  return poTime(int(timesec), int(timems), offset, "samples");
}

void audiofilePlayObjectI::seek(const poTime &time)
{
  if (!sanityCheck())
  {
    return;
  }

  float fnewsamples = -1;

  if (time.seconds != -1 && time.ms != -1)
  {
    float flnewtime = (float)time.seconds+((float)time.ms/1000.0);
    fnewsamples = flnewtime * samplingRate;
  }
  else if (time.custom >= 0 && time.customUnit == "samples")
  {
    fnewsamples = time.custom;
  }

  // Avoid going past end of file.

  AFfileoffset eof = afGetTrackBytes(fh, AF_DEFAULT_TRACK) / frameSize;

  if (fnewsamples > (float)eof)
    fnewsamples = (float)eof;

  // Avoid going past beginning of file.

  if (fnewsamples < 0)
    fnewsamples = 0.0;

  afSeekFrame(fh, AF_DEFAULT_TRACK, (unsigned long)fnewsamples);
}

void audiofilePlayObjectI::calculateBlock(unsigned long count)
{
  if (myState == posPlaying)
  {
    double speed = samplingRate / samplingRateFloat;

    resampler->setStep(speed * _speed);
    resampler->run(left, right, count);

    if (resampler->underrun())
    {
      myState = posIdle;
    }
  }
  else
  {
    for (unsigned long i = 0; i < count; i++)
    {
      left[i] = right[i] = 0;
    }
  }

}

AutoSuspendState audiofilePlayObjectI::autoSuspend()
{
  return asNoSuspend;
}

float audiofilePlayObjectI::speed()
{
  return _speed;
}

void audiofilePlayObjectI::speed(float newSpeed)
{
  if(newSpeed != _speed)
  {
    _speed = newSpeed;
    speed_changed(_speed);
  }
}

void audiofilePlayObjectI::start()
{
}

void audiofilePlayObjectI::stop()
{
}

void audiofilePlayObjectI::streamInit()
{
}

void audiofilePlayObjectI::streamStart()
{
}

void audiofilePlayObjectI::streamEnd()
{
}

REGISTER_IMPLEMENTATION(audiofilePlayObjectI);
// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
