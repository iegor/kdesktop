/*
  jumpslider for winamp skins
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <waVolumeSlider.h>


WaVolumeSlider::WaVolumeSlider() : WaWidget(_WA_MAPPING_VOLUME_BAR)
{

}


WaVolumeSlider::~WaVolumeSlider()
{
}


void WaVolumeSlider::buildGui()
{
    ws = new WaSlider(_WA_MAPPING_VOLUME_BAR, _WA_MAPPING_VOLUME_SLIDER, true);

    ws->setPixmapSliderButtonUp(_WA_SKIN_VOLUME_SLIDER_NORM);
    ws->setPixmapSliderButtonDown(_WA_SKIN_VOLUME_SLIDER_PRES);
    ws->setPixmapSliderBar(_WA_SKIN_VOLUME_BAR);

    connect(ws, SIGNAL(valueChanged(int)), this,
	    SIGNAL(volumeSetValue(int)));
    connect(ws, SIGNAL(sliderPressed()), SIGNAL(sliderPressed()));
    connect(ws, SIGNAL(sliderReleased()), SIGNAL(sliderReleased()));
}

void WaVolumeSlider::setVolumeValue(int val)
{
    int currVal = ws->value();
    if (currVal != val) {
	ws->setValue(val);
    }
}


#include "waVolumeSlider.moc"
