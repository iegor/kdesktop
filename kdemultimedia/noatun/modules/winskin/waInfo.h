/*
  standard Button for winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __WAINFO_H
#define __WAINFO_H

#include <qpainter.h>
#include <qtimer.h>

#include "waWidget.h"

class WaInfo : public WaWidget {
    Q_OBJECT

  public:
     WaInfo();
    ~WaInfo();

    void setText(QString song);
    QString text() const;
    void scrollerSetup();

  protected:
    void paintEvent(QPaintEvent * paintEvent);

    void mousePressEvent (QMouseEvent * e);
    void mouseMoveEvent (QMouseEvent * e);
    void mouseReleaseEvent (QMouseEvent * e);

    QPixmap *completePixmap;
    QString _text;
    QTimer *timer;
    int xScrollPos;
    int xScrollDirection;
    int xGrabbedPos;

  protected slots:
    void pixmapChange();
    void timeEvent();
};
#endif
