/* MPEG/WAVE Sound library

   (C) 1997 by Jung woo-jae */

// Mpegtoraw.cc
// Server which get mpeg format and put raw format.


#include "mpegsound.h"
#include "synthesis.h"
#include "dump.h"
#include "../frame/audioFrame.h"

#include <iostream>

using namespace std;

Mpegtoraw::Mpegtoraw(MpegAudioStream* mpegAudioStream,
		     MpegAudioHeader* mpegAudioHeader) {

  this->mpegAudioStream=mpegAudioStream;
  this->mpegAudioHeader=mpegAudioHeader;
  
  this->lOutputStereo=true;
  setStereo(true);
  setDownSample(false);

  dump=new Dump();
  synthesis=new Synthesis();
  initialize();

}

Mpegtoraw::~Mpegtoraw() {

  delete synthesis;
  delete dump;
}







void Mpegtoraw::setStereo(int flag) {
  lWantStereo=flag;
}

void Mpegtoraw::setDownSample(int flag) {
  lDownSample=flag;
}

int Mpegtoraw::getStereo() {
  return lWantStereo;
}

int Mpegtoraw::getDownSample() {
  return lDownSample;
}







// Convert mpeg to raw
// Mpeg headder class
void Mpegtoraw::initialize() {



  layer3initialize();


}





// Convert mpeg to raw
int Mpegtoraw::decode(AudioFrame* audioFrame) {
  int back=true;

  this->audioFrame=audioFrame;
  if (audioFrame->getSize() < RAWDATASIZE) {
    cout << "audioFrame needs at least:"<<RAWDATASIZE<<" size"<<endl;
    exit(0);
  }

  audioFrame->clearrawdata();
  synthesis->clearrawdata();

  int layer=mpegAudioHeader->getLayer();
  this->lOutputStereo=lWantStereo & mpegAudioHeader->getInputstereo();

  if (mpegAudioHeader->getProtection()==false) {
    mpegAudioStream->getbyte();
    mpegAudioStream->getbyte();
  }    
  switch(layer) {
  case 3:
    extractlayer3();
    break;
  case 2:
    extractlayer2();
    break;
  case 1:
    extractlayer1();
    break;
  default:
    cout << "unknown layer:"<<layer<<endl;
    back=false;
  }
  
  //
  // Now put the frequencies/output etc.. in the frame
  //
  audioFrame->setFrameFormat(lOutputStereo,
			     mpegAudioHeader->getFrequencyHz()>>lDownSample);

  audioFrame->putFloatData(synthesis->getOutputData(),synthesis->getLen());
  return back;

}
