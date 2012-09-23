/*
  jumpslider for winamp skins
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __WAVOLUMESLIDER_H
#define __WAVOLUMESLIDER_H

#include "waSlider.h"
#include "waWidget.h"

class WaVolumeSlider:public WaWidget {
    Q_OBJECT 

  public:
     WaVolumeSlider();
    ~WaVolumeSlider();
    void buildGui();

    void setVolumeValue(int val);

  private:
    WaSlider *ws;

  signals:
    void volumeSetValue(int val);
    void sliderPressed();
    void sliderReleased();
};


#endif
