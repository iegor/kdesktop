/**
   graps cdis

   Compile with
  
   g++ -o cdigrap cdigrap.cpp -lmpeg

*/
#ifdef CONFIG_H
#include "config.h"
#endif
#include "inputPlugin.h"

#if defined(HAVE_GETOPT_H)
#include <getopt.h>
#endif

void usage() {
  cout << "cdigrab grabs video cds"<<endl;
  cout << "Usage : cdigrab [s:b:f:h] cdi:/device"<<endl;
  cout << endl;
  cout << "-s : bytes start a positions <bytes>"<<endl;
  cout << "-b : set block size (default: 32768)"<<endl;
  cout << "-f : set filename (default: a.cdi)"<<endl;
  cout << "-h : help"<<endl;
  cout << "THIS SOFTWARE COMES WITH ABSOLUTELY NO WARRANTY! " \
       << "USE AT YOUR OWN RISK!"<<endl;
  cout << endl;
}


int main(int argn,char** args) {


  if (argn <= 1) {
    usage();
    exit(0);
  }
  long startBytes=0;
  int len=32768;
  char *fname = strdup("a.cdi");
  int c;

  while(1) {
    c = getopt (argn, args, "s:b:f:h");
    if (c == -1) break;
    switch(c) {
    case 'h': {
      usage();
      exit(0);
    }
    case 's': {
      startBytes=atoi(optarg);
      break;  
    }
    case 'b': {
      len=atoi(optarg);
      break;  
    }
    case 'f': {
      fname = strdup(optarg);
      break;  
    }
    default:
      printf ("?? getopt returned character code 0%o ??\n", c);
      usage();
      exit(-1);
    }
  }
  if (optind >= argn ) {
    usage();
    exit(-1);
  }

  InputStream* in=InputPlugin::createInputStream(args[optind]);

  in->open(args[optind]);

  if (startBytes != 0) {
    cout << "seeking to :"<<startBytes<<endl;
    in->seek(startBytes);	
  }
  char* buffer=new char[len];
  int cnt=0;
  FILE* f=fopen(fname,"a+");
  while(1) {
    if (in->eof() == true) {
      cout << "******* plugin->getStreamState() EOF"<<endl;
      break;
    }
    in->read(buffer,len);
    fwrite(buffer,len,1,f);
    cnt++;
    cout << "grapped:"<<cnt*len<<endl;
  }
  fclose(f);
}



