/*
  balanceslider for winamp skins
  Copyright (C) 1999  Martin Vogt
  Copyright (C) 2001  Ryan Cumming

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __WABALANCESLIDER_H
#define __WABALANCESLIDER_H

#include <waSlider.h>
#include "waWidget.h"

class WaBalanceSlider : public WaWidget {
    Q_OBJECT 

  public:
     WaBalanceSlider();
    ~WaBalanceSlider();
    void buildGui();

    void setBalanceValue(int val);

  private:
    WaSlider *ws;

  signals:
    void balanceSetValue(int val);
    void sliderPressed();
    void sliderReleased();
};


#endif
