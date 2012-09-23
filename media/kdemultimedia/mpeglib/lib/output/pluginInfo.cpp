/*
  add on information about plugin.
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "pluginInfo.h"

#include <iostream>

using namespace std;


PluginInfo::PluginInfo() {
  musicName=new DynBuffer(20);

  reset();
  
}


PluginInfo::~PluginInfo() {
  delete musicName;
}

void PluginInfo::setLength(int sec) {
  this->sec=sec;
}


int PluginInfo::getLength() {
  return sec;
}


void PluginInfo::reset(){
  sec=0;
  musicName->clear();
  musicName->append("none");
}

void PluginInfo::print() {
  cerr << "length in sec:"<<sec<<endl;
  cerr << "url:"<<getUrl()<<endl;
}


void PluginInfo::setUrl(char* name) {
  musicName->clear();
  if (name != NULL) {
    musicName->append(name);
  }
}


char* PluginInfo::getUrl() {
  return musicName->getData();
}
