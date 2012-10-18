/*
  standard Button for winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#ifndef __WABPS_H
#define __WABPS_H

#include <qpainter.h>
#include "waWidget.h"

class WaLabel : public WaWidget {
    Q_OBJECT 

  public:
     WaLabel(int mapping);
    ~WaLabel();

    void setText(const QString &text);
    QString text() const { return _text; }

  private:
    void paintEvent(QPaintEvent *);

    QPixmap *completePixmap;
    QString _text;


  private slots:
    void pixmapChange();
};
#endif
