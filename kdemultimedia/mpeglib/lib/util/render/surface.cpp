/*
  surface base class
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "surface.h"

#include <iostream>

using namespace std;

Surface::Surface() {
}


Surface::~Surface() {
}


int Surface::isOpen() {
  cout << "direct virtual call  Surface::isOpen "<<endl;
  return false;
}


int Surface::open(int width, int height,const char *title, bool border) {
  cout << "direct virtual call  Surface::open "<<endl;
  cout << "width:"<<width<<" height:"<<height
       << " title:"<<title<<endl;
  return false;
}


int Surface::close() {
  cout << "direct virtual call  Surface::close "<<endl;
  return true;
}


int Surface::getHeight() {
  cout << "direct virtual call  Surface::getHeight "<<endl;
  return 0;
}


int Surface::getWidth() {
  cout << "direct virtual call  Surface::getWidth "<<endl;
  return 0;
}


int Surface::getDepth() {
  cout << "direct virtual call  Surface::getDepth "<<endl;
  return 0;
}

int Surface::getImageMode() {
  cout << "direct virtual call  Surface::getImageMode "<<endl;
  return 0;
}

int Surface::x11WindowId() {
  cout << "direct virtual call  Surface::x11WindowId " << endl;
  return -1;
}

ImageBase *Surface::findImage(int)
{
  cout << "direct virtual call: Surface::findImage  "<<endl;
  return NULL;
}

int Surface::openImage(int mode, YUVPicture*) {
  cout << "direct virtual call  Surface::openImage "<<endl;
  cout << "imageMode:"<<mode<<endl;
  return false;
}


int Surface::closeImage() {
  cout << "direct virtual call  Surface::closeImage "<<endl;
  return true;
}


int Surface::dither(YUVPicture* pic) {
  cout << "direct virtual call  Surface::dither "<<endl;
  pic->print("Surface::dither");
  return false;
}


int Surface::putImage(YUVPicture* pic) {
  cout << "direct virtual call  Surface::putImage "<<endl;
  pic->print("Surface::putImage");
  return false;
}

int Surface::checkEvent(int*) {
  cout << "direct virtual call  Surface::checkEvent "<<endl;
  return false;
}

void Surface::config(const char* ,
		     const char* ,void* ) {
  cout << "direct virtual call Surface::config"<<endl;
}
 
