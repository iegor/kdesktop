/*
  generic interactive controller 
  Copyright (C) 1998  Martin Vogt

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
using namespace std;

#include <math.h>



// we include our plugin here
#include "../../../lib/decoder/splayPlugin.h"

#if defined(HAVE_GETOPT_H) 
#include <getopt.h>
#endif


void control_splay(InputInterface* input,OutputInterface* output,
		   InputDecoder* decoder) {


  cout<< "Command:0 Msg:protocol yaf-0.1" << endl;
  cout<< "Command:0 Msg:implements xplayer" << endl;
  cout<< "Command:0 Msg:decoder splay Version:0.8.2" << endl;
  cout<< "Command:0 Msg:mimetypes audio/mpg1;audio/mpg2;audio/mp3;" << endl;
  cout<< "Command:0 Msg:comment splay by Woo-jae Jung" << endl;
  cout<< "Command:0 Msg:comment yaf port by mvogt@rhrk.uni-kl.de"<<endl;
  cout<< "Command:0 Msg:comment based on sources from eMusic and splay"<<endl;
  cout<< "Command:0 Msg:comment enter 'help' " << endl;
 
 
  
  yaf_control(input,output,decoder);
}


void usage() {
  cout << "yaf-splay is a interactive frontend for the splay decoder"<<endl;
  cout << "Usage : yaf-splay [-2am] [url]"<<endl;
  cout << endl;
  cout << "-2 : playing with half frequency."<<endl;
  cout << "-m : force to mono."<<endl;
  cout << "-r : autoupdate musicinfo"<<endl;
  cout << "-a : no internal audio device"<<endl;
  cout << "-c : no time calculation"<<endl;
  cout << "-y : autoplay off"<<endl;
  cout << "-d : only decode"<<endl;
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


  SplayPlugin* plugin=new SplayPlugin();

  plugin->setOutputPlugin(yafOutput);
  InputDecoderYAF decoder(plugin,yafOutput);
  


  while(1) { 
    c = getopt (argc, argv, "2yamhd");
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
    case 'd': {
      plugin->config("-d","on",NULL);
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
  control_splay(input,&output,&decoder);
  delete plugin;
  delete yafOutput;
  delete input;

}


