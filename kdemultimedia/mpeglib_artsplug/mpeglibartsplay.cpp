// vim:ts=2:sw=2:sts=2:et
/**
   Starter for plugins.
   The plugins are identified by their extension

   .wav ->WavPlayObject
   .mp3 ->MP3PlayObject
   .mpg ->MPGPlayObject
   .ogg ->OGGPlayObject
*/


#include "soundserver.h"
#include "kmedia2.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include <climits>

using namespace std;

#if defined(HAVE_GETOPT_H) 
#include <getopt.h>
#endif


string file1;
string file2;

Arts::Dispatcher* d=0;
Arts::SimpleSoundServer* server=0;



void usage() {
  cout << "mpeglibartsply command line tool for arts playobjects"<<endl;
  cout << "Usage : mpeglibartsply [url]"<<endl;
  cout << endl;
  cout << "-1 : create/destroy PlayObject test."<<endl;
  cout << "-2 : create/seek/play/destroy PlayObject test."<<endl;
  cout << "-3 : torture seek implementation test"<<endl;
  cout << "-h : help"<<endl;
  cout << endl;
  cout << "THIS SOFTWARE COMES WITH ABSOLUTELY NO WARRANTY! " \
       << "USE AT YOUR OWN RISK!"<<endl;
  cout << endl;
  exit(0);
}


void initServer() {
  d=new Arts::Dispatcher();
  server= new Arts::SimpleSoundServer (Arts::Reference("global:Arts_SimpleSoundServer"));
  
  if(server->isNull()) {
    cerr << "Can't connect to sound server" << endl;
    exit(0);
  }

}


void destroyServer() {
  delete server;
  delete d;
}

/**
   create destroy playobjects without playing anything
*/
void doStress1() {
  int cnt=0;
  while (cnt < 10000) {
    Arts::PlayObject play=server->createPlayObject(file1);
    cout << "cnt:"<<cnt++<<endl;
  }
}

/**
   create playobjects playing some time, do seek play destroy
*/
void doStress2() {
  int cnt=0;
  while (cnt < 1000) {
    Arts::PlayObject play=server->createPlayObject(file1);
    play.play();
    sleep(3);
    Arts::poTime seekTime;
    seekTime.seconds=100;
    seekTime.ms=0;
    play.seek(seekTime);
    cout << "cnt:"<<cnt++<<endl;
  }
}

/**
   create playobject seek random play seek again
*/
void doStress3() {
  int cnt=0;
  Arts::PlayObject play=server->createPlayObject(file1);
  play.play();
  long secs=0;
  while(1) {
    cout << "waiting for length info.."<<endl;
    sleep(3);
    Arts::poTime length=play.overallTime();
    secs=length.seconds;
    if (secs > 0) break;
  }
  // do not jump near to end, the danger that
  // we get an eof is to high
  if (secs < 100) {
    cout << "file to short for test"<<endl;
    exit(-1);
  }
  secs-=10;
  Arts::poTime seekTime;
  Arts::poTime startTime;
  startTime.seconds=0;
  startTime.ms=0;

  while (cnt < 1000) {

    //
    // seek to a know position
    //
    play.seek(startTime);
    // now wait for seek completion we need a poll here
    while(1) {
      Arts::poTime current=play.currentTime();
      if (current.seconds < 5) {
        break;
      }
      usleep(50000);
    }

    //
    // now we know that the playObject is near the beginning
    //

    int randNo=rand();
    long seekPos=(long)(((float)randNo*(float)secs)/(float)RAND_MAX);
    // need to jump over the start area

    seekPos+=5;
    cout << "seek to :"<<seekPos
	 << " | cnt:"<<cnt++
	 << " of 1000"<<endl;
    seekTime.seconds=seekPos;
    seekTime.ms=0;
    play.seek(seekTime);
    // now wait for seek completion we need a poll here
    while(1) {
      Arts::poTime current=play.currentTime();
      if (current.seconds > seekPos) {
	break;
      }
      if (current.seconds == seekPos) {
	if (current.ms > 0) {
	  break;
	}
      }      
      usleep(50000);
    }
    

  }
  cout << "stresstest successfully passed."<<endl;
}


int main(int argc, char **argv) {
  int testNr=0;
  int c;
  initServer();

  while(1) { 
    c = getopt (argc, argv, "123456h");
    if (c == -1) break;
    switch(c) {
    case '1': {    
      testNr=1;
      break;
    }
    case '2': {
      testNr=2;
      break;
    }
    case '3': {
      testNr=3;
      break;
    }
    case '4': {
      testNr=4;
      break;
    }
    case '5': {
      testNr=5;
      break;
    }
    case '6': {
      testNr=5;
      break;
    }
    case 'h': {
      usage();
      break;
    }
    default:
      printf ("?? getopt returned character code 0%o ??\n", c);
      usage();
      exit(-1);
    }
  }

  if (optind >= argc ) {
    usage();
    exit(-1);
  }

  if (optind < argc ) {
    file1=argv[optind];
    optind++;
    file2=file1;
  }
  if (optind < argc ) {
    file2=argv[optind];
  }  


  switch(testNr) {
  case 1: {
    doStress1();
    break;
  }
  case 2: {
    doStress2();
    break;
  }
  case 3: {
    doStress3();
    break;
  }
  default:
    if (file1[0] != '/') {
      char buf[PATH_MAX+1];
      char *path = getcwd(buf, PATH_MAX - file1.length());
      if (path) {
        file1.insert(0, "/");
        file1.insert(0, path);
      }
    }
    Arts::PlayObject play=server->createPlayObject(file1);
    if (play.isNull()) {
      cout << "cannot play this"<<endl;
      destroyServer();
      exit(0);
    }
    play.play();
    while (play.state() != Arts::posIdle) {
      sleep(1);
    }
  }

  destroyServer();
}

