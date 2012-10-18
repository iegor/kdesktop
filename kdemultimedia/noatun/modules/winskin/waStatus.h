/*
  standard Button for winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __WASTATUS_H
#define __WASTATUS_H

#include <qpainter.h>

#include "waWidget.h"

enum status_enum {STATUS_PLAYING, STATUS_STOPPED, STATUS_PAUSED};

class WaStatus : public WaWidget {
    Q_OBJECT

  public:
     WaStatus();
    ~WaStatus();

    void setStatus(status_enum status) { _status = status; update(); }
    status_enum status() const { return _status; }

  private:
    void paintEvent(QPaintEvent * paintEvent);
 
    status_enum _status;
};
#endif
