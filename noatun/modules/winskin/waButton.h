/*
  standard Button for winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __WABUTTON_H
#define __WABUTTON_H

#include <qpainter.h>
#include "waWidget.h"

class WaButton : public WaWidget {
  Q_OBJECT
 public:
   WaButton(int mapId);
   ~WaButton();

   void setPixmapDown(int pixId);
   void setPixmapUp(int pixId);
   void setPixmapUpSelected(int pixId);
   void setPixmapDownSelected(int pixId);

   void setToggled(bool toggled_flag) { _toggled = toggled_flag; update(); }
   bool toggled() const { return _toggled; }
   
   void setTogglable(bool togglable_flag) { _togglable = togglable_flag; update(); }
   bool togglable() const { return _togglable; }

   int getPixmapId();
   void paintEvent(QPaintEvent*);
   
 private:
   void mousePressEvent (QMouseEvent* e);
   void mouseReleaseEvent (QMouseEvent* e);

  int nUpId;
  int nDownId;
  int nDownIdSelected;
  int nUpIdSelected;
  
  QPoint currentLocation;

  bool _toggled;
  bool _togglable;

  bool pressed;

 signals:
   void toggleEvent(bool val);
   void clicked();
};
#endif
