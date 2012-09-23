/*
  Standard slider for Winskin
  Copyright (C) 1999  Martin Vogt
  Copyright (C) 2002  Ryan Cumming

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#include <stdlib.h>

#include "waSlider.h"
#include "waSkinModel.h"

WaSlider::WaSlider(int sliderBarMapId, int sliderId, bool is_volume) : WaWidget(sliderBarMapId)
{
    this->sliderBarMapId = sliderBarMapId;
 
    lDragging = false;

    currentValue = 0;
    setRange(0, 100);

    slider_y = is_volume ? 1 : 0;
    slider_x = 0;

    slider_width = WaSkinModel::instance()->getMapGeometry(sliderId).width();

    slider_visible = true;
}


WaSlider::~WaSlider()
{
}

void WaSlider::setPixmapSliderBar(int pixId)
{
    this->sliderBarId = pixId;
    update();
}

void WaSlider::paintEvent(QPaintEvent *)
{
    // POSBAR.BMP does not have full height in all Winamp skins
    // Paint background before painting slider to be on the safe side
    if(sliderBarId == _WA_SKIN_POS_BAR)
        paintPixmap(-1);

    paintPixmap(sliderBarId, value());

    if (slider_visible) 
        paintPixmap(lDragging ? up_pixmap : down_pixmap, slider_x, slider_y);
}


void WaSlider::mouseMoveEvent(QMouseEvent * e)
{
    if (lDragging == false) {
        WaWidget::mouseMoveEvent(e);
	return; 
    }

    int newX = e->x() - pressPoint.x();

    if (newX < 0) 
	newX = 0;

    QSize size = sizeHint();

    int maxX = size.width() - slider_width;
    
    if(mapping == _WA_MAPPING_VOLUME_BAR) 
        maxX -= 3;
    
    if (newX > maxX)
        newX = maxX;
    
    int value = pixel2Value(newX);

    setValue(value);
}


void WaSlider::updateSliderPos(int value)
{
    if (value > maxValue) {
	value = maxValue;
    }
    if (value < minValue) {
	value = minValue;
    }

    int pixelPos = value2Pixel(value);
    slider_x = (pixelPos);
    
    update();
}

void WaSlider::mousePressEvent(QMouseEvent *e) {
    if (e->button() != LeftButton && e->button() != MidButton) {
        WaWidget::mousePressEvent(e);
        return;
    }

    int maxX = slider_x - slider_width;
    
    if(mapping == _WA_MAPPING_VOLUME_BAR) 
        maxX -= 3;
    
    if ((e->x() < slider_x) || (e->x() > (maxX))) {
        int newX = e->x();
        newX -= (slider_width / 2);
        setValue(pixel2Value(newX));
   }

   
   pressPoint.setX(e->x() - slider_x);
   lDragging = true;

   update();

   emit(sliderPressed());
}
	
void WaSlider::mouseReleaseEvent(QMouseEvent *e)
{
    if (!lDragging) {
        WaWidget::mouseReleaseEvent(e);
        return;
    }

    lDragging = false;
    update();

    emit(sliderReleased());
}


int WaSlider::pixel2Value(int xpos)
{
    QSize size = sizeHint();
    int min = abs(minValue);
    int max = abs(maxValue);

    int valuerange = min + max;
    int pixelrange = size.width() - slider_width;

    if(mapping == _WA_MAPPING_VOLUME_BAR) 
        pixelrange -= 3;
      
    return ((xpos * valuerange) / pixelrange) + minValue;
}

int WaSlider::value2Pixel(int value)
{
    QSize size = sizeHint();
    float min = (float) minValue;
    float max = (float) maxValue;
    float fmin = min;
    float fmax = max;
    if (min < 0) {
	fmin = -1 * fmin;
    }
    if (max < 0) {
	fmax = -1 * fmax;
    }
    float valuerange = fmin + fmax;
    float verhaeltnis = fmin / valuerange;
    float pixelrange = (float) (size.width() - slider_width);
    
    if(mapping == _WA_MAPPING_VOLUME_BAR) 
        pixelrange -= 3;
    
    float zeropoint = verhaeltnis * pixelrange;
    float anstieg = pixelrange / valuerange;

    float pixel = (float) value * anstieg + zeropoint;
    return (int) (pixel+0.5); // add 0.5 to round upwards if larger than x.5
}

void WaSlider::setRange(int min, int max) {
    minValue = min;
    maxValue = max;

    if (currentValue < min)
        currentValue = min;

    if (currentValue > max)
        currentValue = max;
}

void WaSlider::setValue(int value) {
    currentValue = value;

    updateSliderPos(currentValue);
    emit(valueChanged(value));
}

void WaSlider::cancelDrag() {
    lDragging = false;
    update();
}

#include "waSlider.moc"
