/*
  standard Button for winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "waStatus.h"
#include "waSkinModel.h"

WaStatus::WaStatus() : WaWidget(_WA_MAPPING_PLAYPAUS)
{
    _status = STATUS_STOPPED;
}

WaStatus::~WaStatus()
{
}

void WaStatus::paintEvent(QPaintEvent *)
{
    if (_status == STATUS_PLAYING) {
	paintPixmap(_WA_SKIN_PLAYPAUS_WORK_INDICATOR);
	paintPixmap(_WA_SKIN_PLAYPAUS_PLAY, 3, 0);
    }
    else if (_status == STATUS_STOPPED) {
	paintPixmap(_WA_SKIN_PLAYPAUS_FILLER);
	paintPixmap(_WA_SKIN_PLAYPAUS_STOP, 2 ,0); 
    }
    else if (_status == STATUS_PAUSED) {
	paintPixmap(_WA_SKIN_PLAYPAUS_FILLER);
	paintPixmap(_WA_SKIN_PLAYPAUS_PAUSE, 2, 0);
    }
}

#include "waStatus.moc"
