/*
  This class can waits for input on different istreams
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "multiReader.h"



MultiReader::MultiReader() {
  int i;

  buffer=new Buffer(201);
  for(i=0;i<_MAX_INPUT;i++) {
    lineInputArray[i]=new LineInput;
    lineInputArray[i]->tmpLineStack=new LineStack();
    lineInputArray[i]->empty=true;
  }
  script=new LineStack();
}



MultiReader::~MultiReader() {
  int i;
  for(i=0;i<_MAX_INPUT;i++) {
    delete lineInputArray[i]->tmpLineStack;
    delete lineInputArray[i];
  }
  delete script;
}



int MultiReader::add(int fd) {
  int nPos;
  nPos=getEmptySlot();
  
  if (nPos == -1) {
    return -1;
  }
  lineInputArray[nPos]->fd=fd;
  lineInputArray[nPos]->empty=false;
  return nPos;
}

void MultiReader::add(LineStack* aScript) {
  script->appendBottom(aScript);
}


void MultiReader::add(Buffer* aScript) {
  script->appendBottom(aScript->getData(),aScript->len());
}




void MultiReader::remove (int fd) {
  int nPos;
  nPos=getSlot(fd);
  
  if (nPos == -1) {
    return ;
  }
  lineInputArray[nPos]->empty=true;
}



void MultiReader::waitForLine() {
  while(hasLine() == false) {
    doSelect(NULL);
  }
}


void MultiReader::poll(struct timeval* timeout) {
  doSelect(timeout);
}



void MultiReader::doSelect(struct timeval* timeout) {
  int i;
  int ret;
  fd_set readfds;
  int nBytes;
  int maxFd=0;

  FD_ZERO(&readfds);
  for(i=0;i<_MAX_INPUT;i++) {
    if (lineInputArray[i]->empty == false) {
      FD_SET(lineInputArray[i]->fd,&readfds);
      if (lineInputArray[i]->fd > maxFd) {
	maxFd=lineInputArray[i]->fd;
      }
    }
  } 
  ret=select(maxFd+1,&readfds,NULL,NULL,timeout);
  if (ret < 0) {
    if (errno < 0) {
      perror("nach select multireader:");
      exit(0);
    }
  }
  if (ret == 0) return;
  
  for(i=0;i<_MAX_INPUT;i++) {
    if (lineInputArray[i]->empty == false) {
      if (FD_ISSET(lineInputArray[i]->fd,&readfds)) {
	nBytes=read(lineInputArray[i]->fd,buffer->getData(),200);
	if (nBytes == 0) {
	  perror("MultiReader:read error!");
	  exit(-1);
	}
	(buffer->getData())[nBytes]='\0';
	
	lineInputArray[i]->tmpLineStack->appendBottom(buffer->getData(),
						      nBytes);
	FD_CLR(lineInputArray[i]->fd,&readfds);
      }
    }
  } 
} 



int MultiReader::hasLine() {
  int i;
  LineStack* lineStack;        // owned by class
  
  if (script->hasLine() == true) {
    return true;
  }

  for(i=0;i<_MAX_INPUT;i++) {
    if (lineInputArray[i]->empty == false) {
      lineStack=lineInputArray[i]->tmpLineStack;
      if (lineStack->hasLine()) {
	return true;
      }
    }
  } 
  return false;
}


void MultiReader::getLine(Buffer* buffer) {
  int i;
  LineStack* lineStack;        // owned by class
  buffer->clear();

  if (script->hasLine()==true) {
    script->nextLine(buffer);
  } else {
    for(i=0;i<_MAX_INPUT;i++) {
      if (lineInputArray[i]->empty == false) {
	lineStack=lineInputArray[i]->tmpLineStack;
	
	if (lineStack->hasLine()) {
	  lineStack->nextLine(buffer);
	  return;
	}
      }
    } 
  }
  buffer->append("\n");
}




int MultiReader::getEmptySlot() {
  int i;

  for(i=0;i<_MAX_INPUT;i++) {
    if (lineInputArray[i]->empty == true) {
      return i;
    }
  }
  return -1;  
}


int MultiReader::getSlot(int fd) {
  int i;

  for(i=0;i<_MAX_INPUT;i++) {
    if (lineInputArray[i]->empty == false) {
      if (lineInputArray[i]->fd == fd) {
	return i;
      }
    }
  }
  return -1;
}
