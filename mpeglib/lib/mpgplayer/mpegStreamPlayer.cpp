/*
  feeds audio/video streams to the decoders
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "mpegStreamPlayer.h"
#include "../mpegplay/mpegSystemHeader.h"
#include "../util/syncClockMPEG.h"

#define _NUKE_BUFFER_SIZE   1024

#define _BUFFER_HIGH_WATER  0.8
#define _BUFFER_LOW_WATER   0.2

#define _BUFFER_CHUNK_SIZE  1500

#include <iostream>

using namespace std;


MpegStreamPlayer::MpegStreamPlayer(InputStream* input,
				   OutputStream* output,
				   DecoderPlugin* audioDecoder,
				   DecoderPlugin* videoDecoder) {
  this->input=input;
  this->output=output;
  this->audioDecoder=audioDecoder;
  this->videoDecoder=videoDecoder;

  /**
     here we set the plugins in autoPlay mode.
     because we handle all commands over the loopback buffer.
     This is necessary, because on start we send play()
     but this does not mean that we already have
     build this class (divxplugin for example)
     See this switch as an implicit play command
  */

  audioDecoder->config("-y","on",NULL);
  videoDecoder->config("-y","on",NULL);


  audioInput=new BufferInputStream(1024*_BUFFER_CHUNK_SIZE,
				   1024*200,"audioInput");
  videoInput=new BufferInputStream(1024*_BUFFER_CHUNK_SIZE,
				   1024*200,"videoInput");
  audioInput->open("audio loopback");
  videoInput->open("video loopback");
  
  audioDecoder->setOutputPlugin(output);
  audioDecoder->setInputPlugin(audioInput);

  videoDecoder->setOutputPlugin(output);
  videoDecoder->setInputPlugin(videoInput);

  timeStampVideo=new TimeStamp();
  timeStampAudio=new TimeStamp();

  packetCnt=0;
  audioPacketCnt=0;
  videoPacketCnt=0;
  seekPos=-1;
  nukeBuffer=new char[_NUKE_BUFFER_SIZE];

  syncClock=new SyncClockMPEG();
  syncClock->setSyncMode(__SYNC_AUDIO);
  writeToDisk=false;
}


MpegStreamPlayer::~MpegStreamPlayer() {
  audioInput->close();
  videoInput->close();
 
  // everything is prepared for future shutdown
  // restart any waiting decoders
  // (they exit immediately)
  audioDecoder->close();
  videoDecoder->close();
  audioDecoder->waitForStreamState(_STREAM_STATE_EOF);
  videoDecoder->waitForStreamState(_STREAM_STATE_EOF);
  
  delete videoInput;
  delete audioInput;
  delete nukeBuffer;
  delete timeStampVideo;
  delete timeStampAudio;
  delete syncClock;
}

int MpegStreamPlayer::getByteDirect() {
  unsigned char byte;
  if (input->read((char*)&byte,1) != 1) {
    return -1;
  }
  return (int)byte;
}

int MpegStreamPlayer::processResyncRequest() {
  // here is our protocol for resync over the loopback stream
  // if out clients are in sync and we set the resync method on the
  // stream he sometimes set a "commit" in the stream
  // we check if both stream are committed (== clients are in sync and paused)
  // if both are in _STREAM_STATE_RESYNC_COMMIT
  // we can safley do a seek
  // and restart them

  if (seekPos != -1) {
    int audioDecoderState=audioDecoder->getStreamState();
    int videoDecoderState=videoDecoder->getStreamState();
    if  (audioDecoderState == _STREAM_STATE_RESYNC_COMMIT) {
      if (videoDecoderState == _STREAM_STATE_RESYNC_COMMIT)  {
	Command restart(_COMMAND_RESYNC_END);
	input->seek(seekPos);
	seekPos=-1;
	audioDecoder->insertSyncCommand(&restart);
	videoDecoder->insertSyncCommand(&restart);
	Command play(_COMMAND_PLAY);
	audioDecoder->insertAsyncCommand(&play);
	videoDecoder->insertAsyncCommand(&play);
	return true;
      }
    }
  }
  return false;
}

/**
   according to the header info we read the packets
*/
int MpegStreamPlayer::processSystemHeader(MpegSystemHeader* mpegSystemHeader) {
  int layer=mpegSystemHeader->getLayer();


  if (processResyncRequest() == true) {
    return false;
  }
  
  if (layer==_PACKET_NO_SYSLAYER) {
    syncClock->setSyncMode(__SYNC_NONE);
    return insertVideoData(mpegSystemHeader,8192);
  }
  if (layer==_PACKET_SYSLAYER) {
    int packetID=mpegSystemHeader->getPacketID();
    int packetLength=mpegSystemHeader->getPacketLen();
    int subStreamID=mpegSystemHeader->getSubStreamID();

    switch(packetID>>4) {
    case _PAKET_ID_VIDEO>>4:
      if((packetID - _PAKET_ID_VIDEO) !=
	 mpegSystemHeader->getVideoLayerSelect()) {
	break;
      }
      
      insertVideoData(mpegSystemHeader,packetLength);
      //dumpData(mpegSystemHeader);
      return true;
      //break;
    case _PAKET_ID_AUDIO_1>>4:
    case _PAKET_ID_AUDIO_2>>4:
      if((packetID - _PAKET_ID_AUDIO_1) != 
	 mpegSystemHeader->getAudioLayerSelect()) {
	break;
      }
      insertAudioData(mpegSystemHeader,packetLength);
      return true;
    }
    switch(packetID) {
    case _PRIVATE_STREAM_1_ID:
      switch(subStreamID) {
      case _SUBSTREAM_AC3_ID:
	insertAudioData(mpegSystemHeader,packetLength);
	//dumpData(mpegSystemHeader);
	return true;
      default:
	printf("unknown private stream id:%8x\n",subStreamID);
      }
    }
       
    // if we got here the packet is not useful
    // nuke it
    nuke(packetLength);
    return true;

    
  }
  cout << "unknown layer"<<endl;
  return false;
}


void MpegStreamPlayer::nuke(int packetLength) {
  int nukeSize;
  while(packetLength > 0) {
    nukeSize=packetLength;
    if (nukeSize > _NUKE_BUFFER_SIZE) {
      nukeSize=_NUKE_BUFFER_SIZE;
    }
    input->read(nukeBuffer,nukeSize);
    packetLength-=nukeSize;
  }
}



int MpegStreamPlayer::isInit() {
  int audioDecoderState=audioDecoder->getStreamState();
  int videoDecoderState=videoDecoder->getStreamState();
  if ( (audioDecoderState != _STREAM_STATE_FIRST_INIT) &&
       (videoDecoderState != _STREAM_STATE_FIRST_INIT) ) {
    return true;
  }
  return false;
}

/**
   here we make sure that our plugins make
   a clean shutdown for a seek request.
*/
void MpegStreamPlayer::processThreadCommand(Command* command) {
  
  int id=command->getID();

  switch(id) {
  case _COMMAND_NONE:
    break;
  case _COMMAND_PAUSE:
  case _COMMAND_PLAY:
    audioDecoder->insertAsyncCommand(command);
    videoDecoder->insertAsyncCommand(command);
    break;
  case _COMMAND_CLOSE:
    audioDecoder->close();
    videoDecoder->close();
    break;
  case _COMMAND_SEEK: {
    Command cmd1(_COMMAND_PAUSE);
    audioDecoder->insertAsyncCommand(&cmd1);
    videoDecoder->insertAsyncCommand(&cmd1);

    Command cmd2(_COMMAND_RESYNC_START);
    audioDecoder->insertAsyncCommand(&cmd2);
    videoDecoder->insertAsyncCommand(&cmd2);
    
    seekPos=command->getIntArg();
    break;
  }
  default:
    cout << "unknown command id in Command::print"<<endl;
  }
}


void MpegStreamPlayer::setWriteToDisk(int lwriteToDisk) {
  this->writeToDisk=lwriteToDisk;
}


int MpegStreamPlayer::getWriteToDisk() {
  return writeToDisk;
}


int MpegStreamPlayer::hasEnd() {
  audioInput->close();
  videoInput->close();
  TimeWrapper::usleep(100000);
  if (audioInput->getFillgrade()>0) {
    return false;
  }
  if (videoInput->getFillgrade()>0) {
    return false;
  }
  return true;
}



int MpegStreamPlayer::insertAudioData(MpegSystemHeader* header,int len) {

  audioPacketCnt++;
  packetCnt++;
  timeStampAudio->setVideoFrameCounter(0);

  timeStampAudio->setPTSFlag(false);
  if (header->getPTSFlag()==true) {

    timeStampAudio->setPTSFlag(true);

    double pts=header->getPTSTimeStamp();
    double scr=header->getSCRTimeStamp();

    if (pts==timeStampAudio->getPTSTimeStamp()) {
      cout << "(audio) old PTS == NEW PTS"<<pts<<endl;
    }

    timeStampAudio->setSCRTimeStamp(scr);
    timeStampAudio->setPTSTimeStamp(pts);
  }

  timeStampAudio->setSyncClock(syncClock);


  finishAudio(len);

  return true;
}


int MpegStreamPlayer::insertVideoData(MpegSystemHeader* header,int len) {

  videoPacketCnt++;
  packetCnt++;
  timeStampVideo->setVideoFrameCounter(0);
  timeStampVideo->setPTSFlag(false);
  if (header->getPTSFlag()==true) {
    timeStampVideo->setPTSFlag(true);

    double pts=header->getPTSTimeStamp();
    double scr=header->getSCRTimeStamp();
    double dts=header->getDTSTimeStamp();
  
    if (pts==timeStampVideo->getPTSTimeStamp()) {
      cout << "(video) old PTS == NEW PTS"<<pts<<endl;
    }

    timeStampVideo->setSCRTimeStamp(scr);
    timeStampVideo->setPTSTimeStamp(pts);
    timeStampVideo->setDTSTimeStamp(dts);
  }


  timeStampVideo->setSyncClock(syncClock);

  finishVideo(len);

  return true;
} 

int MpegStreamPlayer::insertAudioDataRaw(unsigned char* input,
					 int len,TimeStamp* stamp) {
  audioInput->write((char*)input,len,stamp);
  if (writeToDisk==true) {
    FILE* audio=fopen("audio.mpg","a+");
    fwrite(input,1,len,audio);
    fclose(audio);
  }
  return true;
}


int MpegStreamPlayer::insertVideoDataRaw(unsigned char* input,
					 int len,TimeStamp* stamp) {

  videoInput->write((char*)input,len,stamp);
  if (writeToDisk==true) {
    FILE* video=fopen("video.mpg","a+");
    fwrite(input,1,len,video);
    fclose(video);
  }

  return true;
}
 

void MpegStreamPlayer::dumpData(MpegSystemHeader* mpegSystemHeader) {

  int packetLength=mpegSystemHeader->getPacketLen(); 

  unsigned char* packetBuffer= new unsigned char[packetLength];
  input->read((char*)packetBuffer,packetLength);
  int cnt;
  for(cnt=0;cnt < packetLength;cnt++) {
    printf(" %2x ",packetBuffer[cnt]);
    if ((cnt+1)%16 == 0) {
      printf("\n");
    }
  }

  printf("\n");
  cout << "**************************************** packt Dump"<<endl;
}



int MpegStreamPlayer::finishAudio(int len) {
  int resyncState=audioDecoder->getStreamState();
  if (resyncState != _STREAM_STATE_RESYNC_COMMIT) {
    if (writeToDisk==true) {
      char* buf=new char[len];
      len=input->read(buf,len);
      insertAudioDataRaw((unsigned char*) buf,len,timeStampAudio);
      delete buf;
    } else {
      audioInput->write(input,len,timeStampAudio);
    }

  } else {
    //cout << "throw away audio"<<endl;
  }
  return true;
}


int MpegStreamPlayer::finishVideo(int len) {
  int resyncState=videoDecoder->getStreamState();
  if (resyncState != _STREAM_STATE_RESYNC_COMMIT) {
    if (writeToDisk==true) {
      char* buf=new char[len];
      len=input->read(buf,len);
      insertVideoDataRaw((unsigned char*)buf,len,timeStampVideo);
      delete buf;
    } else {
      videoInput->write(input,len,timeStampVideo);
    }

  } else {
    //cout <<"throw away video"<<endl;
  }
  return true;
}

