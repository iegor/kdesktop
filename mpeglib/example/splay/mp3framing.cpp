/*

  Example how frame creation works.

  For the input we have std. file input.
  We convert it to mpeg I encoded frames (mpegAudioFrame's job)

  The decoded pcmFrames are stores in a queue.
  
  If the queue if full, we "stream" the packets to /dev/dsp.
  (The difference between directly playing the packets and "streaming"
  is, that while streaming you can say: I want x bytes I don't care
  about frame boundaries)
  This is audioFrameQueue's job.


 */


#include "../../lib/splay/mpegAudioFrame.h"
#include "../../lib/splay/splayDecoder.h"
#include "../../lib/util/audio/dspWrapper.h"
#include "../../lib/frame/audioFrameQueue.h"

#include <iostream>

using namespace std;

// used for getopt
#include <unistd.h>

#define INPUT_SIZE  8192
#define _QUEUE_FLOAT 1
#define _QUEUE_INT   2


void usage() {
  printf("Usage:\n\n");
  printf("mp3framing [hf] filename\n\n");
  printf("             -f use float queue\n");
  printf("             -c [samples] checkout samples\n");
}


void initDSP(DSPWrapper* dsp) {
  if (dsp->openDevice()==false) {
    cout << "cannot open /dev/dsp"<<endl;
    exit(-1);
  }
}
  


int main(int argc, char** argv) {
  int queueType=_QUEUE_INT;
  int samples=8192;

  PCMFrame* outFrame;

  while(1) { 
    int c = getopt (argc, argv, "hfc:");
    if (c == -1) break;
    switch(c) {
    case 'h': {    
      usage();
      exit(-1);
      break;
    }
    case 'f': {    
      queueType=_QUEUE_FLOAT;
      break;
    }
    case 'c': {    
      samples=atoi(optarg);
      cout << "checking out samples:samples"<<endl;
      break;
    }
    default:
      printf ("?? getopt returned character code 0%o ??\n", c);
      exit(-1);
    }
  }
  // if we have nothing left, there is no filename
  if (optind >= argc ) {
    usage();
    exit(-1);
  }
  FILE* file=fopen(argv[optind],"r");
  if (file == NULL) {
    cout << "cannot open:"<<argv[1]<<endl;
    exit(-1);
  }
  DSPWrapper* dsp=new DSPWrapper();
  outFrame=new PCMFrame(samples);

  float* left=new float[samples];
  float* right=new float[samples];


  // create queue with 500 frames. We want output to dsp(later)
  // so we need a _FRAME_AUDIO_PCM type and we need
  // for splay at least MP3FRAMESIZE
  AudioFrameQueue* frameQueue;
  if (queueType == _QUEUE_INT) {
    frameQueue=new AudioFrameQueue(500,MP3FRAMESIZE,_FRAME_AUDIO_PCM);
  }
  if (queueType == _QUEUE_FLOAT) {
    frameQueue=new AudioFrameQueue(100,MP3FRAMESIZE,_FRAME_AUDIO_FLOAT);
  }
   

  initDSP(dsp);
  SplayDecoder* splay=new SplayDecoder();


  //
  // MpegAudioFrame has the ability to convert "raw" file
  // data into mpeg I frames. These are "valid" frames,
  // which means: header is ok, and the packet has the
  // correct length. splay decodes these frames _at once_
  // so we need no callbacks for the decoder.
  MpegAudioFrame* frame=new MpegAudioFrame();


  // Note: The inputbuffer must NOT NOT NOT be on the stack!
  // correction: it should not be corrupted by allocation
  // on the stack. This is true, if allocation+decoding
  // or in the _same_ function.
  unsigned char inputbuffer[INPUT_SIZE];


  int cnt=0;
  // we proceed until eof, or our "queue" is full.
  while( (feof(file) == false) && (frameQueue->dataQueueCanWrite() == true) ) {

    //
    // This switch/case statement "partitions" the fileinput
    // into mp3frames. We directly decode them to pcmFrames.
    //

    //
    // There are three states. One is "I want Input, give me input"
    // The other is "I have enough input, dont' bother me and let
    // me do my work"
    // The last ist "I have a frame here, take care of this, or
    // I will continue with my work"
 
    int state=frame->getState();
    switch(state) {
    case FRAME_NEED: {
      int bytes=frame->canStore();
      int read=fread(inputbuffer,1,bytes,file);
      if (read != bytes) {
	// read error. reset framer
	frame->reset();
	continue;
      }
      frame->store(inputbuffer,bytes);
      break;
    }
    case FRAME_WORK:
      frame->work();
      break;
    case FRAME_HAS:{
      AudioFrame* emptyFrame= frameQueue->emptyQueueDequeue();
      splay->decode(frame->outdata(),frame->len(),emptyFrame);
      frameQueue->dataQueueEnqueue(emptyFrame);
      cout << "storing decodec frame:"<<cnt<<endl;
      cnt++;
      break;
    }
    default:
      cout << "unknown state in mpeg audio framing"<<endl;
      exit(0);
    }
  }


  //
  // Fine we put all decoded frames in the buffer.
  // Now we want to "stream" from this buffer.
  // 

  //
  // create our "local" stream
  short int playbuf[INPUT_SIZE];
   
  //
  // we need to setup the dsp manually, when we do not play pcmFrames directly
  //
  if (queueType == _QUEUE_INT) {
    AudioFrame* audioFrame=frameQueue->getCurrent();
    dsp->audioSetup(audioFrame);
  }
  
  if (queueType == _QUEUE_FLOAT) {
    AudioFrame* audioFrame=frameQueue->getCurrent();
    dsp->audioSetup(audioFrame->getStereo(),16,
		    audioFrame->getSigned(),
		    audioFrame->getBigEndian(),
		    audioFrame->getFrequenceHZ());
    outFrame->setFrameFormat(audioFrame->getStereo(),
			     audioFrame->getFrequenceHZ());
  }
  


  //
  // read from stream demo:
  //
  
  while(frameQueue->getLen() > 0) {
    int hasLen= frameQueue->getLen();

    cout << "hasLen:"<<hasLen<<endl;
    int hasRead;
    if (hasLen < samples) {
      samples=hasLen;
    }

    

    if (queueType == _QUEUE_INT) {
      cout << "reading :"<<samples<<" data"<<endl;
      hasRead=frameQueue->copy(outFrame->getData(),samples);
      dsp->audioPlay((char*)outFrame->getData(),hasRead*sizeof(short int));
      frameQueue->forwardStreamSingle(hasRead);
    }


    if (queueType == _QUEUE_FLOAT) {
      int n;

      hasRead=frameQueue->copy(left,right,samples);
      frameQueue->forwardStreamDouble(hasRead);
      n=hasRead;

      /*
      FloatFrame* floatFrame=( FloatFrame*) frameQueue->dataQueueDequeue();
      float* left=floatFrame->getData();
      n=floatFrame->getLen();
      */

      int i=0;
      while(i<n) {
	int val=(int)(32768.0*left[i]);
	if (val > 32767) val=32767;
	if (val < -32768) val=-32768;
	dsp->audioPlay((char*)&val,2);
	
	if (outFrame->getStereo()) {
	  val=(int)(32768.0*right[i]);
	  if (val > 32767) val=32767;
	  if (val < -32768) val=-32768;
	  dsp->audioPlay((char*)&val,2);
	}

	i++;
      }

    }
  }


  delete frameQueue;
  delete splay;
  delete frame;
  delete dsp;
  delete left;
  delete right;
  delete outFrame;
  return(0);  
}

