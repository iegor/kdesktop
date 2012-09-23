/*
 * main.c --
 *
 * Example program for mpegplay library.
 * Build with : 
  BUILD with:(if you have intel mmx)
  

  g++ -DINTEL -I/usr/X11R6/include -I.. \
  -o mpegplay mainMpegPlay.cpp -L/usr/X11R6/lib \
  ../.libs/libmpeg.a  -lX11 -lXext -lXv -lXxf86dga \
  -lXxf86dga -lpthread

 */


#include "mpegPlugin.h"
#include "../output/outPlugin.h"
#include "mpegVideoLength.h"

// Includes for non plugin version
#define _NO_PLUGIN_VERSION
//#define _PLUGIN_VERSION


#ifdef _NO_PLUGIN_VERSION

int main(int argc, char** argv) {



  if (argc <= 1) {
    printf("Usage:\n\n");
    printf("%s filename\n\n",argv[0]);
    exit(0);
  }
  cout << "open -s 1"<<endl;
    
  InputStream* input=InputPlugin::createInputStream(argv[1]);
  OutputStream* output=OutPlugin::createOutputStream(_OUTPUT_LOCAL);

  output->config("performance","true",NULL);
  cout << "open -s"<<endl;
  input->open(argv[1]);
  //loader->seek(1024*1024*364+1024*600);
  //loader->seek(1024*1024*333);

  MpegVideoLength* mpegVideoLength=new MpegVideoLength(input);


  cout << "START length calc"<<endl;
  while (mpegVideoLength->firstInitialize()==false) {
    if (input->eof()) {
      break;
    }
    continue;
  }
  int len=mpegVideoLength->getLength();
  cout << "END length calc"<<endl;


  MpegVideoStream* mpegVideoStream=new MpegVideoStream(input);
  MpegVideoHeader* mpegVideoHeader=new MpegVideoHeader();
  cout << "start init"<<endl;
  while (mpegVideoStream->firstInitialize(mpegVideoHeader)==false) {

    if (input->eof()) {
      break;
    }
  }

  VideoDecoder* video;
  mpegVideoHeader->print("start");
  cout << "**************"<<endl;
  
  PictureArray* pictureArray;
  YUVPicture* pic;

  // now create pictureArray from the sequence
  int width=mpegVideoHeader->getMB_Width()*16;
  int height=mpegVideoHeader->getMB_Height()*16;
  cout << "width:"<<width<<" height:"<<height<<endl;
  
  output->openWindow(width,height,(char*)"kmpg");

  video= new VideoDecoder(mpegVideoStream,mpegVideoHeader);
  
  
  // init is true
  int cnt=0;
  while (input->eof()==false){
    pictureArray=output->lockPictureArray();
    video->mpegVidRsrc(pictureArray);
    pic=pictureArray->getYUVPictureCallback();
    if (pic == NULL) {
      // nothin to display
      continue;
    }

    output->unlockPictureArray(pictureArray);
    pictureArray->setYUVPictureCallback(NULL);

  }
  cout << "DestroyVideoDecoder"<<endl;
  delete video;


  delete mpegVideoHeader;
  delete mpegVideoStream;
  cout << "end"<<endl;

}

#endif


/*

  threaded Plugin version

  ( not useful for gprof )
*/

#ifdef _PLUGIN_VERSION

int main(int argc, char** argv) {



  if (argc <= 1) {
    printf("Usage:\n\n");
    printf("%s filename\n\n",argv[0]);
    exit(0);
  }

  //
  // The order is important !!!!
  // 1. construct
  // 2. set Output
  // 3. open input
  // 4. set input
  // 
  // you cannot set the input _before_ the output 
  // in fact you can, but this gives you a segfault!
    
  MpegPlugin* plugin=new MpegPlugin();
  OutputStream* out=OutPlugin::createOutputStream(_OUTPUT_LOCAL);
  InputStream* in=InputPlugin::createInputStream(argv[1]);

  cout << "open -s 1"<<endl;

  // The plugin does not do "open"
  in->open(argv[1]);
  cout << "open -s 2"<<endl;

  // watch the order!
  plugin->setOutputPlugin(out);
  cout << "open -s 3"<<endl;
  plugin->setInputPlugin(in);
  cout << "open -s 4"<<endl;

  plugin->play();
  int cnt=0;
  while(plugin->getStreamState() != _STREAM_STATE_EOF) {
    sleep(1);
    PluginInfo* pluginInfo=plugin->getPluginInfo();
    pluginInfo->print();
  }
  cout << "plugin eof"<<endl;
  plugin->close();

  delete plugin;
  delete in;
  delete out;
  
}

#endif

