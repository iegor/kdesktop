/*
  Standard state indicator for Winamp skin
  Copyright (C) 2002  Ryan Cumming

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __WAINDICATOR_H
#define __WAINDICATOR_H

#include <qpainter.h>

#include "waWidget.h"

class WaIndicator : public WaWidget {
    Q_OBJECT

  public:
     WaIndicator(int widget_mapping, int enabled_mapping, int disabled_mapping);
    ~WaIndicator();

    void setState(bool state) { _state = state; update(); }
    bool state() const { return _state; }

  public slots:
    void paintEvent(QPaintEvent *);

  private:
    int _enabled_mapping;
    int _disabled_mapping;
    bool _state;
};
#endif
