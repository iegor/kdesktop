/*
  The digit for the time
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __WADIGIT_H
#define __WADIGIT_H

#include <qpainter.h>

#include "waWidget.h"

class WaDigit : public WaWidget {
    Q_OBJECT

  public:
     WaDigit();
    ~WaDigit();

    void setTime(QString time) { timeString = time; update(); }
    QString time() const { return timeString; }

    bool timeReversed() const { return reverse_time; }

  public slots:
    void paintEvent(QPaintEvent * paintEvent);

  private:
    void mousePressEvent(QMouseEvent* e);
    bool reverse_time;

    WaSkinModel *waSkinModel;
    QString timeString;

  signals:
    void digitsClicked();
};
#endif
