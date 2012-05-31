/*
 * main.c --
 *
 * Example program for mpegplay library.


 */


#include "../../lib/decoder/cddaPlugin.h"
#include "../../lib/input/inputPlugin.h"

#include <iostream>

using namespace std;

int main(int argc, char** argv) {


  if (argc <= 1) {
    printf("Usage:\n\n");
    printf("%s \"cdda://dev/<device> track:<nr>.cdda\n\n",argv[0]);
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
    
  CDDAPlugin* plugin=new CDDAPlugin();
  OutputStream* out=OutPlugin::createOutputStream(_OUTPUT_LOCAL);
  InputStream* in= InputPlugin::createInputStream(argv[1]);



  // The plugin does not do "open"
  in->open(argv[1]);





  // watch the order!
  plugin->setOutputPlugin(out);
  plugin->setInputPlugin(in);
  //plugin->seek(1950);
  cout << "hello 1 -s"<<endl;


  plugin->play();
  cout << "hello 1 -e"<<endl;


  while(1) {
    if (plugin->getStreamState() != _STREAM_STATE_EOF) {
      cout << "******* plugin->getStreamState() continue"<<endl;
      sleep(3);
      continue;
    }
    break;
  }
  cout << "********************plugin eof"<<endl;
  plugin->close();

  delete plugin;
  delete in;
  delete out;

}
 
