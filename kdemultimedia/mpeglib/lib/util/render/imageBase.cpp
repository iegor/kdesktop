/*
  base clase for X11 images (stores important parameters and dither pic)
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "imageBase.h"

#include <iostream>

using namespace std;

ImageBase::ImageBase() {
	identifier = NULL;
}


ImageBase::~ImageBase() {
	if (identifier != NULL)
		delete [] identifier;
}

void ImageBase::init(XWindow* xWindow, YUVPicture*)
{
  cout << "direct virtual call: ImageBase::init  "<<endl;
}

int ImageBase::support() {
  cout << "direct virtual call: ImageBase::support  "<<endl;
  return false;
}

int ImageBase::openImage(int)  {
  cout << "direct virtual call: ImageBase::openImage  "<<endl;
  return false;
}


int ImageBase::closeImage(){
  cout << "direct virtual call: ImageBase::closeImage  "<<endl;
  return false;
}

void ImageBase::ditherImage(YUVPicture*) {
  cout << "direct virtual call: ImageBase::ditherImage  "<<endl;
}


void ImageBase::putImage() {
  cout << "direct virtual call: ImageBase::putImage  "<<endl;
}

void ImageBase::putImage(int ,int ) {
  cout << "direct virtual call: ImageBase::putImage(w,h)  "<<endl;
}

void ImageBase::setIdentifier(const char *id)
{
	if (identifier != NULL)
		delete [] identifier;

	identifier = new char [strlen(id) + 1];
	strcpy(identifier, id);
}

char *ImageBase::getIdentifier()
{
	return identifier;
}
