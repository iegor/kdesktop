/*
  standard Button fo winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#include <qbitmap.h>

#include "waButton.h"

WaButton::WaButton(int mapId) : WaWidget(mapId) 
{   
  _togglable = false;
  _toggled = false;
  pressed = false;
}


WaButton::~WaButton() {
}

void WaButton::setPixmapUp(int pixId) {
  nUpId=pixId;
}


void WaButton::setPixmapDown(int pixId) {
  nDownId=pixId;
}


void WaButton::setPixmapUpSelected(int pixId) {
  nUpIdSelected=pixId;
}


void WaButton::setPixmapDownSelected(int pixId) {
  nDownIdSelected=pixId;
}

void WaButton::paintEvent(QPaintEvent *) {  
  paintPixmap(getPixmapId());
}


void WaButton::mousePressEvent(QMouseEvent* e) {
    if (e->button() != LeftButton) {
        // We can't deal with it, but maybe the widget can do something clever
        WaWidget::mousePressEvent(e);
    }
    else {
        pressed = true;
        update();
    }
}

void WaButton::mouseReleaseEvent(QMouseEvent* e) {
    if (!pressed) {
        // We're not pressed, so just pass along the mouse event, it's not ours
        WaWidget::mouseReleaseEvent(e);
    }
    else {
        pressed = false;

        if (this->rect().contains(e->pos())){
            if (_togglable) {
                _toggled = !_toggled;
                emit(toggleEvent(_toggled));
            }

            emit(clicked());
        }
    }

    update();
}

int WaButton::getPixmapId() {
  if (_toggled == true) {
    if (pressed)
      return nDownIdSelected;
    else
      return nUpIdSelected;
  } 
  else {
    if (pressed)
      return nDownId;
    else
      return nUpId;
  }

  return -1; 
} 

#include "waButton.moc"
