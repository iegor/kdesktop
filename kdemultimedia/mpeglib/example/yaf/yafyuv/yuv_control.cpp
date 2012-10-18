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
#include "../../../lib/yuv/yuvPlugin.h"

#if defined(HAVE_GETOPT_H) 
#include <getopt.h>
#endif


void control_yuv(InputInterface* input,OutputInterface* output,
		 InputDecoder* decoder) {


  cout<< "Command:0 Msg:protocol yaf-0.1" << endl;
  cout<< "Command:0 Msg:implements xplayer" << endl;
  cout<< "Command:0 Msg:decoder yuv Version:0.0.2" << endl;
  cout<< "Command:0 Msg:mimetypes vide/yuv1;" << endl;
  cout<< "Command:0 Msg:comment yuv Plugin by Martin Vogt" << endl;
  cout<< "Command:0 Msg:comment yaf port by mvogt@rhrk.uni-kl.de"<<endl;
  cout<< "Command:0 Msg:comment enter 'help' " << endl;
 
 
  
  yaf_control(input,output,decoder);
}


void usage() {
  cout << "yaf-yuv is a interactive frontend for the still image decoder"
       <<endl;
  cout << "Usage : yaf-yuv [w:g:y:f:hr:] <file.raw> [url]"<<endl;
  cout << endl;
  cout << "-w : width of raw yuv stream in pixel"<<endl;
  cout << "-g : height of raw yuv stream in pixel"<<endl;
  cout << "-h : help"<<endl;
  cout << "-y : set image Format:"<<endl;
  cout << "                      1   : yuv lum/Cr/Cb"<<endl;
  cout << "                      2   : yuv lum/Cb/Cr"<<endl;
  cout << "                      3   : rgb 24 BIT"<<endl;
  cout << "                      4   : rgb 24 BIT (flipped)"<<endl;

  cout << "-f : [1..n] frames per second"<<endl;

  cout << endl;
  cout << "THIS SOFTWARE COMES WITH ABSOLUTELY NO WARRANTY! " \
       << "USE AT YOUR OWN RISK!"<<endl;
  cout << endl;
  cout << "for more help type 'help' in interactive mode"<<endl;
}


int readFormat(int* w,int* h,char* filename) {
  char formatFileName[100];
  FILE* file=NULL;
  snprintf(formatFileName,100,"%s.format",filename);
  cout << "open:"<<formatFileName<<endl;
  file=fopen(formatFileName,"r");
  if (file == NULL) {
    cout << "no format file found"<<endl;
    return false;
  }

  if (fscanf(file,"Version 0.1\nw:%dh:%d\n",w,h)==2) {
    cout << "read ! w:"<<*w<<"h:"<<*h<<endl;
    return true;
  }

  
  cout << "format file mismatch"<<endl;
  return false;
}


int main(int argc, char** argv) {
  int c;
  int lInternalAudio=true;
  char* width=NULL;
  char* height=NULL;
  int fps=1;

  pow(6.0,3.0);            // fixes bug in __math.h



  
  InputInterface* input=new InputInterface();

  OutputInterface output(&cout);

  YafOutputStream* yafOutput=new YafOutputStream(input);

  YUVPlugin* plugin=new YUVPlugin();

  plugin->setOutputPlugin(yafOutput);
  InputDecoderYAF decoder(plugin,yafOutput);
  


  while(1) { 
    c = getopt (argc, argv, "w:g:y:hf:");
    if (c == -1) break;
    switch(c) {
    case 'a': {    
      lInternalAudio=false;
      break;
    }
    case 'h': {
      cout << "query for help"<<endl;
      usage();
      exit(0);
    }
    case 'w': {    
      width=optarg;
      printf("setting width:%s\n",width);
      break;
    }
    case 'f': {    
      fps=atoi(optarg);
      if (fps <= 0) {
	cout << "wrong fps. must be > 0"<<endl;
	exit(0);
      }
      break;
    }
    case 'g': {
      height=optarg;
      printf("setting height:%s\n",height);
      break;
    }
    case 'y': {
      cout << "config:imageType:"<<optarg<<endl;
      plugin->config("imageType",optarg,NULL);
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
  if ((width == NULL) || (height == NULL)) {
    int w;
    int h;
    cout << "automatic try to read a .format file"<<endl;
    if (readFormat(&w,&h,argv[optind]) == false) {
      cout << "no format specified"<<endl;
      exit(-1);
    }
    width=new char[20];
    height=new char[20];
    snprintf(width,20,"%d",w);
    snprintf(height,20,"%d",h);
  }

  char* picPerSec=new char[20];
  snprintf(picPerSec,20,"%d",fps);
  
 
  plugin->config("picPerSec",picPerSec,NULL);
  plugin->config("height",height,NULL);
  plugin->config("width",width,NULL);
 

  yafOutput->internalDevice(lInternalAudio);
  control_yuv(input,&output,&decoder);
  delete plugin;
  delete yafOutput;
  delete input;

}

