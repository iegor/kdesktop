/*
  base class for all mpeglib decoders
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#include <queue>
#include <iostream>

#include <connect.h>

#include "decoderBaseObject_impl.h"
#include "../mpeglib/lib/decoder/decoderPlugin.h"
#include "debug.h"

// define this to run the playobject without the
// arts backend. (useful to check if a bug is in arts or mpeglib)
//#define _STRIP_ZERO

static int instanceCnt=0;

DecoderBaseObject_impl::DecoderBaseObject_impl()
    : _speed(1.0f)
{

  flpos=0.0;
  _blocking = false;

#ifdef _STRIP_ZERO
  outputStream=NULL;
#else
  m_outputStream=new ArtsOutputStream(NULL);
  arts_debug("outputStream created");
  decoderPlugin=NULL;
#endif
  startTime=0.0;
  m_inputStream=NULL;
  setStreamState(_THREADSTATE_INIT);
  _state=Arts::posIdle;
  instance=instanceCnt;
  instanceCnt++;

  m_packetQueue = new std::queue<DataPacket<mcopbyte>*>;
}

DecoderBaseObject_impl::~DecoderBaseObject_impl() {
  arts_debug("~DecoderBaseObject_impl -s");
  shudownPlugins();

  if (decoderPlugin != NULL) {
    arts_debug("delete decoderPlugin");
    delete decoderPlugin;
    decoderPlugin=NULL;
  }
  if (m_outputStream != NULL) {
    arts_debug("delete outputStream");
    delete m_outputStream;
    m_outputStream=NULL;
  }

  if (m_streaming)
    m_artsInputStream.streamEnd();

  delete m_packetQueue;
}


DecoderPlugin* DecoderBaseObject_impl::createPlugin() {
  arts_fatal("direct virtual call DecoderBaseObject_impl::getPlugin");
  return NULL;
}


InputStream* DecoderBaseObject_impl::createInputStream(const char* url) {
  InputStream* back = InputPlugin::createInputStream(url,true);
  return back;
}


bool DecoderBaseObject_impl::loadMedia(const string &filename) {
  arts_debug("loadMedia");
  int back=true;

  m_streaming = false;

  if ( m_inputStream != NULL ) {
    arts_fatal("remove resources first with a call to: halt()");
  }
  if (decoderPlugin == NULL) {
    decoderPlugin=createPlugin();
    if(doFloat()) decoderPlugin->config("dofloat",0,0);
  }

  flpos=0.0;
  startTime=0.0;

  lastAudioBufferSize=-1;
  /**
     Note: you can only play one file with a PlayObject !!
     Then you must destroy it.
     A StreamEnd call should do the job.
  */

#ifdef _STRIP_ZERO
  return true;
#endif

  m_inputStream=createInputStream(filename.c_str());

  // the plugin does not open the stream!
  // we do it.
  back=m_inputStream->open((char*)filename.c_str());
  setStreamState(_THREADSTATE_OPENED);

  // we are still in posIdle here
  m_outputStream->audioOpen();

  // watch the order!
  decoderPlugin->setOutputPlugin(m_outputStream);
  decoderPlugin->setInputPlugin(m_inputStream);

  return back;
}

#define INPUT_BUFFER_SIZE 32768

bool DecoderBaseObject_impl::streamMedia(Arts::InputStream instream) {
  arts_debug("DecoderBaseObject_impl::streamMedia -s");
  
  bool back = true;
  
  if (m_inputStream != NULL) {
    arts_fatal("resource in use, call halt() first");
  }
  if (decoderPlugin == NULL) {
    decoderPlugin = createPlugin();
    if (doFloat())
      decoderPlugin->config("dofloat", 0, 0);
    // streaming, don't know the length
    decoderPlugin->config("-c", 0, 0);
  }
  
  flpos = 0.0;
  startTime = 0.0;
  m_streaming = true;
  lastAudioBufferSize = -1;
  
  m_artsInputStream = instream;
  
  m_inputStream = new BufferInputStream(INPUT_BUFFER_SIZE, 4096, (char*)"InputStream");
  m_inputStream->open((char*)"InputStream");
  
  // connect the stream now
  Arts::StreamPlayObject self = Arts::StreamPlayObject::_from_base(_copy());
  connect(m_artsInputStream, "outdata", self);
  
  setStreamState(_THREADSTATE_OPENED);
  
  m_outputStream->audioOpen();
  
  decoderPlugin->setOutputPlugin(m_outputStream);
  decoderPlugin->setInputPlugin(m_inputStream);
  
  arts_debug("DecoderBaseObject_impl::streamMedia -e");
  
  return back;
}

void DecoderBaseObject_impl::process_indata(DataPacket<mcopbyte> *inpacket) {

  m_packetQueue->push(inpacket);
  processQueue();
}

void DecoderBaseObject_impl::processQueue() {

  // early exit if no packets in the queue
  if (m_packetQueue->empty())
    return;
  
  // see how much space we have in the stream
  BufferInputStream* stream = static_cast<BufferInputStream*>(m_inputStream);
  if (!stream) return;
  
  int length = stream->getByteLength();
  int freeSpace = INPUT_BUFFER_SIZE - length;
  
  DataPacket<mcopbyte> *inpacket = m_packetQueue->front();
  if (!inpacket) return;
  
  if (freeSpace >= inpacket->size) {
    stream->write((char*)inpacket->contents, inpacket->size, 0);
    m_packetQueue->pop();
    inpacket->processed();
  }
}

string DecoderBaseObject_impl::description() {
  arts_debug("description");
  string back;
#ifdef _STRIP_ZERO
  return back;
#endif
  PluginInfo* pluginInfo=decoderPlugin->getPluginInfo();
  pluginInfo->print();
  return back;

}

void DecoderBaseObject_impl::description(const string &) {
  arts_debug("description");
    // what should do this?
}

poTime DecoderBaseObject_impl::currentTime() {
  poTime time;
#ifdef _STRIP_ZERO
  return time;
#endif
  AudioTime* audioTime=m_outputStream->getAudioTime();
  float currentTime=audioTime->getTime()+(float)startTime;
  time.seconds=(long)(currentTime);
  time.ms=(long) (1000.0*(currentTime-(float)time.seconds));
  return time;
}



poTime DecoderBaseObject_impl::overallTime() {
  poTime time;
#ifdef _STRIP_ZERO
  return time;
#endif

  PluginInfo* pluginInfo=decoderPlugin->getPluginInfo();
  time.seconds=pluginInfo->getLength();
  time.ms=0;	
  return time;
}

poCapabilities DecoderBaseObject_impl::capabilities() {
  arts_debug("capabilities");
#ifdef _STRIP_ZERO
  return capSeek;
#endif
  PluginInfo* pluginInfo=decoderPlugin->getPluginInfo();
  long len=pluginInfo->getLength();
  if (len == 0) {
    return Arts::capPause;  /* no seek supported */
  }
  // seek and pause supported
  return (poCapabilities)(Arts::capSeek | Arts::capPause);
}

string DecoderBaseObject_impl::mediaName() {
  arts_debug("mediaName");
  string back;
  // whats a mediaName?
  return back;
}

poState DecoderBaseObject_impl::state() {
  return _state;
}

void DecoderBaseObject_impl::play() {
  arts_debug("play: %d", (int)streamState);
  if (streamState == _THREADSTATE_OPENED) {
      decoderPlugin->play();
  } else {
      Command cmd(_COMMAND_PLAY);
      decoderPlugin->insertAsyncCommand(&cmd);
  }
  setStreamState(_THREADSTATE_PLAYING);
  _state = Arts::posPlaying;
}

void DecoderBaseObject_impl::seek(const class poTime& seekTime) {
#ifdef _STRIP_ZERO
  return;
#endif

  long sec=seekTime.seconds;

  arts_debug("sec in plugin is %d:", sec);

  // we send an async command
  Command cmd(_COMMAND_SEEK,sec);
  decoderPlugin->insertAsyncCommand(&cmd);

  // if the thread blocks on the artsOutputstream: kick him out
  // the next command will the the seek command
  m_outputStream->audioClose();


  // thread blocking allowed
  m_outputStream->audioOpen();
  arts_debug("************ reopen");
  // now set a new startTime
  startTime=sec;
}

void DecoderBaseObject_impl::pause() {
  arts_debug("pause");
  _state = Arts::posPaused;
  Command cmd(_COMMAND_PAUSE);
  decoderPlugin->insertAsyncCommand(&cmd);
}

void DecoderBaseObject_impl::halt() {
  /*
   *
   * halt() (which the normal programmer would probably refer to as stop())
   * should seek to the beginning and go into the posIdle state, like a just
   * opened PlayObject
   *
   */

  arts_debug("halt");
  _state=Arts::posIdle;
  shudownPlugins();
}


void DecoderBaseObject_impl::streamInit() {
#ifdef _STRIP_ZERO
  return;
#endif

}


void DecoderBaseObject_impl::streamStart() {
    arts_debug("DecoderBaseObject_impl::streamStart");
}

int DecoderBaseObject_impl::fillArts(unsigned long samples,
				     float* left , float* right) {
  unsigned long haveSamples = 0;

  AudioTime* audioTime=m_outputStream->getAudioTime();
  int wav_samplingRate=audioTime->getSpeed();
  int wav_sampleWidth=audioTime->getSampleSize();
  int wav_channelCount=audioTime->getStereo()+1;

  if(doFloat()) wav_sampleWidth = sizeof(float)*8;

  // here seems to be an error, I have clicks sometimes in the stream
  //int byteMultiplikator=(wav_sampleWidth/8)*wav_channelCount;

  // maybe first multiply, then divide?
  int byteMultiplikator = wav_channelCount * wav_sampleWidth / 8;

  char* buffer;
  int hasBytes = 0;
  int wantBytes = 0;
  int bufferSize=getBufferSize();
  if (bufferSize != lastAudioBufferSize) {
    lastAudioBufferSize=bufferSize;
    m_outputStream->setAudioBufferSize(bufferSize);
  }

  /* difference between the sampling rates in percent */
  float diff = fabs((double)wav_samplingRate - (double)(samplingRateFloat/_speed))
             / (double)samplingRateFloat;

  /*
   * efficient optimized case:
   * 1. decoder -> float rendering
   * 2. no resampling (i.e. artsd running @ 44100 Hz, playing an 44100 Hz mp3)
   */
  if(_state == Arts::posPlaying && doFloat() && diff < 0.0005) {
    wantBytes = sizeof(float) * wav_channelCount * samples;
    hasBytes = m_outputStream->read(&buffer,wantBytes);
    float *flptr = (float *)buffer;

    if(wav_channelCount == 1)
    {
      while((int)(haveSamples * sizeof(float)) < hasBytes)
      {
        left[haveSamples] = right[haveSamples] = flptr[haveSamples];
	haveSamples++;
      }
    }
    else if(wav_channelCount == 2)
    {
      while((int)(haveSamples * 2 * sizeof(float)) < hasBytes)
      {
        left[haveSamples] = flptr[haveSamples*2];
        right[haveSamples] = flptr[haveSamples*2+1];
	haveSamples++;
      }
    }
    m_outputStream->forwardReadPtr(haveSamples*sizeof(float)*wav_channelCount);
  }
  else if(_state == Arts::posPlaying) {
    //
    // since the samplingrate of the MP3 and the samplingrate of the output
    // device (soundcard) are not necessarily the same, it's a bit tricky
    //

    // calculate "how fast" we consume input samples (2.0 means, we need 2
    // input samples to generate one output sample)
    double speed = (double)wav_samplingRate / (double)(samplingRateFloat/_speed);

    // calculate how many input samples we need, then to satisfy the request
    // use a larger amount than "really" required, to ensure that samples are
    // available for rounding errors and interpolation
    double wantWavSamples = (double)samples*speed+8.0;

    // convert that into bytes and try to read that many bytes
    wantBytes=(int) (wantWavSamples*byteMultiplikator);
    hasBytes=m_outputStream->read(&buffer,wantBytes);

    int format = doFloat()?Arts::uni_convert_float_ne:wav_sampleWidth;

    // convert those bytes into the suitable output form
    haveSamples = Arts::uni_convert_stereo_2float(samples, (unsigned char *)buffer,
					    hasBytes, wav_channelCount,
					    format,
					    left,right,speed,flpos);

    // calculate where we are now (as floating point position) in our
    // inputsample buffer
    flpos += (double)haveSamples * speed;

    // Good - so how many input samples we won't need anymore (for the
    // next request)? Skip them.
    int skip = (int)floor(flpos);
    // we need to call this even on skip == 0
    // because we must unlock the remoteBuffer
    int forward=skip*byteMultiplikator;


    flpos = flpos - floor(flpos);

    m_outputStream->forwardReadPtr(forward);
  }

  if(haveSamples != samples) {

    unsigned long i;

    for(i=haveSamples;i<samples;i++)
      left[i] = right[i] = 0.0;

  }
  return samples;
}

void DecoderBaseObject_impl::calculateBlock(unsigned long samples,
					    float* left , float* right) {


#ifndef _STRIP_ZERO

  int audioState=m_outputStream->waitStreamState(_OUTPUT_WAIT_METHOD_POLL,
					       _STREAM_MASK_ALL,
					       _STREAMTYPE_AUDIO);

  if (audioState & _STREAM_MASK_IS_INIT) {
    // now check if we already have enough data
    int lenough=false;
    if (audioState & _STREAM_MASK_IS_EOF) {
      if(_state == Arts::posPlaying) {
	arts_debug("eof got in arts********** END");
	_state = Arts::posIdle;
      }
      
      lenough=true;
    }
    if (m_outputStream->getBufferFillgrade() >= 4096) {
      lenough=true;
    }
    
    if (_state == Arts::posPlaying) {
      if (m_streaming) {
	// produce more data
	processQueue();
	// check for stream end
	if ( m_inputStream->getByteLength() == 0 ) {
	  if ( m_artsInputStream.eof() ) {
	    m_inputStream->close();
	    m_artsInputStream.streamEnd();
	  }
	}
      }
      if (lenough || _blocking) {
	fillArts(samples, left, right);
	return;
      }
    }
  }
#endif

  // filling with zero , stream not ready(yet)

  unsigned int i;
  for(i=0;i<samples;i++)
    left[i] = right[i] = 0.0;
}

void DecoderBaseObject_impl::streamEnd() {
  arts_debug("streamEnd");
#ifdef _STRIP_ZERO
  return;
#endif
  
  halt();
}


int DecoderBaseObject_impl::getBufferSize() {
  float hardwareBuffer;
  float fragmentSize;
  float fragmentCount;
  float channels;
  float sampleSize;
  fragmentSize=Arts::AudioSubSystem::the()->fragmentSize();
  fragmentCount=Arts::AudioSubSystem::the()->fragmentCount();
  channels=Arts::AudioSubSystem::the()->channels();
  sampleSize=16.0/8.0;

  hardwareBuffer=fragmentSize*fragmentCount;


  return (int)hardwareBuffer;
}


void DecoderBaseObject_impl::shudownPlugins() {
  arts_debug("shudownPlugins -s");
  /**
     The order here is important.
     First we close the audio so that the thread never blocks
     on the ringbuffer.
     Then we are sure thst we can safley call plugin->close,
     because the thread does not block.
     We then have the thread back in the decoder_loop of
     the plugin.
  */

  // this should theoretically be faster
  if (decoderPlugin != NULL) {
    Command cmd(_COMMAND_CLOSE);
    decoderPlugin->insertAsyncCommand(&cmd);
  }
  if (m_outputStream != NULL) {
    m_outputStream->audioClose();
  }

  // very likely the thread already is closed
  // because of the asyncCommand above.
  if (decoderPlugin) {
    decoderPlugin->close();
  }

  delete m_inputStream;
  m_inputStream=NULL;
  
  if (m_streaming)
    m_artsInputStream.streamEnd();

  setStreamState(_THREADSTATE_CLOSED);

  arts_debug("shudownPlugins -e");
}

void DecoderBaseObject_impl::setStreamState(int state) {

  switch (state) {
  case _THREADSTATE_INIT: {
    streamState=_THREADSTATE_INIT;
    break;
  }
  case _THREADSTATE_OPENED: {
    streamState=_THREADSTATE_OPENED;
    break;
  }
  case _THREADSTATE_PLAYING: {
    streamState=_THREADSTATE_PLAYING;
    break;
  }
  case _THREADSTATE_CLOSED: {
    streamState=_THREADSTATE_INIT;
    break;
  }
  default:
   std::cerr << "unknown streamState:DecoderBaseObject_impl:"<<state<<std::endl;
  }

}

/** internal stuff for testing **/

void DecoderBaseObject_impl::blocking(bool newvalue)
{
  _blocking = newvalue;
}

bool DecoderBaseObject_impl::blocking()
{
  return _blocking;
}

void DecoderBaseObject_impl::speed(float newValue)
{
    _speed= newValue;
}

float DecoderBaseObject_impl::speed()
{
    return _speed;
}
