/*

  Example how the plugin interface works.(LINUX Open Sound System only!)





 */


#include "../../lib/decoder/tplayPlugin.h"
#include "../../lib/decoder/nukePlugin.h"


#include <iostream>

using namespace std;

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
    
  DecoderPlugin* plugin=new TplayPlugin();
  //DecoderPlugin* plugin=new NukePlugin();
  OutputStream* out=OutPlugin::createOutputStream(_OUTPUT_LOCAL,true);
  InputStream* in=InputPlugin::createInputStream(argv[1],true);

  // The plugin does not do "open"
  in->open(argv[1]);

  // watch the order!
  plugin->setOutputPlugin(out);
  plugin->setInputPlugin(in);



  plugin->play();

  while(plugin->getStreamState() != _STREAM_STATE_EOF) {
    TimeWrapper::sleep(1);
  }
  cout << "plugin eof"<<endl;
  plugin->close();

  delete plugin;
  delete in;
  delete out;
  return(0);
}
  
