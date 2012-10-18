/*
  jumpslider for winamp skins
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __WAJUMPSLIDER_H
#define __WAJUMPSLIDER_H

#include "waSlider.h"
#include "waWidget.h"

class WaJumpSlider : public WaWidget {
    Q_OBJECT

  public:
     WaJumpSlider();
    ~WaJumpSlider();
    void buildGui();

    void setJumpRange(int val);

    void setJumpValue(int val);
    int jumpValue();

    void cancelDrag();

  protected:
    WaSlider *ws;
    void showEvent (QShowEvent *);
    void hideEvent (QHideEvent *);

  private slots:
    void releasedSlider();

  signals:
    void jump(int seconds);
    void sliderPressed();
    void sliderReleased();
    void valueChanged(int);
};


#endif
