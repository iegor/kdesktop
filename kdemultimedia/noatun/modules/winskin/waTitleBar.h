/*
  standard Button for winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __WATITLEBAR_H
#define __WATITLEBAR_H

#include <qpainter.h>

#include "waIndicator.h"


/**
   A titlebar is similar to a button only that we move
   the whole widget, when pressed
*/


class WaTitleBar : public WaIndicator {
    Q_OBJECT

  public:
     WaTitleBar();
    ~WaTitleBar();

  private:
    void mouseDoubleClickEvent (QMouseEvent * e);
    void mousePressEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * e);

    bool moving;
    QPoint mLastPos;
    QPoint mDragStart;

  signals:
    void shaded();

};
#endif
