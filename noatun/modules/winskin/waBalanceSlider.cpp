/*
  balanceslider for winamp skins
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <waBalanceSlider.h>


WaBalanceSlider::WaBalanceSlider() : WaWidget(_WA_MAPPING_BALANCE_BAR)
{
}


WaBalanceSlider::~WaBalanceSlider()
{
}


void WaBalanceSlider::buildGui()
{
    ws = new WaSlider(_WA_MAPPING_BALANCE_BAR,
		      _WA_MAPPING_BALANCE_SLIDER, true);

    ws->setRange(-100, 100);


    ws->setPixmapSliderButtonUp(_WA_SKIN_BALANCE_SLIDER_NORM);
    ws->setPixmapSliderButtonDown(_WA_SKIN_BALANCE_SLIDER_PRES);
    ws->setPixmapSliderBar(_WA_SKIN_BALANCE_BAR);

    ws->setValue(0);

    connect(ws, SIGNAL(valueChanged(int)), this,
	    SIGNAL(balanceSetValue(int)));
    connect(ws, SIGNAL(sliderPressed()), SIGNAL(sliderPressed()));
    connect(ws, SIGNAL(sliderReleased()), SIGNAL(sliderReleased()));
}


void WaBalanceSlider::setBalanceValue(int val)
{
    int currVal = ws->value();
    if (currVal != val) {
	ws->setValue(val);
    }
}

#include "waBalanceSlider.moc"
