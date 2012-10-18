/*
  queues audio frames in an IOQueue, allows streaming from frames
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "audioFrameQueue.h"

#define _FLOAT_2_TRANSFER       1
#define _FLOAT_1_TRANSFER       2
#define _INT_2_TRANSFER         3
#define _INT_1_TRANSFER         4
#define _FORWARD_TRANSFER       5

#include <iostream>

using namespace std;

AudioFrameQueue::AudioFrameQueue(int queueSize,
				 int frameSize,
				 int frameType):IOFrameQueue(queueSize) {
  switch(frameType) {
  case _FRAME_AUDIO_PCM: {
    // fill queue with elements
    while(emptyQueueCanWrite()) {
      PCMFrame* pcmFrame=new PCMFrame(frameSize);
      emptyQueueEnqueue(pcmFrame);
    }
    break;
  }
  case _FRAME_AUDIO_FLOAT: {
    // fill queue with elements
    while(emptyQueueCanWrite()) {
      FloatFrame* floatFrame=new FloatFrame(frameSize);
      emptyQueueEnqueue(floatFrame);
    }
    break;
  }
  default:
    cout << "unknown frameType:"<<Frame::getFrameName(frameType)
	 <<" in AudioFrameQueue"<<endl;
    exit(0);
  }
  len=0;
  currentAudioFrame=new AudioFrame();
  currentRead=0;
  this->frameType=frameType;
}


AudioFrameQueue::~AudioFrameQueue() {
  delete currentAudioFrame;
}


void AudioFrameQueue::emptyQueueEnqueue(AudioFrame* frame) {
  empty->enqueue(frame);
}


AudioFrame* AudioFrameQueue::emptyQueueDequeue() {
  return (AudioFrame*)empty->dequeue();
}


void AudioFrameQueue::dataQueueEnqueue(AudioFrame* frame) {
  if (data->getFillgrade() == 0) {
    frame->copyFormat(currentAudioFrame);
  }

  len+=frame->getLen();
  data->enqueue(frame);

}


AudioFrame* AudioFrameQueue::dataQueueDequeue() {
  AudioFrame* back=(AudioFrame*)data->dequeue();
  currentRead=0;
  len-=back->getLen();
  back->copyFormat(currentAudioFrame);
  return back;
}

AudioFrame* AudioFrameQueue::getCurrent() {
  return currentAudioFrame;
}


int AudioFrameQueue::getLen() {
  return len-currentRead;
}


int AudioFrameQueue::copy(float* left,float* right,int wantLen) {
  if (frameType != _FRAME_AUDIO_FLOAT) {
    cout << "AudioFrameQueue::copy class is frameType short int"<<endl;
    exit(0);
  }
  // whats mux?
  // well the routines are badly broken and need a rewrite
  // mux deals with the problem that
  // if the src is stereo we expect the left/right pointers
  // to have half the size of wantlen*2
  // ridicouls complex? 
  // right, as I said, this needs a clean solution
  int mux=1;

  if (currentAudioFrame->getStereo()) {
    wantLen*=2;
    mux=2;
  }
  
  int back=copygeneric((char*)left,(char*)right,
		       wantLen,_FLOAT_2_TRANSFER,mux);
  if (currentAudioFrame->getStereo()) back/=2;

  return back;
}

int AudioFrameQueue::copy(short int* left,short int* right,int wantLen) {
  if (frameType != _FRAME_AUDIO_PCM) {
    cout << "AudioFrameQueue::copy class is frameType float"<<endl;
    exit(0);
  }
  // for this mux ugly hack read above
  int mux=1;

  if (currentAudioFrame->getStereo()) {
    wantLen*=2;
    mux=2;
  }
  int back=copygeneric((char*)left,(char*)right,
		       wantLen,_INT_2_TRANSFER,mux);
  if (currentAudioFrame->getStereo()) back/=2;

  return back;
}

int AudioFrameQueue::copy(short int* dest,int wantLen) {
  if (frameType != _FRAME_AUDIO_PCM) {
    cout << "AudioFrameQueue::copy class is frameType int single"<<endl;
    exit(0);
  }
  int back=copygeneric((char*)dest,(char*)NULL,
		       wantLen,_INT_1_TRANSFER,1);
  return back;
}

int AudioFrameQueue::copy(float* dest,int wantLen) {
  if (frameType != _FRAME_AUDIO_FLOAT) {
    cout << "AudioFrameQueue::copy class is frameType float single"<<endl;
    exit(0);
  }
  int back=copygeneric((char*)dest,(char*)NULL,
		       wantLen,_FLOAT_1_TRANSFER,1);
  return back; 
}

void AudioFrameQueue::forwardStreamSingle(int forwardLen) {
  int back=copygeneric((char*)NULL,(char*)NULL,
		       forwardLen,_FORWARD_TRANSFER,1);
  if (back != forwardLen) {
    cout << "error while forwarding stream"<<endl;
    exit(0);
  }
}

void AudioFrameQueue::forwardStreamDouble(int forwardLen) {
  
  int mux=1;

  if (currentAudioFrame->getStereo()) {
    forwardLen*=2;
  }
  int back=copygeneric((char*)NULL,(char*)NULL,
		       forwardLen,_FORWARD_TRANSFER,mux);
  
  if (back != forwardLen) {
    cout << "error while forwarding stream"<<endl;
    exit(0);
  }
}


int AudioFrameQueue::copygeneric(char* left,char* right,
				 int wantLen,int version,int mux) {


  int processed=currentRead;

  if (wantLen > (len-processed)) {
    wantLen=len-processed;
  }
  int pos=0;
  int doLen=wantLen;
  while(doLen > 0) {
    AudioFrame* current=(AudioFrame*)data->peekqueue(pos);
    int totallen=current->getLen();
    int restlen=totallen-processed;
    int copylen=doLen;
    if (doLen > restlen) {
      copylen=restlen;
    }
    doLen-=copylen;

    switch(version) {
    case _FLOAT_2_TRANSFER:
      transferFrame((float*)left,
		    (float*)right,
		    (FloatFrame*) current,processed,copylen);
      left+=copylen/mux*(sizeof(float));
      right+=copylen/mux*(sizeof(float));
      break;
    case _FLOAT_1_TRANSFER:
      transferFrame((float*)left,
		    (FloatFrame*) current,processed,copylen);
      left+=copylen*sizeof(short int);
      break;
    case _INT_2_TRANSFER:
      transferFrame((short int*)left,
		    (short int*)right,
		    (PCMFrame*) current,processed,copylen);
      left+=copylen/mux*(sizeof(short int));
      right+=copylen/mux*(sizeof(short int));
      break;
    case _INT_1_TRANSFER:
      transferFrame((short int*)left,
		    (PCMFrame*) current,processed,copylen);
      left+=copylen*sizeof(short int);
      break;
    case _FORWARD_TRANSFER: 
      break;
    default:
      cout << "unknown transfer method AudioFrameQueue::copygeneric"<<endl;
      exit(0);
    }
    processed+=copylen;
    if ( processed == totallen) {
      processed=0;
      if (version == _FORWARD_TRANSFER) {
	current=dataQueueDequeue();
	emptyQueueEnqueue(current);
      } else {
	pos++;
      }
    }
  }
  if (version == _FORWARD_TRANSFER) {
    currentRead=processed;
  }
  if (doLen < 0) {
    cout << "error while copy in AudioFrameQueue"<<endl;
    exit(0);
  }
  return wantLen;
}


void AudioFrameQueue::transferFrame(float* left,float* right,
				    FloatFrame* current,int start,int len) {
  float* ptr=current->getData()+start;
  switch(currentAudioFrame->getStereo()) {
  case true:
    len=len/2;
    while(len) {
      *left++=*ptr++;
      *right++=*ptr++;
      len--;
    }
    break;
  case false:
    while(len) {
      *left++=*ptr;
      *right++=*ptr++;
      len--;
    }
    break;
  default:
    cout << "bad stereo value AudioFrameQueue::transferFrame (float)"<<endl;
    exit(0);
  }
}

void AudioFrameQueue::transferFrame(short int* left,short int* right,
				    PCMFrame* current,int start,int len) {
  short int* ptr=current->getData()+start;
  switch(currentAudioFrame->getStereo()) {
  case true:
    len=len/2;
    while(len) {
      *left++=*ptr++;
      *right++=*ptr++;
      len--;
    }
    break;
  case false:
    while(len) {
      *left++=*ptr;
      *right++=*ptr++;
      len--;
    }
    break;
  default:
    cout << "bad stereo value AudioFrameQueue::transferFrame (int)"<<endl;
    exit(0);
  }
}


void AudioFrameQueue::transferFrame(short int* dest,
				    PCMFrame* current,int start,int len) {
  short int* ptr=current->getData()+start;
  memcpy(dest,ptr,len*sizeof(short int));

}


void AudioFrameQueue::transferFrame(float* dest,
				    FloatFrame* current,int start,int len) {
  float* ptr=current->getData()+start;
  memcpy(dest,ptr,len*sizeof(float));

}


void AudioFrameQueue::clear() {
  while(dataQueueCanRead()) {
    AudioFrame* current=dataQueueDequeue();
    emptyQueueEnqueue(current);
  }
}
