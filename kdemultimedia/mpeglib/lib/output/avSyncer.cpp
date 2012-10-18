/*
  encapsulates the syncing methods, to use it in other classes
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "avSyncer.h"

#include "audioDataArray.h"
#include "performance.h"

#include <iostream>

using namespace std;

AVSyncer::AVSyncer(int bufferSize) {
  oneFrameTime=0;
  onePicFrameInAudioBytes=8192;
  this->bufferSize=bufferSize;

  abs_thread_mutex_init(&writeInMut);
  abs_thread_mutex_init(&changeMut);

  
  audioDataInsert=new AudioData();
  audioDataArray=new AudioDataArray(400);
  audioDataCurrent=audioDataArray->readAudioData();
  
  startAudio=new TimeStamp();
  endAudio=new TimeStamp();
  audioTime=new AudioTime();
  lAudioRunning=false;
  performance=new Performance();

  waitTime=new TimeStamp();
  diffTime=new TimeStamp();
  videoTimeStamp=new TimeStamp();

  lPerformance=false;
  lavSync=true;


}


AVSyncer::~AVSyncer() {
  delete audioDataArray;
  delete audioDataInsert;
  delete audioTime;
  delete startAudio;
  delete endAudio;
  abs_thread_mutex_destroy(&writeInMut);
  abs_thread_mutex_destroy(&changeMut);
  delete waitTime;
  delete diffTime;
  delete performance;
  delete videoTimeStamp;
}



int AVSyncer::audioSetup(int frequency,int stereo,int sign,
			 int big,int sixteen) {
  audioTime->setFormat(stereo,sixteen,frequency,sign,big);
  setAudioRunning(true);
  return true;
}


int AVSyncer::audioPlay(TimeStamp* startStamp,
			TimeStamp* endStamp,char* , int size) {




  audioDataInsert->setStart(startStamp);
  audioDataInsert->setEnd(endStamp);
  audioDataInsert->setAudioTime(audioTime);
  audioDataInsert->setPCMLen(size);

  setAudioSync(audioDataInsert);
  return size;
}



void AVSyncer::audioClose(void) {
  lockSyncData();
  setAudioRunning(false);
  audioDataArray->clear();
  unlockSyncData();
}





int AVSyncer::getPreferredDeliverSize() {
  return onePicFrameInAudioBytes;
}



int AVSyncer::getFrameusec() {
  lockSyncData();
  int back=oneFrameTime;
  unlockSyncData();
  return back; 
}

void AVSyncer::setAudioBufferSize(int size) {
  bufferSize=size;
}


void AVSyncer::config(const char* key,const char* value,void*) {
  if (strcmp(key,"-s")==0) {
    if (strcmp(value,"on")==0) {
      lavSync=true;
      cout << "******** lavSync on"<<endl;
    } else {
      lavSync=false;
      cout << "******** lavSync off"<<endl;
    }
  }
  if (strcmp(key,"-p")==0) {
    cout << "setting perfomance test true"<<endl;
    lPerformance=true;
  }  

}



void AVSyncer::setAudioSync(AudioData* audioData) {

  lockSyncData();

  if (onePicFrameInAudioBytes <= 0) {
    // no video present, we cannot calculate audio
    // pcm length
    //cout << "no video present, we cannot calculate audio pcm length"<<endl;
    unlockSyncData();    
    return;
  }
  // buffersize is the simulated size of /dev/dsp



  // we implement a fifo hopefully
  // we check the right audio pts time out
  // when /dev/dsp acutally plays it.
  audioDataArray->insertAudioData(audioData);
  int pcmSum=audioDataArray->getPCMSum();
  if (pcmSum >= bufferSize) {
    audioDataCurrent=audioDataArray->readAudioData();
    setAudioRunning(true);
    audioDataArray->forward();
  }
  TimeStamp* startAudio=audioDataCurrent->getStart();
  int lpts=startAudio->getPTSFlag();

  if (lpts==true) {
    SyncClock* syncClock=startAudio->getSyncClock();
    if (syncClock != NULL) {
      double pts=startAudio->getPTSTimeStamp();
      double scr=startAudio->getSCRTimeStamp();

      syncClock->syncAudio(pts,scr);

    } else {
      cout <<"syncClock == NULL (audio)"<<endl;
    }
  } else {
    //cout << "lpts is false"<<endl;
  }
  unlockSyncData();    
  
}



int AVSyncer::syncPicture(YUVPicture* syncPic) {
  if (syncPic == NULL) {
    cout << "syncPic == NULL"<<endl;
    return false;
  }

  float picPerSec=syncPic->getPicturePerSecond();
  int oneFrameTime=0;

  if (picPerSec > 0.0) {
    oneFrameTime=(int) (1000000.0/picPerSec);
  } else {
    syncPic->print("picPersec is 0");
    return true;
  }

  if (lPerformance==true) {
    waitTime->set(0,0);
    syncPic->setWaitTime(waitTime);
    performance->incPictureCount();
    return true;
  }

  int lpacketSync=true;

  videoTimeStamp->gettimeofday();
  diffTime->minus(videoTimeStamp,videoTimeStamp);


  if (lavSync==false) {
    if (videoTimeStamp->isNegative()) {
      diffTime->gettimeofday();
      diffTime->addOffset(0,oneFrameTime);
      cout << "skip time based"<<endl;
      return false;
    }
  }



  videoTimeStamp->copyTo(waitTime);

  TimeStamp* earlyTime=syncPic->getEarlyTime();
  earlyTime->set(0,0);


  if (lavSync) {
    
    lpacketSync=avSync(syncPic->getStartTimeStamp(),
		       waitTime,
		       earlyTime,
		       syncPic->getPicturePerSecond());

  } 




  if (lpacketSync == false) {
    //cout << "skip"<<endl;
    diffTime->gettimeofday();
    diffTime->addOffset(0,oneFrameTime);
    return false;
  }

  syncPic->setWaitTime(waitTime);

  if (lavSync) {
     waitTime->minus(videoTimeStamp,waitTime);
     if (waitTime->isPositive()) {
       diffTime->addOffset(waitTime);
     }
  }
  diffTime->addOffset(0,oneFrameTime);
  return true;


}



/**
   Heart of the sync routine is here!

   Currently its more in a state of "try/test"

   
*/
int AVSyncer::avSync(TimeStamp* startVideo,
		     TimeStamp* waitTime,
		     TimeStamp* earlyTime,
		     float picPerSec ) {


  double videoStartPTSTime=startVideo->getPTSTimeStamp();
  double videoStartSCRTime=startVideo->getSCRTimeStamp();
  int videoFrameCounter=startVideo->getVideoFrameCounter();
  double frameTime=0.0;

 

  lockSyncData();
  if (picPerSec > 0) {
    oneFrameTime=(long)(1000000.0/picPerSec);
    frameTime=1.0/picPerSec;
    onePicFrameInAudioBytes=audioTime->calculateBytes(1.0/picPerSec);

  }
  if (lAudioRunning==false) {
    waitTime->set(0,oneFrameTime);

    unlockSyncData();
    return true;
  }
   

  /*
    startAudio->print("audio");
    startVideo->print("video");
  */




		       

  /*
  cout << "audioStartAudioPacketNrMinor:"<<audioStartAudioPacketNrMinor<<endl;
  cout << "audioStartPTSTime:"<<audioStartPTSTime<<endl;
  cout << "audioStartEndPTSTime:"<<audioStartEndPTSTime<<endl;
  cout << "videoStartPTSTime:"<<videoStartPTSTime<<endl;
  */
  
    
  /**
     Here we make sure that we sync over an audio packet only one
     time.
  */

  waitTime->set(0,0);
  SyncClock* syncClock=startVideo->getSyncClock();
  int back=false;
  double addPts=videoFrameCounter*frameTime;
  double pts=videoStartPTSTime+addPts;

  if (syncClock != NULL) {

    back=syncClock->syncVideo(pts,
			      videoStartSCRTime,earlyTime,waitTime);
  } else {
    cout << "syncClock == NULL (video)"<<endl;
  }
  unlockSyncData();

  if (back==true) {
    //earlyTime->print("earlyTime");
    earlyTime->waitForIt();
    /*
    double tmp;
    double time=syncClock->getPTSTime(&tmp);
    cout << "time after wait:"<<time<<endl;
    */
  }
  /*
  if (back == false) {
    cout <<"real pts:"<<videoStartPTSTime
         <<" calc pts"<<pts
	 <<" frameNo:"<<videoFrameCounter
         <<" frameTime:"<<frameTime<<endl;
  }
  */
          


  return back;

}
  
int AVSyncer::getAudioRunning() {
  return lAudioRunning;
}


void AVSyncer::setAudioRunning(int lAudioRunning) {
  this->lAudioRunning=lAudioRunning;
}
 

void AVSyncer::lockSyncData() {
  abs_thread_mutex_lock(&changeMut);
  abs_thread_mutex_lock(&writeInMut);
  abs_thread_mutex_unlock(&changeMut);
}

void AVSyncer::unlockSyncData() {
  abs_thread_mutex_unlock(&writeInMut); 
}
