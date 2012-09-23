/*
  generic interactive controller 
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


// Set for local include
#define DIRECT_INCLUDE

#include "../yafcore/yaf_control.h"
#include "../yafxplayer/inputDecoderYAF.h"



#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// for dynamic loading
#include <dlfcn.h>

#include "../../../lib/decoder/vorbisPlugin.h"

#if defined(HAVE_GETOPT_H) 
#include <getopt.h>
#endif

using namespace std;

void control_vorbis(InputInterface* input,OutputInterface* output,
		   InputDecoder* decoder) {


  cout<< "Command:0 Msg:protocol yaf-0.1" << endl;
  cout<< "Command:0 Msg:implements xplayer" << endl;
  cout<< "Command:0 Msg:decoder vorbis Version:20000223" << endl;
  cout<< "Command:0 Msg:mimetypes audio/ogg;" << endl;
  cout<< "Command:0 Msg:comment vorbis by Monty http://www.xiph.org/" << endl;
  cout<< "Command:0 Msg:comment yaf port by mvogt@rhrk.uni-kl.de"<<endl;
  cout<< "Command:0 Msg:comment based on sources from vorbis"<<endl;
  cout<< "Command:0 Msg:comment enter 'help' " << endl;
 

  
 
  
  yaf_control(input,output,decoder);
}


void usage() {
  cout << "yaf-vorbis is a interactive frontend for the vorbis decoder"<<endl;
  cout << "Usage : yaf-vorbis [url]"<<endl;
  cout << endl;
  cout << "-a : no internal audio device"<<endl;
  cout << "-y : autoplay off"<<endl;
  cout << endl;
  cout << "THIS SOFTWARE COMES WITH ABSOLUTELY NO WARRANTY! " \
       << "USE AT YOUR OWN RISK!"<<endl;
  cout << endl;
  cout << "for more help type 'help' in interactive mode"<<endl;
}



int main(int argc, char** argv) {
  int c;
  int lInternalAudio=true;


  pow(6.0,3.0);            // fixes bug in __math.h
  InputInterface* input=new InputInterface();
  OutputInterface output(&cout);

  YafOutputStream* yafOutput=new YafOutputStream(input);


  VorbisPlugin* plugin=new VorbisPlugin();

  plugin->setOutputPlugin(yafOutput);
  InputDecoderYAF decoder(plugin,yafOutput);



  while(1) { 
    c = getopt (argc, argv, "2amhy");
    if (c == -1) break;
    switch(c) {
    case 'a': {    
      lInternalAudio=false;
      break;
    }
    case 'h': {
      usage();
      exit(0);
    }
    case '2': {
      plugin->config("-2","true",NULL);
      break;
    }
    case 'y': {
      decoder.setAutoPlay(false);
      break;
    }
    case 'm': {
      plugin->config("-m","true",NULL);
      break;
    }
    case 'r': {
      plugin->config("runtime","on",NULL);
      break;
    }
    case 'c': {
      plugin->config("-c","true",NULL);
      break;
    }
    default:
      printf ("?? getopt returned character code 0%o ??\n", c);
      usage();
      exit(-1);
    }
  }
  if (optind < argc ) {
    Buffer buffer(300);
    buffer.append("open ");
    buffer.append(argv[optind]);
    buffer.append("\n");
    
    input->addInputLine(&buffer);

  }



  yafOutput->internalDevice(lInternalAudio);
  control_vorbis(input,&output,&decoder);
  delete plugin;
  delete yafOutput;

}


