/*
  State indicator for Winamp Skin
  Copyright (C) 2002 Ryan Cumming

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <waIndicator.h>

WaIndicator::WaIndicator(int widget_mapping, int enabled_mapping, int disabled_mapping) 
                         : WaWidget(widget_mapping)
{
    _state = false;
    _enabled_mapping = enabled_mapping;
    _disabled_mapping = disabled_mapping;
}

WaIndicator::~WaIndicator()
{
}

void WaIndicator::paintEvent(QPaintEvent *)
{
    paintPixmap( _state ? _enabled_mapping : _disabled_mapping );
}


#include "waIndicator.moc"
