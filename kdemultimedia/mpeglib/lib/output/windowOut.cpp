/*
  wrapper for X11 Window
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "windowOut.h"







WindowOut::WindowOut() {

  renderMachine=new RenderMachine();

}


WindowOut::~WindowOut() {
  delete renderMachine;
}

int WindowOut::openWindow(int width, int height,const char *title){
  return renderMachine->openWindow(width,height,title);
}

int WindowOut::x11WindowId() {
 return renderMachine->x11WindowId();
}

void WindowOut::flushWindow() {
  renderMachine->flushWindow();
}


void WindowOut::closeWindow() {
  renderMachine->closeWindow();
}


PictureArray* WindowOut::lockPictureArray() {
  return renderMachine->lockPictureArray();
}


void WindowOut::unlockPictureArray(PictureArray* pictureArray) {
  renderMachine->unlockPictureArray(pictureArray);
}



void WindowOut::config(const char* key, const char* value,void* user_data) {
  renderMachine->config(key,value,user_data);
}
