/*
  jumpslider for winamp skins
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "waJumpSlider.h"

WaJumpSlider::WaJumpSlider() : WaWidget(_WA_MAPPING_POS_BAR)
{
}

WaJumpSlider::~WaJumpSlider()
{
}

void WaJumpSlider::buildGui()
{
    ws = new WaSlider(_WA_MAPPING_POS_BAR, _WA_MAPPING_POS_BAR_SLIDER);
    ws->setPixmapSliderButtonUp(_WA_SKIN_POS_BAR_SLIDER_NORM);
    ws->setPixmapSliderButtonDown(_WA_SKIN_POS_BAR_SLIDER_PRES);
    ws->setPixmapSliderBar(_WA_SKIN_POS_BAR);
    ws->setRange(0, 100);
    ws->setValue(0);

    connect(ws, SIGNAL(sliderPressed()), this, SIGNAL(sliderPressed()));
    connect(ws, SIGNAL(sliderReleased()), this, SLOT(releasedSlider()));
    connect(ws, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));
}

void WaJumpSlider::setJumpRange(int val)
{
    if (val == -1)
        ws->hideButton();
    else {
        ws->showButton();
        ws->setRange(0, val);
    }
}

int WaJumpSlider::jumpValue() {
    return ws->value();
}

void WaJumpSlider::setJumpValue(int val)
{
    int currVal = ws->value();

    if (currVal != val) {
	ws->setValue(val);
    }
}

void WaJumpSlider::releasedSlider() {
    emit(jump(ws->value()));
    emit(sliderReleased());
}

void WaJumpSlider::showEvent (QShowEvent *) {
    ws->show();
}

void WaJumpSlider::hideEvent (QHideEvent *) {
    ws->hide();
}

void WaJumpSlider::cancelDrag() {
    ws->cancelDrag();
}

#include "waJumpSlider.moc"
