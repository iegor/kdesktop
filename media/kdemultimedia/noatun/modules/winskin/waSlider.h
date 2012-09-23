/*
  standard Slider for winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __WASLIDER_H
#define __WASLIDER_H

#include <qpainter.h>

#include "waWidget.h"
#include "waButton.h"

class WaSlider : public WaWidget {
    Q_OBJECT

  public:
     WaSlider(int sliderBarMapId, int sliderId, bool is_volume = false);
    ~WaSlider();

    void setRange(int min, int max);    

    void setValue(int value);
    int value() const { return currentValue; }

    void hideButton() { slider_visible = false; }
    void showButton() { slider_visible = true; }

    void cancelDrag();

  public slots:
    void setPixmapSliderButtonDown(int pixId) { down_pixmap = pixId; }
    void setPixmapSliderButtonUp(int pixId) { up_pixmap = pixId; }
    void setPixmapSliderBar(int pixId);

  private:
    void paintEvent(QPaintEvent *);
    int pixel2Value(int xpos);
    int value2Pixel(int value);

    int slider_x;
    int slider_y;
    int slider_width;
    bool slider_visible;

    int up_pixmap;
    int down_pixmap;

    int sliderBarId;
    int sliderBarMapId;

    bool lDragging;
    QPoint pressPoint;

    int currentValue;

    int minValue;
    int maxValue;

    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

  private slots:
    void updateSliderPos(int value);

  signals:
    void sliderPressed();
    void sliderReleased();
    void valueChanged(int);
};
#endif
