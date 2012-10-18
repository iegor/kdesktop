/*
  mpg I video/audio player plugin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "mpegVideoLength.h"

#include "../mpegplay/mpegVideoStream.h"
#include "../mpegplay/gop.h"

#include <iostream>

using namespace std;

#define SEARCH_SIZE 1024*1024*6
#define SEEKWINDOW  1024*1024

MpegVideoLength::MpegVideoLength(InputStream* input) {

  this->input=input;
  this->mpegVideoStream=new MpegVideoStream(input);

  startGOP=new GOP();
  endGOP=new GOP();
  lengthGOP=new GOP();
  mpegVideoHeader=new MpegVideoHeader();


  lHasStart=false;
  lHasEnd=false;
  lHasStream=false;
  lHasResync=false;
  lHasSystemStream=false;
  lHasRawStream=false;
  lSysLayer=false;

  mpegSystemStream=new MpegSystemStream(input);
  mpegSystemHeader=new MpegSystemHeader();


  lCanSeek=input->seek(0);
  if (lCanSeek == false) {
    cout << "mpegVideoLength: stream does not support seek"<<endl;
  }
  realLength=input->getByteLength();
  upperEnd=realLength;
  if (realLength > 1024*1024*600) {
    upperEnd=1024*1024*600;
  }
}


MpegVideoLength::~MpegVideoLength() {
  delete startGOP;
  delete endGOP;
  delete lengthGOP;
  delete mpegVideoStream;
  delete mpegVideoHeader;
  delete mpegSystemHeader;
  delete mpegSystemStream;
}


/**
   This long and ugly functions initialize a reader from 
   where to get the time informations.
   All these switches deal with the problem, that we really should
   have never a while loop, because this makes the decoder
   thread unresponsive and can lead to deadlocks.
*/

int MpegVideoLength::firstInitialize() {

  // no seek means no length detection
  if (lCanSeek==false) {
    // no detection possible, initialized ready
    input->seek(0);
    return true;
  }
  // do we have already a reader from where to get time?
  if (lHasStream == false) {
    // still init system ?
    if (lHasSystemStream == false) {
      if (mpegSystemStream->firstInitialize(mpegSystemHeader) == true) {
	lHasSystemStream=true;

	// if non system, reset everything and work on raw stream
	if (mpegSystemHeader->getLayer() == _PACKET_SYSLAYER) {
	  lSysLayer=true;
	}
	if (lSysLayer == false) {
	  input->seek(0);
	}
      }
      return false;
    }
    // if working on syslayer level we dont need the raw stream
    // set it to true
    if (lSysLayer == true) {
      lHasRawStream=true;
    }
    if (lHasRawStream == false) {
      if (mpegVideoStream->firstInitialize(mpegVideoHeader) == true) {
	lHasRawStream=true;
      }
      return false;
    }
    lHasStream=true;
    return false;
  }
  if (lHasStart == false) {
    if (seekToStart() == true) {
      lHasStart=true;
    }

    // clear and jump near the end
    mpegVideoStream->clear();
    int success=input->seek(upperEnd-SEARCH_SIZE);
    if (success == false) {
      cout << "mpegVideoStreamStream does not support seek"<<endl;
      // we can't find an upper end, thus we are ready
      input->seek(0);
      return true;
    }
    return false;
  }
  if (lHasResync==false) {
    if (lSysLayer) {
      if (mpegSystemStream->nextPacket(mpegSystemHeader) == false) {
	return false;
      } 
    } else {
      if (mpegVideoStream->nextGOP()==false) {
	return false;
      }
    }
    lHasResync=true;
    return false;
  }
  if (lHasEnd == false) {
    if (seekToEnd() == true) {
      lHasEnd=true;
      if (endGOP->substract(startGOP,lengthGOP) == false) {
	cout << "substract error in final length detection"<<endl;
	if (startGOP->substract(endGOP,lengthGOP) == true) {
	  cout << "this stream counts the time backwards"<<endl;
	} else {
	  cout << "couldnt determine stream length"<<endl;
	  GOP dummy;
	  dummy.copyTo(lengthGOP);
	}
      }
      // ok now we have the length but we must calculate
      // the length of the whole stream
      // we do not jump ofer 600 MB because this makes problems
      // on some cdrom.
      // here we calculate the real length if upperEnd != realEnd
      int hour=lengthGOP->getHour();
      int minute=lengthGOP->getMinutes();
      long seconds=lengthGOP->getSeconds();
      seconds=seconds+minute*60+hour*60*60;
      if (upperEnd > 1) {
	if (realLength > upperEnd) {
	  float ratio=realLength/upperEnd;
	  float realSeconds=seconds*ratio;
	  hour=(int)(realSeconds/(float)(60*60));
	  realSeconds=realSeconds-hour*60*60;
	  minute=(int)(realSeconds/60.0);
	  realSeconds=realSeconds-minute*60;
	  seconds=(int)realSeconds;
	  lengthGOP->setHour(hour);
	  lengthGOP->setMinute(minute);
	  lengthGOP->setSecond(seconds);
	}
      }
    }
  }
  input->seek(0);
  return true;
}


long MpegVideoLength::getLength() {
  long back=0;
  back=lengthGOP->getHour()*60*60;
  back+=lengthGOP->getMinutes()*60;
  back+=lengthGOP->getSeconds();

  // length in second
  return back;
}


long MpegVideoLength::getSeekPos(int seconds) {
  // calculate from seconds to predicted position in stream
  long back=0;
  double ratio;

  ratio=(double)realLength*(double)seconds;
  ratio=ratio/((double)getLength()+1.0);
  back=(long)ratio;

  return back;
    
}


// We try to search the first SEARCH_SIZE KB for a valid picture start.
int MpegVideoLength::seekToStart() {
  int success;

  /**
     If we are a system Layer we calculate the startGOP
     by the system Packets
  */
  if (lSysLayer == true) {
    success=parseToPTS(startGOP);
  } else {
  
    mpegVideoStream->hasBytes(100);

    success=parseToGOP(startGOP);
  }

  if (success == false) {
    cout << "picture startcode not found [START]"<<endl;
    
  }
  // we return always true
  // if we really have the start, fine, otherwithe we can
  // nothing do about it, nor yet, nor in future, thus
  // we have even success (in terms of failure) 
  return true;
}



int MpegVideoLength::seekToEnd() {
  int success;

  if (lSysLayer == true) {
    success=parseToPTS(endGOP);
  } else {
    mpegVideoStream->hasBytes(100);
    success=parseToGOP(endGOP);
  }
  if (success == false) {
    cout << "picture endcode not found [END]"<<endl;
  }

  return true;
}


int MpegVideoLength::parseToGOP(GOP* dest) {
  int success;
  // we try ten attempts
  // and the diff between each must be (less) than one second

  int successCnt=0;


  long maxArea=0;
  long area=0;

  GOP lastGOP;
  GOP currentGOP;
  GOP diffGOP;


  while(successCnt < 4) {
    if (mpegVideoStream->eof()) {
      return false;
    }
    if (input->eof() == true) {
      cout << "abort"<<endl;
      return false;
    }
    if (maxArea > SEARCH_SIZE) {
      return false;
    }


    success=seekValue(GOP_START_CODE,area);
    maxArea+=area;
    if (success == false) {
      continue;
    }

    

    currentGOP.copyTo(&lastGOP);
    //    currentGOP.print("current");
    successCnt++;
    currentGOP.processGOP(mpegVideoStream);
    if (currentGOP.substract(&lastGOP,&diffGOP) == false) {
      cout << "substract error"<<endl;
    }
    /*
    currentGOP.print("current");
    lastGOP.print("last");
    diffGOP.print("diff");
    */
    if (diffGOP.getHour() != 0) {
      successCnt=0;
      continue;
    }
    if (diffGOP.getMinutes() != 0) {
      successCnt=0;
      continue;
    }
    if (diffGOP.getSeconds() > 8) {
      successCnt=0;
      continue;
    }   
    
  }
  currentGOP.copyTo(dest);
  return true;
}






int MpegVideoLength::seekValue(unsigned int ,long& valueSeeked) {
  long start=input->getBytePosition();
  long cnt=0;
  long end=start+SEEKWINDOW;
  long area=0;

  // if we have not enough space we skip
  // because of the mpeg frame garbage "mb_stuffing,etc..."

  if (end > upperEnd-SEEKWINDOW){
    valueSeeked=SEEKWINDOW;
    return false;
  }
  area=end-start;
  while(mpegVideoStream->nextGOP() == false) {
    if (mpegVideoStream->eof()) {
      return false;
    }
    cnt++;
    if (cnt >= area) {
      valueSeeked=cnt;
      cout << "nothing found"<<area<<endl;
      return false;
    }
  }
  return true;
}



int MpegVideoLength::parseToPTS(GOP* dest) {

  // we try ten attempts
  // and the diff between each must be (less) than one second

  int successCnt=0;

  long startArea=input->getBytePosition();
  long maxArea=0;


  double lastPTS=0;
  double currentPTS=0;
  double diffPTS=0;


  while(successCnt < 4) {
    if (input->eof() == true) {
      cout << "abort"<<endl;
      return false;
    }
    maxArea=input->getBytePosition()-startArea;
    if (maxArea > SEARCH_SIZE) {
      return false;
    }
    if (mpegSystemStream->nextPacket(mpegSystemHeader) == false) {
      continue;
    }
    if (mpegSystemHeader->getPTSFlag()==false) {
      continue;
    }
    // we have a packet
    lastPTS=currentPTS;
    currentPTS=mpegSystemHeader->getPTSTimeStamp();
    diffPTS=currentPTS-lastPTS;
    successCnt++;

    if (diffPTS > 1.0) {
      successCnt=0;
      continue;
    }
  }

  // now put it into the gop structure 
  // this is the interface for the time.
  // a little hack here and there....
  unsigned int hour=((long)currentPTS) / 3600;
  currentPTS=currentPTS-hour*3600;
  unsigned int minute=((long)currentPTS) / 60;
  currentPTS=currentPTS-minute*60;
  unsigned int seconds=((long)currentPTS);


  dest->setHour(hour);
  dest->setMinute(minute);
  dest->setSecond(seconds);
  return true;
}


