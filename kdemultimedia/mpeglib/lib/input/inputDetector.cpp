/*
  returns inputtype for a given string
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "inputDetector.h"

#include <iostream>

using namespace std;

static const char *filestrglobal1="/";
static const char *filestrglobal2="./";

typedef struct prot2type_s {
  const char* name;
  int type;
} prot2type;

static prot2type protocols[]= { {"http:"   , __INPUT_HTTP },
				{"cdi:"    , __INPUT_CDI  },
				{"vcd:"    , __INPUT_CDI  },
				{"file:"   , __INPUT_FILE },
				{"cdda:"   , __INPUT_CDDA },
				{"audiocd:", __INPUT_CDDA },
				{NULL      , __INPUT_UNKNOWN} };


InputDetector::InputDetector() {
}


InputDetector::~InputDetector() {
}

int InputDetector::getProtocolPos(int type,const char* url) {
  prot2type* current=protocols;
  int i=0;
  while(current->name != NULL) {
    if (current->type == type) {
      int len=strlen(current->name);
      if (strncmp(url,current->name,len)==0) {
	return i;
      }
    }
    i++;
    current++;
  }
  return -1;
}

int InputDetector::getProtocolType(const char* url) {
  prot2type* current=protocols;
  int n=strlen(url);
  if (n > 0) {
    while(current->name != NULL) {
      int k=strlen(current->name);
      if (n >= k) {
	if (strncmp(url,current->name,k)==0) {
	  return current->type;
	}
      }
      current++;
    }
  }
  return __INPUT_UNKNOWN;
}

int InputDetector::getInputType(const char* url) {
  int back=__INPUT_FILE;

  if (url == NULL) {
    return back;
  }

  back=InputDetector::getProtocolType(url);
  if (back == __INPUT_UNKNOWN) {
    back = __INPUT_FILE;
  }

  return back;

}

char* InputDetector::removeProtocol(const char* url) {
  int type=InputDetector::getProtocolType(url);
  int n=strlen(url);
  if (n == 0) {
    return NULL;
  }
  if (type == __INPUT_UNKNOWN) {
    return strdup(url);
  }
  int pos=InputDetector::getProtocolPos(type,url);
  if (pos == -1) {
    return NULL;
  }
  const char* name=protocols[pos].name;
  int k=strlen(name);
  if (n >= k) {
    return strdup(url+k);
  }
  return NULL;
  
}

char* InputDetector::getExtension(const char* url) {
  if (url == NULL) {
    cout << "get url NULL"<<endl;
    return NULL;
  } 
  char* back=NULL;
  char* extStart=strrchr(url,'.');
  if (extStart != NULL) {
    cout << "extStart:"<<extStart<<endl;
    back=strdup(extStart);
  }
  return back;
}

char* InputDetector::removeExtension(const char* url,char* extension) {
  if (url == NULL) {
    cout << "removeExtension url NULL"<<endl;
    return NULL;
  }
  if (extension == NULL) {
    cout << "removeExtension extension NULL"<<endl;
    return strdup(url);
  }
  char* back=NULL;
  int nExt=strlen(extension);
  int nUrl=strlen(url);
  cout << "extension:"<<extension<<" url:"<<url<<endl;
  if (nUrl >= nExt) {
    if(strncmp(url+nUrl-nExt,extension,nExt)==0) {
      back=new char[nUrl-nExt+1];
      back[nUrl-nExt]=0;
      strncpy(back,url,nUrl-nExt);
    }
  }
  cout << "removeExt:"<<back<<endl;
  return back;
    
}

char* InputDetector::getWithoutExtension(const char* url) {
  char* extension=NULL;
  char* back=NULL;
  if (url == NULL) {
    return NULL;
  }
  extension=InputDetector::getExtension(url);
  if (extension == NULL) {
    back=strdup(url);
  } else {
    back=InputDetector::removeExtension(url,extension);
    delete extension;
  }
  return back;
}
  
char* InputDetector::getFilename(const char* url) {
  if (url == NULL) {
    return NULL;
  }
  char* startSlash=strrchr(url,'/');
  if (startSlash == NULL) {
    return NULL;
  }
  if (strlen(startSlash) == 1) {
    return NULL;
  }
  startSlash++;
  if (*startSlash == 0) {
    return NULL;
  }
  return strdup(startSlash);
}


char* InputDetector::removeSlash(const char* url) {
  return InputDetector::removeExtension(url,(char*)"/");
}
